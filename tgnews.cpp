/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/


#if defined(__linux__)
	#define _GNU_SOURCE    // includes _BSD_SOURCE for DT_UNKNOWN etc.
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
#include "metric/modules/mapping.hpp"
#include "modules/json.hpp"


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

/// Language consts
enum Language { UNKNOWN_LANGUAGE, ENGLISH_LANGUAGE, RUSSIAN_LANGUAGE };
size_t num_language_samples = 300;
double language_score_min_level = 0.1;

////////////////////////////


std::vector<std::string> readAllPathsFromDir(std::string dirname)
{
	std::vector<std::string> path_names;

	#if defined(__linux__)
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dirname.c_str())) == NULL) {
			std::cout << "Error(" << errno << ") opening " << dirname << std::endl;
			return std::vector<std::vector<double>>();
		}

		while ((dirp = readdir(dp)) != NULL) {
			std::string fn = std::string(dirp->d_name);
			path_names.push_back(dirname + "/" + fn);
		}
		closedir(dp);
	#endif
	
	#if defined(_WIN64)
		for (const auto & entry : fs::directory_iterator(dirname))
		{
			std::string fn = entry.path().filename().string();
			
			path_names.push_back(entry.path().string());

			//std::fstream fin;

			//fin.open(filename, std::ios::in);
		}
	#endif

	return path_names;
}


std::vector<std::string> readFilePaths(std::string dirname, bool recursively = true)
{
	std::vector<std::string> files_paths;

	std::vector<std::string> all_paths = readAllPathsFromDir(dirname);

	for (auto fp : all_paths)
	{
		#if defined(_WIN64)
			const fs::path path(fp);
			std::error_code ec; // For using the non-throwing overloads of functions below.
			if (fs::is_directory(path, ec))
			{ 
				if (recursively)
				{
					std::vector<std::string> dir_files = readFilePaths(fp, recursively);
					files_paths.insert(files_paths.end(), dir_files.begin(), dir_files.end());
				}
			}
			if (ec)
			{
				std::cerr << "Error in is_directory: " << ec.message();
			}
			if (fs::is_regular_file(path, ec))
			{
				if (fp.size() > 5 && fp.substr(fp.size() - 5) == ".html")
				{
					files_paths.push_back(fp);
				}
			}
			if (ec) 
			{
				std::cerr << "Error in is_regular_file: " << ec.message();
			}
		#endif

		#if defined(__linux__)
			DIR *dp;
			struct dirent *dirp;
			if((dp  = opendir(dirname.c_str())) == NULL) {
				std::cout << "Error(" << errno << ") opening " << dirname << std::endl;
				return std::vector<std::string>();
			}

			while ((dirp = readdir(dp)) != NULL) 
			{
				bool is_dir;
				#ifdef _DIRENT_HAVE_D_TYPE
					if (dirp->d_type != DT_UNKNOWN && dirp->d_type != DT_LNK) {
					   // don't have to stat if we have d_type info, unless it's a symlink (since we stat, not lstat)
					   is_dir = (dirp->d_type == DT_DIR);
					} else
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
							std::vector<std::string> dir_files = readFilePaths(dirname + "/" + fn, recursively);
							files_paths.insert(files_paths.end(), dir_files.begin(), dir_files.end());
						}
					}
					else
					{						
						std::string fn = std::string(dirp->d_name);
						if (fn.size() > 5 && fn.substr(fn.size() - 5) == ".html")
						{
							files_paths.push_back(dirname + "/" + fn);
						}
					}
			}
			closedir(dp);
		#endif
	}

	return files_paths;
}



std::vector<std::string> readFileContent(std::string filename, char delimeter = ' ', int min_word_size = 2)
{
	std::vector<std::string> words;
	std::string word;

	std::fstream fin;

	fin.open(filename, std::ios::in);

	while (getline(fin, word, delimeter))
	{
		//std::cout << " -> " << word << std::endl;
		if (word.size() >= min_word_size)
		{
			words.push_back(word);
		}
	}

	return words;
}

