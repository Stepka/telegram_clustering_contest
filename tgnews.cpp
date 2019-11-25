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

#include "metric/modules/mapping.hpp"
#include "modules/language_detector.hpp"
#include "modules/text_embedding.hpp"
#include "modules/content_parser.hpp"
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


//////////////////////////// Parsing


//////////////////////////// Language recognition


//////////////////////////// News recognition

int extractYear(const char *p) {
    int x = 0;
    if (*p < '0' || *p > '9') {
		return -1;
    }
    while (*p >= '0' && *p <= '9') {
        x = (x*10) + (*p - '0');
        ++p;
    }
    return x;
}



/**
return dd.mm.yyyy, f.e. 28.01.2019
*/
std::vector<int> checkIfDate(std::string part_1, std::string part_2, std::string part_3, std::unordered_map<std::string, int> day_names, std::unordered_map<std::string, int> month_names, 
	std::locale locale, news_clustering::Language language)
{
	int now_year = 2019;

	std::vector<std::vector<int>> valid_masks;
	
	switch (language)
	{
		case news_clustering::RUSSIAN_LANGUAGE:
			// D M Y   - 1 Янв 2000
			valid_masks.push_back({ 0, 1, 2 });
			// Y M D   - 2000 Jan 1
			valid_masks.push_back({ 2, 1, 0 });
			
			// Y D M   - 2000 1 Янв
			valid_masks.push_back({ 2, 0, 1 });

			// D M X   - 1 Янв X
			valid_masks.push_back({ 0, 1, 3 });
			// X D M   - X 1 Янв
			valid_masks.push_back({ 3, 0, 1 });
			break;
			
		case news_clustering::ENGLISH_LANGUAGE:
		default:
			// M D Y   - Jan 1 2000
			valid_masks.push_back({ 1, 0, 2 });
			// Y M D   - 2000 Jan 1
			valid_masks.push_back({ 2, 1, 0 });
			// D M Y   - 1 Jan 2000
			valid_masks.push_back({ 0, 1, 2 });
			// Y D M   - 2000 1 Jan
			valid_masks.push_back({ 2, 0, 1 });

			// M D X   - Jan 1 X
			valid_masks.push_back({ 1, 0, 3 });
			// X M D   - X Jan 1
			valid_masks.push_back({ 3, 1, 0 });
			// D M X   - 1 Jan X
			valid_masks.push_back({ 0, 1, 3 });
			// X D M   - X 1 Jan
			valid_masks.push_back({ 3, 0, 1 });
			break;
	}

	// Positions for the mask: is a day, is a month, is a year, doesn't matter
	std::vector<int> part_1_mask = { -1, -1, -1, 0 };
	std::vector<int> part_2_mask = { -1, -1, -1, 0 };
	std::vector<int> part_3_mask = { -1, -1, -1, 0 };
	
	std::unordered_map<std::string, int>::const_iterator what_found;

	what_found = day_names.find(boost::locale::to_lower(part_1, locale));
	if (what_found != day_names.end())
	{
		part_1_mask[0] = what_found->second;
	}
	what_found = day_names.find(boost::locale::to_lower(part_2, locale));
	if (what_found != day_names.end())
	{
		part_2_mask[0] = what_found->second;
	}
	what_found = day_names.find(boost::locale::to_lower(part_3, locale));
	if (what_found != day_names.end())
	{
		part_3_mask[0] = what_found->second;
	}

	//

	what_found = month_names.find(boost::locale::to_lower(part_1, locale));
	if (what_found != month_names.end())
	{
		part_1_mask[1] = what_found->second;
	}
	what_found = month_names.find(boost::locale::to_lower(part_2, locale));
	if (what_found != month_names.end())
	{
		part_2_mask[1] = what_found->second;
	}
	what_found = month_names.find(boost::locale::to_lower(part_3, locale));
	if (what_found != month_names.end())
	{
		part_3_mask[1] = what_found->second;
	}

	//

	auto part_1_i = extractYear(part_1.c_str());
	if (part_1_i >= 0 && part_1_i < 100)
	{
		part_1_i += part_1_i + ((int)now_year / 100) * 100;
	}		
	if(part_1_i >= now_year - 1 && part_1_i <= now_year + 1)
	{
		part_1_mask[2] = part_1_i;
	}

	auto part_2_i = extractYear(part_2.c_str());
	if (part_2_i >= 0 && part_2_i < 100)
	{
		part_2_i += part_2_i + ((int)now_year / 100) * 100;
	}		
	if(part_2_i >= now_year - 1 && part_2_i <= now_year + 1)
	{
		part_2_mask[2] = part_2_i;
	}

	auto part_3_i = extractYear(part_3.c_str());
	if (part_3_i >= 0 && part_3_i < 100)
	{
		part_3_i += part_3_i + ((int)now_year / 100) * 100;
	}		
	if(part_3_i >= now_year - 1 && part_3_i <= now_year + 1)
	{
		part_3_mask[2] = part_3_i;
	}

	//
	
	for (auto i = 0; i < valid_masks.size(); i++)
	{
		//std::cout << "valid_mask " << i << ":    " << (part_1_mask[valid_masks[i][0]] && part_2_mask[valid_masks[i][1]] && part_3_mask[valid_masks[i][2]]) << std::endl;
		if (part_1_mask[valid_masks[i][0]] >= 0 && part_2_mask[valid_masks[i][1]] >= 0 && part_3_mask[valid_masks[i][2]] >= 0)
		{
			int day = 0;
			int month = 0;
			int year = 0;

			switch (valid_masks[i][0])
			{
				case 0:
					// we expext day at first gram
					day = part_1_mask[valid_masks[i][0]];
					break;
				case 1:
					// we expext month at first gram
					month = part_1_mask[valid_masks[i][0]];
					break;
				case 2:
					// we expext year at first gram
					year = part_1_mask[valid_masks[i][0]];
					break;
			}
			switch (valid_masks[i][1])
			{
				case 0:
					// we expext day at second gram
					day = part_2_mask[valid_masks[i][1]];
					break;
				case 1:
					// we expext month at second gram
					month = part_2_mask[valid_masks[i][1]];
					break;
				case 2:
					// we expext year at second gram
					year = part_2_mask[valid_masks[i][1]];
					break;
			}
			switch (valid_masks[i][2])
			{
				case 0:
					// we expext day at third gram
					day = part_3_mask[valid_masks[i][2]];
					break;
				case 1:
					// we expext month at third gram
					month = part_3_mask[valid_masks[i][2]];
					break;
				case 2:
					// we expext year at third gram
					year = part_3_mask[valid_masks[i][2]];
					break;
			}
			//std::cout << "valid date " << i << ":    " << day << "." << month << "." << year << std::endl;
			return { day, month, year };
		}
	}

	return std::vector<int>();
}

