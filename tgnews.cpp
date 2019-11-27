/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/


#if defined(__linux__)
	#include <dirent.h>
	#include <stdint.h>

	#include <sys/types.h>
	#include <sys/stat.h>
	#include <stdio.h>
	#include <stdlib.h>
#endif

#if defined(_WIN64)
	#include <filesystem>
#endif

#include <vector>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>

#include "modules/language_detector.hpp"
#include "modules/news_detector.hpp"
#include "modules/categories_detector.hpp"
#include "modules/news_clusterizer.hpp"
#include "modules/news_ranger.hpp"

#include "3rdparty/json.hpp"


using json = nlohmann::json;

#if defined(_WIN64)
namespace  fs = std::filesystem;
#endif


const std::string LANGUAGES_MODE_COMMAND = "languages";
const std::string NEWS_MODE_COMMAND = "news";
const std::string CATEGORIES_MODE_COMMAND = "categories";
const std::string THREAD_MODE_COMMAND = "threads";
const std::string TOP_MODE_COMMAND = "top";

enum Mode { UNKNOWN_MODE, LANGUAGES_MODE, NEWS_MODE, CATEGORIES_MODE, THREAD_MODE, TOP_MODE };


////////////////////////////


std::vector<std::string> readFilePaths(std::string dirname, bool recursively = true)
{
	std::vector<std::string> path_names;

	#if defined(__linux__)
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dirname.c_str())) == NULL) {
			std::cout << "Error(" << errno << ") opening " << dirname << std::endl;
			return std::vector<std::string>();
		}

		while ((dirp = readdir(dp)) != NULL) 
		{
			if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
			{
				bool is_dir;
				#ifdef _DIRENT_HAVE_D_TYPE
				if (dirp->d_type != DT_UNKNOWN && dirp->d_type != DT_LNK) {
					// don't have to stat if we have d_type info, unless it's a symlink (since we stat, not lstat)
					is_dir = (dirp->d_type == DT_DIR);
				}
				else
				#endif
				{  // the only method if d_type isn't available,
					// otherwise this is a fallback for FSes where the kernel leaves it DT_UNKNOWN.
					struct stat stbuf;
					// stat follows symlinks, lstat doesn't.
					stat(dirp->d_name, &stbuf);              // TODO: error check
					is_dir = S_ISDIR(stbuf.st_mode);
				}

				if (is_dir) {
					if (recursively)
					{
						std::vector<std::string> dir_files = readFilePaths(dirname + "/" + std::string(dirp->d_name), recursively);
						path_names.insert(path_names.end(), dir_files.begin(), dir_files.end());
					}
				}
				else
				{
					path_names.push_back(dirname + "/" + std::string(dirp->d_name));
				}
			}
		}
		closedir(dp);
	#endif
	
	#if defined(_WIN64)
		for (const auto & entry : fs::directory_iterator(dirname))
		{
			std::error_code ec; // For using the non-throwing overloads of functions below.
			if (fs::is_directory(entry.path(), ec))
			{ 
				if (recursively)
				{
					std::vector<std::string> dir_files = readFilePaths(entry.path().string(), recursively);
					path_names.insert(path_names.end(), dir_files.begin(), dir_files.end());
				}
			}
			if (ec)
			{
				std::cerr << "Error in is_directory: " << ec.message();
			}
			if (fs::is_regular_file(entry.path(), ec))
			{
				path_names.push_back(entry.path().string());
			}
			if (ec) 
			{
				std::cerr << "Error in is_regular_file: " << ec.message();
			}
		}
	#endif

	return path_names;
}


std::vector<std::string> selectHtmlFiles(std::string dirname, bool recursively = true)
{
	std::vector<std::string> files_paths;

	std::vector<std::string> all_paths = readFilePaths(dirname);

	for (auto fp : all_paths)
	{
		if (fp.size() > 5 && fp.substr(fp.size() - 5) == ".html")
		{
			files_paths.push_back(fp);
		}
	}

	return files_paths;
}


////////////////////////////