std::vector<std::string> readVocabulary(std::string filename)
{
	std::vector<std::string> words;
	std::string word;

	std::fstream fin;

	fin.open(filename, std::ios::in);

	while (getline(fin, word))
	{
		words.push_back(word);
	}

	return words;
}

double countVocabFrequency(std::vector<std::string> content, std::vector<size_t> sampling_indexes, std::vector<std::string> vocab)
{
	int score = 0;

	for (auto i = 0; i < sampling_indexes.size(); i++)
	{
		std::string sample = content[sampling_indexes[i]];
		for (auto word : vocab)
		{
			if (sample == word)
			{
				score++;
			}
		}
	}

	return (double) score / sampling_indexes.size();
}

Language checkLanguage(std::vector<std::string> content, std::vector<std::vector<std::string>> vocabs)
{		
    // Random sampleing 
    std::vector<size_t> randomized_samples(content.size());
    std::iota(randomized_samples.begin(), randomized_samples.end(), 0);
	// shuffle samples after all was processed		
    std::shuffle(randomized_samples.begin(), randomized_samples.end(), std::mt19937 { std::random_device {}() });

	if (num_language_samples < randomized_samples.size())
	{
		randomized_samples.resize(num_language_samples);
	}	  
	
	std::vector<Language> languages = { ENGLISH_LANGUAGE, RUSSIAN_LANGUAGE };
	std::vector<double> scores(2);

	scores[0] = countVocabFrequency(content, randomized_samples, vocabs[0]);
	scores[1] = countVocabFrequency(content, randomized_samples, vocabs[1]);
	
	//for (auto word : content)
	//{
	//	std::cout << word << " ";  
	//}
	//std::cout << std::endl;   
	//std::cout << "Num words: " << content.size() << std::endl;  
	//std::cout << std::endl; 
	//std::cout << "Samples size: " << randomized_samples.size() << std::endl;  
	//std::cout << std::endl;
	//std::cout << "English score: " << scores[0] << std::endl;  
	//std::cout << std::endl;  
	//std::cout << "Russian score: " << scores[1] << std::endl;   
	//std::cout << std::endl;  

	auto max_score_iterator = std::max_element(scores.begin(), scores.end());
	auto max_score_index = std::distance(scores.begin(), max_score_iterator);

	if (scores[max_score_index] > language_score_min_level)
	{
		return languages[max_score_index];
	}

	return UNKNOWN_LANGUAGE;
}

////////////////////////////

int main(int argc, char *argv[]) 
{
	
	std::cout << "tgnews have started" << std::endl;  
	std::cout << std::endl;  
	
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

	/// Load data

	auto file_names = readFilePaths(data_path);
	std::cout << "Num files: " << file_names.size() << std::endl;  
	std::cout << std::endl;  

	/// Check language
	
	// load vocabularies  with top frequency words for each language
	std::vector<std::string> top_english_words = readVocabulary("assets/vocabs/top_english_words.voc");
	std::vector<std::string> top_russian_words = readVocabulary("assets/vocabs/top_russian_words.voc");
	
	std::vector<std::string> english_filenames;
	std::vector<std::string> russian_filenames;
	
	auto t1 = std::chrono::steady_clock::now();
	for (auto i = 0; i < file_names.size(); i++)
	{

		auto content = readFileContent(file_names[i]);

		auto language = checkLanguage(content, {top_english_words, top_russian_words});

		switch (language)
		{
			case ENGLISH_LANGUAGE:
				//std::cout << file_names[i] << " is in english " << std::endl;  
				english_filenames.push_back(file_names[i]);
				break;

			case RUSSIAN_LANGUAGE:
				//std::cout << file_names[i] << " is in russian " << std::endl;  
				russian_filenames.push_back(file_names[i]);
				break;

			default:
				//std::cout << file_names[i] << " is in unknown language " << std::endl;  
				break;
		}

		if (i % 1000 == 0)
		{
			auto t2 = std::chrono::steady_clock::now();
			std::cout << i << " (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000 << " s)" << std::endl;
			std::cout << std::endl;  
			t1 = std::chrono::steady_clock::now();
		}
	}

    return 0;
}