std::vector<std::vector<int>> findDates(std::vector<std::string> content, std::unordered_map<std::string, int> day_names, std::unordered_map<std::string, int> month_names, 
	std::locale locale, news_clustering::Language language)
{
	std::vector<std::vector<int>> dates;

	for (auto i = 0; i < content.size(); i++)
	{ 
		if (month_names.find(boost::locale::to_lower(content[i], locale)) != month_names.end())
		{
			std::vector<int> date;
			if (i > 0 && i < content.size() - 1)
			{
				date = checkIfDate(content[i - 1], content[i], content[i + 1], day_names, month_names, locale, language );
			}
			else if(i == 0)
			{
				date = checkIfDate("", content[i], content[i + 1], day_names, month_names, locale, language );
			}
			else if(i == content.size() - 1)
			{
				date = checkIfDate(content[i - 1], content[i], "", day_names, month_names, locale, language );
			}

			if (date.size() == 3)
			{
			    //std::cout << "valid date:    " << date[0] << "." << date[1] << "." << date[2] << std::endl;
				dates.push_back(date);
			}
		}
	}

	return dates;
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

	///
	
    std::unordered_map<std::string, news_clustering::Language> articles_with_all_languages; 
	std::vector<news_clustering::Language> languages = { news_clustering::ENGLISH_LANGUAGE, news_clustering::RUSSIAN_LANGUAGE };
	
	std::vector<std::locale> language_boost_locales;
	language_boost_locales.push_back(en_boost_locale);
	language_boost_locales.push_back(ru_boost_locale);

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
	
	auto t0 = std::chrono::steady_clock::now();
	auto t1 = std::chrono::steady_clock::now();
	auto t2 = std::chrono::steady_clock::now();
	
    std::unordered_map<std::string, news_clustering::Language> articles_with_known_languages; 
	
	articles_with_all_languages = language_detector.detect_language_by_file_names(file_names);
	for (auto i = articles_with_all_languages.begin(); i != articles_with_all_languages.end(); i++)
	{	
		switch (i->second)
		{
			case news_clustering::ENGLISH_LANGUAGE:
				std::cout << i->first << " is in english " << std::endl;  
				articles_with_known_languages[i->first] = i->second;
				break;

			case news_clustering::RUSSIAN_LANGUAGE:
				std::cout << i->first << " is in russian " << std::endl;  
				articles_with_known_languages[i->first] = i->second;
				break;

			default:
				std::cout << i->first << " is in unknown language " << std::endl;  
				break;
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

	

	/// News detection
	
    std::unordered_map<std::string, int> russian_month_names = content_parser.read_vocabulary_and_tag("assets/vocabs/russian_month_names.voc", ru_boost_locale, 1, 12); 
    std::unordered_map<std::string, int> english_month_names = content_parser.read_vocabulary_and_tag("assets/vocabs/english_month_names.voc", en_boost_locale, 1, 12); 
	
    std::unordered_map<std::string, int> russian_day_names = content_parser.read_vocabulary_and_tag("assets/vocabs/russian_day_names.voc", ru_boost_locale, 1, 31); 
    std::unordered_map<std::string, int> english_day_names = content_parser.read_vocabulary_and_tag("assets/vocabs/english_day_names.voc", en_boost_locale, 1, 31); 

	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();
	int index = 0;
	for (auto i = articles_with_known_languages.begin(); i != articles_with_known_languages.end(); i++) { 
		std::cout << i->first << std::endl;  
		
		std::vector<std::string> content;
		std::vector<std::vector<int>> dates;
		switch (i->second)
		{
			case news_clustering::ENGLISH_LANGUAGE:
				content = content_parser.parse(i->first, en_boost_locale);   
				dates = findDates(content, english_day_names, english_month_names, en_boost_locale, i->second); 
				break;

			case news_clustering::RUSSIAN_LANGUAGE:
				content = content_parser.parse(i->first, ru_boost_locale); 
				dates = findDates(content, russian_day_names, russian_month_names, ru_boost_locale, i->second);
				break;

			default:
				break;
		}
		//for (auto date : dates)
		//{
		//	std::cout << date[0] << "." << date[1] << "." << date[2] << std::endl;
		//}
		//std::cout << std::endl;
		if (dates.size() > 0)
		{
			std::cout << " it is news" << std::endl;
		}
		else
		{
			std::cout << " it is not news" << std::endl;
		}

		index++;
		if (index % 1000 == 0)
		{
			t2 = std::chrono::steady_clock::now();
			std::cout << index << " (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000 << " s)" << std::endl;
			std::cout << std::endl;  
			t1 = std::chrono::steady_clock::now();
		}
	}
	t2 = std::chrono::steady_clock::now();
	std::cout << "Total (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cout << std::endl;  

	/// Categorization
	
	
	std::vector<std::string> word2vec_vocab_paths;
	word2vec_vocab_paths.push_back("../data/embedding/GoogleNews-vectors-10000-words.bin");
	auto word2vec = news_clustering::Word2Vec(word2vec_vocab_paths);
	
	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();
	index = 0;
	
	std::cout << "Society | Economy | Technology | Sports | Entertainment | Science" << std::endl;

	for (auto i = articles_with_known_languages.begin(); i != articles_with_known_languages.end(); i++) { 
		std::cout << i->first << std::endl;  
		
		std::vector<std::string> content;
		std::vector<float> text_distances;
		switch (i->second)
		{
			case news_clustering::ENGLISH_LANGUAGE:
				content = content_parser.parse(i->first, en_boost_locale);   
				text_distances = word2vec.texts_distance(content, { 
					{"Society", "Politics", "Elections", "Legislation", "Incidents", "Crime"}, 
					{"Economy", "Markets", "Finance", "Business"}, 
					{"Technology", "Gadgets", "Auto", "Apps", "Internet"}, 
					{"Sports", "Cybersport"},
					{"Entertainment", "Movies", "Music", "Games", "Books", "Arts"}, 
					{"Science", "Health", "Biology", "Physics", "Genetics"} 
					}, en_boost_locale);
				break;

			case news_clustering::RUSSIAN_LANGUAGE:
				content = content_parser.parse(i->first, ru_boost_locale); 
				text_distances = word2vec.texts_distance(content, { 
					{"Society", "Politics", "Elections", "Legislation", "Incidents", "Crime"}, 
					{"Economy", "Markets", "Finance", "Business"}, 
					{"Technology", "Gadgets", "Auto", "Apps", "Internet"}, 
					{"Sports", "Cybersport"},
					{"Entertainment", "Movies", "Music", "Games", "Books", "Arts"}, 
					{"Science", "Health", "Biology", "Physics", "Genetics"} 
					}, ru_boost_locale);
				break;

			default:
				break;
		}
		std::cout << "{ ";
		for (auto i : text_distances)
		{
			std::cout << i << " ";
		}
		std::cout << "}" << std::endl;

		index++;
		if (index % 1000 == 0)
		{
			t2 = std::chrono::steady_clock::now();
			std::cout << index << " (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000 << " s)" << std::endl;
			std::cout << std::endl;  
			t1 = std::chrono::steady_clock::now();
		}
	}
	t2 = std::chrono::steady_clock::now();
	std::cout << "Total (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cout << std::endl;  


	/// Threads (similar news) clustering
	
	
	std::vector<std::string> vocab_paths;
	vocab_paths.push_back("../data/embedding/GoogleNews-30-clusters-10000-words.bin");
	auto text_embedder = news_clustering::TextEmbedder(vocab_paths);
	
	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();
	index = 0;
	for (auto i = articles_with_known_languages.begin(); i != articles_with_known_languages.end(); i++) { 
		std::cout << i->first << std::endl;  
		
		std::vector<std::string> content;
		std::vector<int> text_embedding;
		switch (i->second)
		{
			case news_clustering::ENGLISH_LANGUAGE:
				content = content_parser.parse(i->first, en_boost_locale);   
				text_embedding = text_embedder(content, en_boost_locale);
				break;

			case news_clustering::RUSSIAN_LANGUAGE:
				content = content_parser.parse(i->first, ru_boost_locale); 
				text_embedding = text_embedder(content, ru_boost_locale);
				break;

			default:
				break;
		}
		std::cout << "{ ";
		for (auto i : text_embedding)
		{
			std::cout << i << " ";
		}
		std::cout << "}" << std::endl;

		index++;
		if (index % 1000 == 0)
		{
			t2 = std::chrono::steady_clock::now();
			std::cout << index << " (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000 << " s)" << std::endl;
			std::cout << std::endl;  
			t1 = std::chrono::steady_clock::now();
		}
	}
	t2 = std::chrono::steady_clock::now();
	std::cout << "Total (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cout << std::endl;  

    return 0;
}