int main(int argc, char *argv[]) 
{
    //setlocale(LC_ALL, "Russian");
	
	std::cout << "tgnews have started" << std::endl;  
	std::cout << std::endl;  
	
    // Create system default locale
    boost::locale::generator gen;
	#if defined(__linux__)
		std::locale ru_locale = gen("ru_RU.UTF-8");
		std::locale en_locale = gen("ru_RU.UTF-8");
	#endif
	
	#if defined(_WIN64)
		std::locale ru_boost_locale = gen("russian_russia.65001");
		std::locale en_boost_locale = gen("english_us.65001");
		std::locale ru_locale("russian_russia.65001");
		std::locale::global(ru_locale);	
	#endif
    //std::cout.imbue(ru_locale);

	
	/// Select working mode

	Mode mode = UNKNOWN_MODE;
	std::string data_path = "data";

	if (argc > 1)
	{
		if (argv[1] == LANGUAGES_MODE_COMMAND)
		{
			mode = LANGUAGES_MODE;
		}
		else if (argv[1] == NEWS_MODE_COMMAND)
		{
			mode = NEWS_MODE;
		}
		else if (argv[1] == CATEGORIES_MODE_COMMAND)
		{
			mode = CATEGORIES_MODE;
		}
		else if (argv[1] == THREAD_MODE_COMMAND)
		{
			mode = THREAD_MODE;
		}
		else if (argv[1] == TOP_MODE_COMMAND)
		{
			mode = TOP_MODE;
		}
		else
		{
			std::cout << "Unknown command: " << argv[1] << std::endl; 
			return EXIT_FAILURE; 
		}
	}
	else
	{
		std::cout << "Unspecified mode: you should specify working mode. Possible modes are: 'languages', 'news', 'categories', 'threads', 'top'." << std::endl;  
		return EXIT_FAILURE;
	}

	if (argc > 2)
	{
		data_path = argv[2];
		std::cout << "Using data path: " << data_path << std::endl;  
	}
	else
	{
		std::cout << "You haven't specified data path, default path will be used instead: " << data_path << std::endl;  
	}

	std::cout << std::endl;  

	/// variables
	
	auto english_language = news_clustering::Language(news_clustering::ENGLISH_LANGUAGE);
	auto russian_language = news_clustering::Language(news_clustering::RUSSIAN_LANGUAGE);

	std::vector<news_clustering::Language> languages = { 
		english_language, 
		russian_language
	};

	//
	
	std::unordered_map<news_clustering::Language, std::locale> language_boost_locales;
	language_boost_locales[english_language] = en_boost_locale;
	language_boost_locales[russian_language] = ru_boost_locale;

	//


	/// Load data

	auto file_names = selectHtmlFiles(data_path);
	std::cout << "Num files: " << file_names.size() << std::endl;  
	std::cout << std::endl;  

	
	auto content_parser = news_clustering::ContentParser();

	/// Language detection

	// load vocabularies  with top frequency words for each language
	
	std::vector<std::string> top_freq_vocab_paths;
	top_freq_vocab_paths.push_back("assets/vocabs/top_english_words.voc");
	top_freq_vocab_paths.push_back("assets/vocabs/top_russian_words.voc");
	
	auto language_detector = news_clustering::LanguageDetector(languages, top_freq_vocab_paths, language_boost_locales);

    std::unordered_map<std::string, news_clustering::Language> selected_language_articles; 
	
	auto t0 = std::chrono::steady_clock::now();
	auto t1 = std::chrono::steady_clock::now();
	auto t2 = std::chrono::steady_clock::now();	
	
	auto all_articles = language_detector.detect_language_by_file_names(file_names);

	for (auto i = all_articles.begin(); i != all_articles.end(); i++)
	{	
		std::cout << i->first.to_string() << " : " << std::endl;
		
		std::cout << "[ " << std::endl;
		for (auto k : i->second)
		{
			std::cout << "    " << k << std::endl;
		}
		std::cout << "]" << std::endl;

		// select only known languages
		if (i->first.id() != news_clustering::UNKNOWN_LANGUAGE)
		{
			for (auto k : i->second)
			{
				selected_language_articles[k] = i->first;
			}
		}

		//if (i % 1000 == 0)
		//{
		//	t2 = std::chrono::steady_clock::now();
		//	std::cout << i << " (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000 << " s)" << std::endl;
		//	std::cout << std::endl;  
		//	t1 = std::chrono::steady_clock::now();
		//}
	}
	t2 = std::chrono::steady_clock::now();
	std::cout << "Total (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cout << std::endl;  

	/// Name Entities recognition
	
	std::unordered_map<std::string, std::vector<std::string>> selected_language_content;	
	std::vector<std::string> content;

	for (auto i = selected_language_articles.begin(); i != selected_language_articles.end(); i++)
	{
		content = content_parser.parse(i->first, language_boost_locales[i->second]);
		selected_language_content[i->first] = content;
	}
	
	auto ner = news_clustering::NER(languages, language_boost_locales);
    auto ner_articles = ner.find_name_entities(selected_language_articles, selected_language_content); 
	
	for (auto i = ner_articles.begin(); i != ner_articles.end(); i++)
	{
		std::cout << i->first << " : " << std::endl;
		
		std::cout << "[ " << std::endl;
		for (auto k : i->second)
		{
			std::cout << "    " << k << std::endl;
		}
		std::cout << "]" << std::endl;
	}

	/// News detection
	
	std::unordered_map<news_clustering::Language, std::string> day_names_path;
	day_names_path[english_language] = "assets/vocabs/english_day_names.voc";
	day_names_path[russian_language] = "assets/vocabs/russian_day_names.voc";
	
	std::unordered_map<news_clustering::Language, std::string> month_names_path;
	month_names_path[english_language] = "assets/vocabs/english_month_names.voc";
	month_names_path[russian_language] = "assets/vocabs/russian_month_names.voc";
	
	auto news_detector = news_clustering::NewsDetector(languages, language_boost_locales, day_names_path, month_names_path);

	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();
	
    auto news_articles = news_detector.detect_news(selected_language_articles); 
    std::unordered_map<std::string, news_clustering::Language> selected_news_articles; 

	int index = 0;
	for (auto i = news_articles.begin(); i != news_articles.end(); i++) 
	{ 
		std::cout << i->first << " : " << std::endl;
		
		std::cout << "[ " << std::endl;
		for (auto k : i->second)
		{
			std::cout << "    " << k << std::endl;
		}
		std::cout << "]" << std::endl;

		// select only news
		if (i->first)
		{
			for (auto k : i->second)
			{
				selected_news_articles[k] = selected_language_articles[k];
			}
		}

		//index++;
		//if (index % 1000 == 0)
		//{
		//	t2 = std::chrono::steady_clock::now();
		//	std::cout << index << " (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000 << " s)" << std::endl;
		//	std::cout << std::endl;  
		//	t1 = std::chrono::steady_clock::now();
		//}
	}
	t2 = std::chrono::steady_clock::now();
	std::cout << "Total (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cout << std::endl;  


	/// Categorization
	
	// create lemmatizers here for reuse
	std::unordered_map<news_clustering::Language, news_clustering::Lemmatizer> lemmatizers;
	lemmatizers[english_language] = news_clustering::Lemmatizer();
	lemmatizers[russian_language] = news_clustering::Lemmatizer("../data/embedding/dict.opcorpora-upos-tags-100000-words.voc", russian_language);

	std::unordered_map<news_clustering::Language, std::string> word2vec_vocab_paths;
	word2vec_vocab_paths[english_language] = "../data/embedding/GoogleNews-vectors-10000-words.bin";
	word2vec_vocab_paths[russian_language] = "../data/embedding/RusVectoresNews-2019-vectores-10000-words.bin";
	
	std::unordered_map<news_clustering::Language, std::vector<std::vector<std::string>>> categories;
	categories[english_language] = {
		{"Society", "Politics", "Elections", "Legislation", "Incidents", "Crime"}, 
		{"Economy", "Markets", "Finance", "Business"}, 
		{"Technology", "Gadgets", "Auto", "Apps", "Internet"}, 
		{"Sports", "Cybersport"},
		{"Entertainment", "Movies", "Music", "Games", "Books", "Arts"}, 
		{"Science", "Health", "Biology", "Physics", "Genetics"} 
	};
	categories[russian_language] = { 
		{"Общество", "Политика", "Выборы", "Закон", "Инцидент", "Криминал"}, 
		{"Экономика", "Рынок", "Финансы", "Бизнес"}, 
		{"Технология", "Гаджет", "Авто", "Приложение", "Интернет"}, 
		{"Спорт", "Киберспорт"},
		{"Развлечение", "Фильм", "Музыка", "Игра", "Книга", "Искусство"}, 
		{"Наука", "Здоровье", "Биология", "Физика", "Генетика"}
	};
	   	 
	auto categories_detector = news_clustering::CategoriesDetector(languages, language_boost_locales, word2vec_vocab_paths, lemmatizers, categories);
	
	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();
	
    auto categories_articles = categories_detector.detect_categories(selected_language_articles); 
	index = 0;

	for (auto i = categories_articles.begin(); i != categories_articles.end(); i++) 
	{ 
		std::cout << i->first << " : " << std::endl;
		
		std::cout << "[ " << std::endl;
		for (auto k : i->second)
		{
			std::cout << "    " << k << std::endl;
		}
		std::cout << "]" << std::endl;

		//index++;
		//if (index % 1000 == 0)
		//{
		//	t2 = std::chrono::steady_clock::now();
		//	std::cout << index << " (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000 << " s)" << std::endl;
		//	std::cout << std::endl;  
		//	t1 = std::chrono::steady_clock::now();
		//}
	}
	t2 = std::chrono::steady_clock::now();
	std::cout << "Total (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cout << std::endl;  


	/// Threads (similar news) clustering
	
	std::unordered_map<news_clustering::Language, std::string> word2vec_clustered_vocab_paths;
	word2vec_clustered_vocab_paths[english_language] = "../data/embedding/GoogleNews-vectors-10000-words-30-clusters.bin";
	word2vec_clustered_vocab_paths[russian_language] = "../data/embedding/RusVectoresNews-2019-vectores-10000-words-30-clusters.bin";
	   	 
	auto news_clusterizer = news_clustering::NewsClusterizer(languages, language_boost_locales, word2vec_clustered_vocab_paths, lemmatizers);

	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();
	
    auto clustered_articles = news_clusterizer.clusterize(selected_language_articles); 
	
	index = 0;
	for (auto i = clustered_articles.begin(); i != clustered_articles.end(); i++) 
	{ 
		std::cout << i->first << " : " << std::endl;
		
		std::cout << "[ " << std::endl;
		for (auto k : i->second)
		{
			std::cout << "    " << k << std::endl;
		}
		std::cout << "]" << std::endl;

		//index++;
		//if (index % 1000 == 0)
		//{
		//	t2 = std::chrono::steady_clock::now();
		//	std::cout << index << " (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000 << " s)" << std::endl;
		//	std::cout << std::endl;  
		//	t1 = std::chrono::steady_clock::now();
		//}
	}
	t2 = std::chrono::steady_clock::now();
	std::cout << "Total (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cout << std::endl;  


	/// News arrange by relevance
	   	 
	auto news_ranger = news_clustering::NewsRanger(languages, language_boost_locales, word2vec_clustered_vocab_paths, lemmatizers);

	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();
	
    std::unordered_map<std::string, std::vector<int>> ranged_articles = news_ranger.arrange(selected_language_articles); 
	
	index = 0;
	for (auto i = ranged_articles.begin(); i != ranged_articles.end(); i++) { 
		std::cout << i->first << " : ";
		
		std::cout << "{ ";
		for (auto k : i->second)
		{
			std::cout << k << " ";
		}
		std::cout << "}" << std::endl;

		//index++;
		//if (index % 1000 == 0)
		//{
		//	t2 = std::chrono::steady_clock::now();
		//	std::cout << index << " (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000 << " s)" << std::endl;
		//	std::cout << std::endl;  
		//	t1 = std::chrono::steady_clock::now();
		//}
	}
	t2 = std::chrono::steady_clock::now();
	std::cout << "Total (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cout << std::endl;  

    return 0;
}
