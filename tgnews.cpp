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
#include <ctime>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>

#include "modules/language_detector.hpp"
#include "modules/name_entities_recognizer.hpp"
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
	std::cerr << "tgnews have started" << std::endl;  
	std::cerr << std::endl;  
	
	auto total_start_time = std::chrono::steady_clock::now();
	auto t0 = std::chrono::steady_clock::now();
	auto t1 = std::chrono::steady_clock::now();
	auto t2 = std::chrono::steady_clock::now();	

	
    // Create system default locale
    boost::locale::generator gen;
	#if defined(__linux__)
		std::locale ru_boost_locale = gen("ru_RU.UTF-8");
		std::locale en_boost_locale = gen("en_US.UTF-8");
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
			std::cerr << "Unknown command: " << argv[1] << std::endl; 
			return EXIT_FAILURE; 
		}
	}
	else
	{
		std::cerr << "Unspecified mode: you should specify working mode. Possible modes are: 'languages', 'news', 'categories', 'threads', 'top'." << std::endl;  
		return EXIT_FAILURE;
	}

	if (argc > 2)
	{
		data_path = argv[2];
		std::cerr << "Using data path: " << data_path << std::endl;  
	}
	else
	{
		std::cerr << "You haven't specified data path, default path will be used instead: " << data_path << std::endl;  
	}

	std::cerr << std::endl;  


	/// Load data

	std::cerr << "Data loading..." << std::endl;  

	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();


	auto file_names = selectHtmlFiles(data_path);
	std::cerr << "Num files: " << file_names.size() << std::endl;  

	
	auto content_parser = news_clustering::ContentParser();
	
	std::unordered_map<std::string, std::vector<std::string>> all_content;	
	std::vector<std::string> content;

	for (auto i = 0; i < file_names.size(); i++)
	{
		content = content_parser.parse(file_names[i], std::locale(), ' ', 1);
		all_content[file_names[i]] = content;	

		if ((i + 1) % 1000 == 0)
		{
			t2 = std::chrono::steady_clock::now();
			std::cerr << "progress: " << (i + 1) << " from " << file_names.size() << " (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000 << " s)" << std::endl;
			t1 = std::chrono::steady_clock::now();
		}
	}
	

    std::unordered_map<std::string, news_clustering::Language> selected_language_articles; 
    std::unordered_map<std::string, std::vector<std::string>> selected_language_content; 
	
    std::unordered_map<std::string, news_clustering::Language> selected_news_articles; 
    std::unordered_map<std::string, std::vector<std::string>> selected_news_content; 
	
	std::unordered_map<std::string, std::string> articles_by_category;

	std::unordered_map<std::string, std::vector<std::string>> clustered_articles;

	t2 = std::chrono::steady_clock::now();
	std::cerr << "Data have loaded (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cerr << std::endl;  

	//
	
	json result;


	/// Data and vocabs prepare

	std::cerr << "Vocabs parsing..." << std::endl;  

	t0 = std::chrono::steady_clock::now();
	
	//
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
	
	// create lemmatizers here for reuse
	std::unordered_map<news_clustering::Language, news_clustering::Lemmatizer> lemmatizers;
	lemmatizers[english_language] = news_clustering::Lemmatizer();
	lemmatizers[russian_language] = news_clustering::Lemmatizer("../data/embedding/dict.opcorpora-upos-tags-100000-words.voc", russian_language, "_PROPN");
			
	//
	std::unordered_map<news_clustering::Language, news_clustering::TextEmbedder> text_embedders;
	text_embedders[english_language] = news_clustering::TextEmbedder("../data/embedding/GoogleNews-vectors-10000-words-30-clusters.bin", lemmatizers[english_language], english_language);
	text_embedders[russian_language] = news_clustering::TextEmbedder("../data/embedding/RusVectoresNews-2019-vectores-10000-words-30-clusters.bin", lemmatizers[russian_language], russian_language);
			
	//
	//std::unordered_map<news_clustering::Language, news_clustering::Word2Vec> word2vec_embedders;
	//word2vec_embedders[english_language] = news_clustering::Word2Vec("../data/embedding/GoogleNews-vectors-10000-words.bin", lemmatizers[english_language], english_language);
	//word2vec_embedders[russian_language] = news_clustering::Word2Vec("../data/embedding/RusVectoresNews-2019-vectores-10000-words.bin", lemmatizers[russian_language], russian_language);
	
	//
	std::vector<std::string> top_freq_vocab_paths;
	top_freq_vocab_paths.push_back("assets/vocabs/top_english_words.voc");
	top_freq_vocab_paths.push_back("assets/vocabs/top_russian_words.voc");
	
	//
	std::unordered_map<news_clustering::Language, std::string> day_names_path;
	day_names_path[english_language] = "assets/vocabs/english_day_names.voc";
	day_names_path[russian_language] = "assets/vocabs/russian_day_names.voc";
	
	std::unordered_map<news_clustering::Language, std::string> month_names_path;
	month_names_path[english_language] = "assets/vocabs/english_month_names.voc";
	month_names_path[russian_language] = "assets/vocabs/russian_month_names.voc";

	//
	std::unordered_map<news_clustering::Language, std::vector<std::vector<std::string>>> categories;
	categories[english_language] = {
		{"society", "politics", "elections", "legislation", "incidents", "crime"}, 
		{"economy", "markets", "finance", "business"}, 
		{"technology", "gadgets", "auto", "apps", "internet"}, 
		{"sports", "cybersport"},
		{"entertainment", "movies", "music", "games", "books", "arts"}, 
		{"science", "health", "biology", "physics", "genetics"} 
	};
	categories[russian_language] = { 
		{"общество", "политика", "выборы", "закон", "инцидент", "криминал"}, 
		{"экономика", "рынок", "финансы", "бизнес"}, 
		{"технология", "гаджет", "авто", "приложение", "интернет"}, 
		{"спорт", "киберспорт"},
		{"развлечение", "фильм", "музыка", "игра", "книга", "искусство"}, 
		{"наука", "здоровье", "биология", "физика", "генетика"}
	};

	t2 = std::chrono::steady_clock::now();
	std::cerr << "Vocab have parsed (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cerr << std::endl;  


	/// Language detection

	if (mode == LANGUAGES_MODE || mode == NEWS_MODE || mode == CATEGORIES_MODE || mode == THREAD_MODE || mode == TOP_MODE)
	{
		std::cerr << "Language detection..." << std::endl;

		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();

		auto language_detector = news_clustering::LanguageDetector(languages, top_freq_vocab_paths, language_boost_locales);
		
		/// Language consts
		size_t num_language_samples = 300;
		double language_score_min_level = 0.1;
		auto all_articles = language_detector.detect_language(all_content, num_language_samples, language_score_min_level);
		
		result = json();
		for (auto i = all_articles.begin(); i != all_articles.end(); i++)
		{		
			// select only known languages
			if (i->first.id() != news_clustering::UNKNOWN_LANGUAGE)
			{	
				json lang_item = {
					{"lang_code", i->first.to_string()}, 		
					{"articles", std::vector<std::string>()}
				};
				for (auto k : i->second)
				{
					selected_language_articles[k] = i->first;
					selected_language_content[k] = all_content[k];
					lang_item["articles"].push_back(k);
				}
				result.push_back(lang_item);
			}			
		}

		t2 = std::chrono::steady_clock::now();
		std::cerr << "Language detection have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		std::cerr << std::endl;
		
		if (mode == LANGUAGES_MODE)
		{
			std::cout << result.dump(4) << std::endl;
		}
	}
	

	/// Name Entities recognition

	std::cerr << "Name Entities recognition..." << std::endl;

	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();

	//auto ner = news_clustering::NER(languages, text_embedders, language_boost_locales);
	//auto ner_articles = ner.find_name_entities(selected_language_articles, selected_language_content);
	std::unordered_map<std::string, std::vector<std::string>> ner_articles;

	//for (auto i = ner_articles.begin(); i != ner_articles.end(); i++)
	//{
	//	std::cout << i->first << " : " << std::endl;

	//	std::cout << "[ " << std::endl;
	//	for (auto k : i->second)
	//	{
	//		std::cout << "    " << k << std::endl;
	//	}
	//	std::cout << "]" << std::endl;
	//}

	t2 = std::chrono::steady_clock::now();
	std::cerr << "Name Entities recognition have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cerr << std::endl;


	/// Titles extracting

	std::cerr << "Titles extracting..." << std::endl;

	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();

	auto title_extractor = news_clustering::TitleExtractor(language_boost_locales);
	auto title_articles = title_extractor.find_titles(selected_language_articles);

	//for (auto i = title_articles.begin(); i != title_articles.end(); i++)
	//{
	//	std::cout << i->first << " : " << i->second << std::endl;
	//}

	t2 = std::chrono::steady_clock::now();
	std::cerr << "Titles extracting have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cerr << std::endl;


	/// Dates extracting

	std::cerr << "Dates extracting..." << std::endl;

	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();

	auto dates_extractor = news_clustering::DatesExtractor(languages, language_boost_locales, day_names_path, month_names_path, 2019);
	auto found_dates = dates_extractor.find_dates(selected_language_articles, selected_language_content);

	//for (auto i = found_dates.begin(); i != found_dates.end(); i++)
	//{
	//	std::cout << i->first << " : " << std::endl;

	//	std::cout << "[ ";
	//	for (auto k = 0; k < i->second.size(); k++)
	//	{
	//		for (auto d = 0; d < i->second[k].size() - 1; d++)
	//		{
	//			std::cout << i->second[k][d] << ".";
	//		}
	//		std::cout << i->second[k][i->second[k].size() - 1] << ", ";
	//	}
	//	std::cout << "]" << std::endl;
	//}

	t2 = std::chrono::steady_clock::now();
	std::cerr << "Dates extracting have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cerr << std::endl;


	/// News detection
	
	if (mode == NEWS_MODE || mode == CATEGORIES_MODE || mode == THREAD_MODE || mode == TOP_MODE)
	{	
		std::cerr << "News detection..." << std::endl;  

		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();
	
		auto news_detector = news_clustering::NewsDetector(languages, language_boost_locales);
	
		auto news_articles = news_detector.detect_news(selected_language_articles, selected_language_content, found_dates, ner_articles); 
	
		result = {		
			{"articles", std::vector<std::string>()}
		};
		for (auto i = news_articles.begin(); i != news_articles.end(); i++) 
		{ 
			// select only news
			if (i->first)
			{
				for (auto k : i->second)
				{
					selected_news_articles[k] = selected_language_articles[k];
					selected_news_content[k] = all_content[k];
					result["articles"].push_back(k);
				}
			}		
		}

		t2 = std::chrono::steady_clock::now();
		std::cerr << "News detection have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		std::cerr << std::endl;  

		if (mode == NEWS_MODE)
		{
			std::cout << result.dump(4) << std::endl;
		}
	}


	/// Categorization
	
	if (mode == CATEGORIES_MODE || mode == TOP_MODE)
	{		
		std::cerr << "News categorization..." << std::endl;  
	
		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();
	   	 
		//auto categories_detector = news_clustering::CategoriesDetector(languages, text_embedders, word2vec_embedders, language_boost_locales, categories);
		auto categories_detector = news_clustering::CategoriesDetector(languages, text_embedders, language_boost_locales, categories);
	
		float category_detect_level = 0.9;
		auto categories_articles = categories_detector.detect_categories(selected_language_articles, selected_news_content, category_detect_level); 
	
		result = json();
		for (auto i = categories_articles.begin(); i != categories_articles.end(); i++) 
		{ 
			json category_item;			
			if (i->first == -1)
			{
				category_item = {
					{"category", "other"},
					{"articles", std::vector<std::string>()}
				};
			}
			else
			{
				category_item = {
					{"category", categories[english_language][i->first][0]},
					{"articles", std::vector<std::string>()}
				};
			}
			for (auto k : i->second)
			{	
				if (i->first == -1)
				{
					articles_by_category[k] = "other";
				}
				else
				{
					articles_by_category[k] = categories[english_language][i->first][0];
				}
				category_item["articles"].push_back(k);
			}
			result.push_back(category_item);
		}

		t2 = std::chrono::steady_clock::now();
		std::cerr << "News categorization have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		std::cerr << std::endl;  

		if (mode == CATEGORIES_MODE)
		{
			std::cout << result.dump(4) << std::endl;
		}
	}


	/// Threads (similar news) clustering
	
	if (mode == THREAD_MODE || mode == TOP_MODE)
	{	
		std::cerr << "Threads clustering..." << std::endl;  

		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();
	   	 
		//auto news_clusterizer = news_clustering::NewsClusterizer(languages, text_embedders, word2vec_embedders, language_boost_locales);
		auto news_clusterizer = news_clustering::NewsClusterizer(languages, text_embedders, language_boost_locales);
	
		float eps = 4;
		std::size_t minpts = 2;
		clustered_articles = news_clusterizer.clusterize(selected_news_articles, selected_news_content, title_articles, eps, minpts); 
	
		result = json();
		for (auto i = clustered_articles.begin(); i != clustered_articles.end(); i++) 
		{ 
			json thread_item = {
				{"title", title_articles[i->first]}, 		
				{"articles", std::vector<std::string>()}
			};
			for (auto k : i->second)
			{
				thread_item["articles"].push_back(k);
			}
			result.push_back(thread_item);
		}

		t2 = std::chrono::steady_clock::now();
		std::cerr << "Threads clustering have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		std::cerr << std::endl;  

		if (mode == THREAD_MODE)
		{
			std::cout << result.dump(4) << std::endl;
		}
	}


	/// News arrange by relevance
	
	if (mode == TOP_MODE)
	{		
		std::cerr << "Threads arranging..." << std::endl;  

		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();
	   	 
		std::time_t t = std::time(0);   // get time now
		std::tm* now = std::localtime(&t);
		std::vector<int> today = {now->tm_mday, now->tm_mon + 1, now->tm_year + 1900};
		auto news_ranger = news_clustering::NewsRanger(languages, text_embedders, language_boost_locales, today);
	
		auto ranged_articles = news_ranger.arrange(clustered_articles, found_dates, ner_articles); 
		std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::vector<std::string>>>> ranged_articles_by_categories;
	
		result = json();
		for (auto thread : ranged_articles)
		{
			for (auto i = thread.begin(); i != thread.end(); i++)
			{
				ranged_articles_by_categories["any"].push_back(thread);
				ranged_articles_by_categories[articles_by_category[i->first]].push_back(thread);
			}
		}
		for (auto i = ranged_articles_by_categories.begin(); i != ranged_articles_by_categories.end(); i++) 
		{ 
			json top_item = {
				{"category", i->first}, 		
				{"threads", std::vector<json>()}
			};
			for (auto k : i->second)
			{
				for (auto p = k.begin(); p != k.end(); p++)
				{
					json thread_item;
					if (i->first == "any")
					{
						thread_item = {
							{"title", title_articles[p->first]}, 
							{"category", ""}, 		
							{"articles", std::vector<std::string>()}
						};
					}
					else
					{
						thread_item = {
							{"title", title_articles[p->first]}, 		
							{"articles", std::vector<std::string>()}
						};
					}
					for (auto h : p->second)
					{
						if (i->first == "any")
						{
							thread_item["category"] = articles_by_category[h];
						}
						thread_item["articles"].push_back(h);
					}
					top_item["threads"].push_back(thread_item);
				}
			}
			result.push_back(top_item);
		}

		t2 = std::chrono::steady_clock::now();
		std::cerr << "Threads arranging have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		std::cerr << std::endl;  

		if (mode == TOP_MODE)
		{
			std::cout << result.dump(4) << std::endl;
		}
	}
	
	t2 = std::chrono::steady_clock::now();
	std::cerr << "Total time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - total_start_time).count()) / 1000000 << " s" << std::endl;

    return 0;
}
