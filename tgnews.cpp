﻿/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/

#include <vector>
#include <iostream>
#include <ctime>

#include "modules/language_detector.hpp"
#include "modules/name_entities_recognizer.hpp"
#include "modules/news_detector.hpp"
#include "modules/categories_detector.hpp"
#include "modules/news_clusterizer.hpp"
#include "modules/news_ranger.hpp"
#include "metric/modules/utils/ThreadPool.cpp"
#include "metric/modules/utils/Semaphore.h"

#include "3rdparty/json.hpp"


using json = nlohmann::json;


const std::string LANGUAGES_MODE_COMMAND = "languages";
const std::string NEWS_MODE_COMMAND = "news";
const std::string CATEGORIES_MODE_COMMAND = "categories";
const std::string THREAD_MODE_COMMAND = "threads";
const std::string TOP_MODE_COMMAND = "top";

enum Mode { UNKNOWN_MODE, LANGUAGES_MODE, NEWS_MODE, CATEGORIES_MODE, THREAD_MODE, TOP_MODE };

////////////////////////////

int main(int argc, char *argv[]) 
{	
	//std::cerr << "tgnews have started" << std::endl;  
	//std::cerr << std::endl;  
	
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
		//std::cerr << "Using data path: " << data_path << std::endl;  
	}
	else
	{
		std::cerr << "You haven't specified data path, default path will be used instead: " << data_path << std::endl;  
	}

	//std::cerr << std::endl;  

	/// config
	
	std::string config_filename = "assets/default.cfg";
	if (argc > 3)
	{
		config_filename = argv[3];
		std::cerr << "Using config: " << config_filename << std::endl;  
		std::cerr << std::endl;
	}
	std::ifstream config_fin(config_filename, std::ifstream::in);
	json config;

	if (config_fin.is_open()) 
	{
		config_fin >> config;
	}
	else
	{
		std::cerr << "Cannot open config file: " << config_filename << ", use default values instead" << std::endl;
		std::cerr << std::endl;

		config =
		{
			{"ru",
				{
					{"lemmatizer", "assets/vocabs/dict.opcorpora-upos-tags.voc"},
					{"clusterizer", "assets/vocabs/RusVectoresNews-2019-vectores-50000-words-1024-clusters.bin"},
					{"top_freq_words", "assets/vocabs/top_russian_words.voc"},
					{"day_names", "assets/vocabs/russian_day_names.voc"},
					{"month_names", "assets/vocabs/russian_month_names.voc"}
				}
			},

			{"en",
				{
					{"lemmatizer", ""},
					{"clusterizer", "assets/vocabs/GoogleNews-vectors-50000-words-1024-clusters.bin"},
					{"top_freq_words", "assets/vocabs/top_english_words.voc"},
					{"day_names", "assets/vocabs/english_day_names.voc"},
					{"month_names", "assets/vocabs/english_month_names.voc"}
				}
			}
		};
	}
	

	/// variables	
	
	json result;
	news_clustering::Language language;
	
	std::unordered_map<std::string, std::vector<std::string>> all_content;	

	std::unordered_map<std::string, std::vector<std::string>> ner_articles;
	std::unordered_map<std::string, std::string> title_articles;
	std::unordered_map<std::string, std::vector<std::vector<int>>> found_dates;
	
    std::unordered_map<std::string, news_clustering::Language> selected_language_articles; 
    std::unordered_map<std::string, std::vector<std::string>> selected_language_content; 
	
    std::unordered_map<std::string, news_clustering::Language> selected_news_articles; 
    std::unordered_map<std::string, std::vector<std::string>> selected_news_content; 
	
	std::unordered_map<std::string, std::string> articles_by_category;

	std::unordered_map<std::string, std::vector<std::string>> clustered_articles;


	/// Load data

	//std::cerr << "Data loading..." << std::endl;  

	t0 = std::chrono::steady_clock::now();
	t1 = std::chrono::steady_clock::now();
	
	std::time_t t = std::time(0);   // get time now
	std::tm* now = std::localtime(&t);
	std::vector<int> today = {now->tm_mday, now->tm_mon + 1, now->tm_year + 1900};

	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
	//std::cerr << "Num cores: " << concurentThreadsSupported << std::endl;
	std::mutex mutex_;
		
	auto content_parser = news_clustering::ContentParser();

	auto file_names = content_parser.selectHtmlFiles(data_path);
	//std::cerr << "Num files: " << file_names.size() << std::endl;  
	
	Semaphore sem;
	ThreadPool pool(concurentThreadsSupported);
	for (auto i = 0; i < file_names.size(); i++)
	{
		pool.execute(
			[i, &sem, &content_parser, &all_content, &file_names, &mutex_]()
			{			
				auto content = content_parser.parse(file_names[i], std::locale(), ' ', 1);

				mutex_.lock();
				all_content[file_names[i]] = content;
				mutex_.unlock();

				sem.notify();
			}
		);
	}	
	for (auto i = 0; i < file_names.size(); i++)
	{
		sem.wait();
	}
	pool.close();

	t2 = std::chrono::steady_clock::now();
	//std::cerr << "Data have loaded (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	//std::cerr << std::endl;  


	/// Data and vocabs prepare

	//std::cerr << "Vocabs parsing..." << std::endl;  

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
	lemmatizers[russian_language] = news_clustering::Lemmatizer(config["ru"]["lemmatizer"], russian_language, "_PROPN");
			
	//
	std::unordered_map<news_clustering::Language, news_clustering::TextEmbedder> text_embedders;
	text_embedders[english_language] = news_clustering::TextEmbedder(config["en"]["clusterizer"], lemmatizers[english_language], english_language);
	text_embedders[russian_language] = news_clustering::TextEmbedder(config["ru"]["clusterizer"], lemmatizers[russian_language], russian_language);
			
	//
	//std::unordered_map<news_clustering::Language, news_clustering::Word2Vec> word2vec_embedders;
	//word2vec_embedders[english_language] = news_clustering::Word2Vec("../data/embedding/GoogleNews-vectors-10000-words.bin", lemmatizers[english_language], english_language);
	//word2vec_embedders[russian_language] = news_clustering::Word2Vec("../data/embedding/RusVectoresNews-2019-vectores-10000-words.bin", lemmatizers[russian_language], russian_language);
	
	//
	std::vector<std::string> top_freq_vocab_paths;
	top_freq_vocab_paths.push_back(config["en"]["top_freq_words"]);
	top_freq_vocab_paths.push_back(config["ru"]["top_freq_words"]);
	
	//
	std::unordered_map<news_clustering::Language, std::string> day_names_path;
	day_names_path[english_language] = config["en"]["day_names"];
	day_names_path[russian_language] = config["ru"]["day_names"];
	
	std::unordered_map<news_clustering::Language, std::string> month_names_path;
	month_names_path[english_language] = config["en"]["month_names"];
	month_names_path[russian_language] = config["ru"]["month_names"];

	//
	std::unordered_map<news_clustering::Language, std::vector<std::vector<std::string>>> categories;
	categories[english_language] = content_parser.parse_categories(config["en"]["categories"], en_boost_locale);
	categories[russian_language] = content_parser.parse_categories(config["ru"]["categories"], en_boost_locale);

	t2 = std::chrono::steady_clock::now();
	//std::cerr << "Vocab have parsed (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
	//std::cerr << std::endl;  


	/// Language detection

	if (mode == LANGUAGES_MODE || mode == NEWS_MODE || mode == CATEGORIES_MODE || mode == THREAD_MODE || mode == TOP_MODE)
	{
		//std::cerr << "Language detection..." << std::endl;

		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();

		auto language_detector = news_clustering::LanguageDetector(languages, top_freq_vocab_paths, language_boost_locales);
		
		/// Hyperparams
		// Language consts
		size_t num_language_samples = 300;
		double language_score_min_level = 0.1;
		
		auto found_languages = language_detector.detect_language(all_content, num_language_samples, language_score_min_level);			
		
		std::size_t found_filename_start;
		std::string filename;

		// Prepare result
		result = json();
		for (auto i = found_languages.begin(); i != found_languages.end(); i++)
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
					
					found_filename_start = k.find_last_of("/\\");
					filename = k.substr(found_filename_start + 1);
					lang_item["articles"].push_back(filename);
				}
				result.push_back(lang_item);
			}			
		}

		t2 = std::chrono::steady_clock::now();
		//std::cerr << "Language detection have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		//std::cerr << std::endl;
		
		if (mode == LANGUAGES_MODE)
		{
			std::cout << result.dump(4, ' ', false, json::error_handler_t::replace) << std::endl;
		}
	}
	

	/// Name Entities recognition

	if (mode == NEWS_MODE || mode == CATEGORIES_MODE || mode == THREAD_MODE || mode == TOP_MODE)
	{

		//std::cerr << "Name Entities recognition..." << std::endl;

		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();

		//auto ner = news_clustering::NER(languages, text_embedders, language_boost_locales);
		//auto ner_articles = ner.find_name_entities(selected_language_articles, selected_language_content);

		t2 = std::chrono::steady_clock::now();
		//std::cerr << "Name Entities recognition have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		//std::cerr << std::endl;


		/// Titles extracting

		//std::cerr << "Titles extracting..." << std::endl;

		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();

		auto title_extractor = news_clustering::TitleExtractor(language_boost_locales);
		title_articles = title_extractor.find_titles(selected_language_articles);

		t2 = std::chrono::steady_clock::now();
		//std::cerr << "Titles extracting have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		//std::cerr << std::endl;


		/// Dates extracting

		//std::cerr << "Dates extracting..." << std::endl;

		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();

		auto dates_extractor = news_clustering::DatesExtractor(languages, language_boost_locales, day_names_path, month_names_path, today[2]);
		
		found_dates = dates_extractor.find_dates(selected_language_articles, selected_language_content);

		t2 = std::chrono::steady_clock::now();
		//std::cerr << "Dates extracting have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		//std::cerr << std::endl;
	}


	/// News detection
	
	if (mode == NEWS_MODE || mode == CATEGORIES_MODE || mode == THREAD_MODE || mode == TOP_MODE)
	{	
		//std::cerr << "News detection..." << std::endl;  

		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();
	
		auto news_detector = news_clustering::NewsDetector(languages, language_boost_locales, today);
		
		/// Hyperparams
		// News detection consts
		int freshness_days = 180;
	
		auto news_articles = news_detector.detect_news(selected_language_articles, selected_language_content, found_dates, ner_articles, freshness_days); 
		
		std::size_t found_filename_start;
		std::string filename;

		// Prepare result
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

					found_filename_start = k.find_last_of("/\\");
					filename = k.substr(found_filename_start + 1);
					result["articles"].push_back(filename);
				}
			}	
		}

		t2 = std::chrono::steady_clock::now();
		//std::cerr << "News detection have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		//std::cerr << std::endl;  

		if (mode == NEWS_MODE)
		{
			std::cout << result.dump(4, ' ', false, json::error_handler_t::replace) << std::endl;
		}
	}


	/// Categorization
	
	if (mode == CATEGORIES_MODE || mode == TOP_MODE)
	{		
		//std::cerr << "News categorization..." << std::endl;  
	
		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();
	   	 
		//auto categories_detector = news_clustering::CategoriesDetector(languages, text_embedders, word2vec_embedders, language_boost_locales, categories);
		auto categories_detector = news_clustering::CategoriesDetector(languages, text_embedders, language_boost_locales, categories);
	
		/// Hyperparams
		// Categories consts	
		std::unordered_map<news_clustering::Language, std::vector<float>> category_detect_levels;
		// society | economy | technology | sports | entertainment | science
		category_detect_levels[english_language] = {0.02, 0.02, 0.02, 0.02, 0.02, 0.04};
		category_detect_levels[russian_language] = {0.05, 0.02, 0.15, 0.02, 0.15, 0.15};

		auto categories_articles = categories_detector.detect_categories(selected_language_articles, selected_news_content, category_detect_levels); 
	
		std::size_t found_filename_start;
		std::string filename;

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
				found_filename_start = k.find_last_of("/\\");
				filename = k.substr(found_filename_start + 1);
				category_item["articles"].push_back(filename);
			}
			result.push_back(category_item);
		}

		t2 = std::chrono::steady_clock::now();
		//std::cerr << "News categorization have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		//std::cerr << std::endl;  

		if (mode == CATEGORIES_MODE)
		{
			std::cout << result.dump(4, ' ', false, json::error_handler_t::replace) << std::endl;
		}
	}


	/// Threads (similar news) clustering
	
	if (mode == THREAD_MODE || mode == TOP_MODE)
	{	
		//std::cerr << "Threads clustering..." << std::endl;  

		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();
	   	 
		//auto news_clusterizer = news_clustering::NewsClusterizer(languages, text_embedders, word2vec_embedders, language_boost_locales);
		auto news_clusterizer = news_clustering::NewsClusterizer(languages, text_embedders, language_boost_locales);
	
		float eps = 12;
		std::size_t minpts = 2;
		clustered_articles = news_clusterizer.clusterize(selected_news_articles, selected_news_content, title_articles, eps, minpts); 
	
		std::size_t found_filename_start;
		std::string filename;

		result = json();
		for (auto i = clustered_articles.begin(); i != clustered_articles.end(); i++) 
		{ 
			json thread_item = {
				{"title", title_articles[i->first]}, 		
				{"articles", std::vector<std::string>()}
			};
			for (auto k : i->second)
			{
				found_filename_start = k.find_last_of("/\\");
				filename = k.substr(found_filename_start + 1);
				thread_item["articles"].push_back(filename);
			}
			result.push_back(thread_item);
		}

		t2 = std::chrono::steady_clock::now();
		//std::cerr << "Threads clustering have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		//std::cerr << std::endl;  

		if (mode == THREAD_MODE)
		{
			std::cout << result.dump(4, ' ', false, json::error_handler_t::replace) << std::endl;
		}
	}


	/// News arrange by relevance
	
	if (mode == TOP_MODE)
	{		
		//std::cerr << "Threads arranging..." << std::endl;  

		t0 = std::chrono::steady_clock::now();
		t1 = std::chrono::steady_clock::now();
	   	 
		auto news_ranger = news_clustering::NewsRanger(languages, text_embedders, language_boost_locales, today);
	
		auto ranged_articles = news_ranger.arrange(clustered_articles, found_dates, ner_articles); 
		std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::vector<std::string>>>> ranged_articles_by_categories;
	
		std::size_t found_filename_start;
		std::string filename;

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
						found_filename_start = h.find_last_of("/\\");
						filename = h.substr(found_filename_start + 1);
						thread_item["articles"].push_back(filename);
					}
					top_item["threads"].push_back(thread_item);
				}
			}
			result.push_back(top_item);
		}

		t2 = std::chrono::steady_clock::now();
		//std::cerr << "Threads arranging have finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t0).count()) / 1000000 << " s)" << std::endl;
		//std::cerr << std::endl;  

		if (mode == TOP_MODE)
		{
			std::cout << result.dump(4, ' ', false, json::error_handler_t::replace) << std::endl;
		}
	}
	
	t2 = std::chrono::steady_clock::now();
	//std::cerr << "Total time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - total_start_time).count()) / 1000000 << " s" << std::endl;

    return 0;
}
