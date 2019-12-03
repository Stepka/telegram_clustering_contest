/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_CONTENT_PARSER_CPP
#define _NEWS_CLUSTERING_CONTENT_PARSER_CPP

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

#include "content_parser.hpp"
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>


#if defined(_WIN64)
namespace  fs = std::filesystem;
#endif

namespace news_clustering {
	
	std::vector<std::string> ContentParser::parse(const std::string& filename, const std::locale& locale, char delimeter, int min_word_size)
	{
		std::vector<std::string> words, result;
	
		std::string line, word;

		std::fstream fin;
		fin.imbue(locale);

		fin.open(filename, std::ios::in);	
		
		if (!fin.is_open())
		{
			std::cerr << "Cannot open file: " << filename << std::endl;
		}
		else
		{
			while (getline(fin, line))
			{
				words = split_string(line, delimeter, min_word_size);
				result.insert(result.end(), words.begin(), words.end());
			}
		}

		return result;
	}
	
	std::vector<std::vector<std::string>> ContentParser::parse_categories(const std::string& filename, const std::locale& locale, char delimeter)
	{
		std::vector<std::string> words;
		std::vector<std::vector<std::string>> result;
	
		std::string line, word;

		std::fstream fin;
		fin.imbue(locale);

		fin.open(filename, std::ios::in);	
		
		if (!fin.is_open())
		{
			std::cerr << "Cannot open file: " << filename << std::endl;
		}
		else
		{
			while (getline(fin, line))
			{
				words = split_string(line, delimeter);
				result.push_back(words);
			}
		}

		return result;
	}
	
	std::vector<std::string> ContentParser::split_string(std::string& line, char delimeter, int min_word_size)
	{
		std::vector<std::string> words;
	
		std::string word;

		boost::replace_all(line, ",", " ");
		boost::replace_all(line, ".", " ");
		boost::replace_all(line, ": ", " ");
		boost::replace_all(line, ";", " ");
		boost::replace_all(line, "\"", " ");
		boost::replace_all(line, "'", " ");
		boost::replace_all(line, "?", " ");
		boost::replace_all(line, "!", " ");
		boost::replace_all(line, "-", " ");
		boost::replace_all(line, "—", " ");
		boost::replace_all(line, "(", " ");
		boost::replace_all(line, ")", " ");
		boost::replace_all(line, ">", "> ");
		boost::replace_all(line, "<", " <");
		boost::replace_all(line, "T", " T"); // Time identifier for datetimes 
		std::stringstream s(line);
		while (getline(s, word, delimeter))
		{
			if (word.size() >= min_word_size)
			{
				words.push_back(word);
			}
		}

		return words;
	}

	std::unordered_map<std::string, std::string> ContentParser::read_simple_vocabulary(const std::string& filename, const std::locale& locale)
	{
		std::unordered_map<std::string, std::string> words;
		std::string word;

		std::fstream fin;
		fin.imbue(locale);

		fin.open(filename, std::ios::in);
		
		if (!fin.is_open())
		{
			std::cerr << "Cannot open file: " << filename << std::endl;
		}
		else
		{
			while (getline(fin, word))
			{
				words[word] = word;
			}
		}

		return words;
	}

	std::vector<std::string> ContentParser::parse_by_lines(const std::string& filename, const std::locale& locale)
	{
		std::vector<std::string> lines;
		std::string line;

		std::fstream fin;
		fin.imbue(locale);

		fin.open(filename, std::ios::in);
		
		if (!fin.is_open())
		{
			std::cerr << "Cannot open file: " << filename << std::endl;
		}
		else
		{
			while (getline(fin, line))
			{
				lines.push_back(line);
			}
		}

		return lines;
	}

	std::unordered_map<std::string, int> ContentParser::read_vocabulary_and_tag(const std::string& filename, const std::locale& locale, int start_tag, int end_tag)
	{
		std::unordered_map<std::string, int> map; 
		
		std::vector<std::string> words;
		std::string word;

		std::fstream fin;
		fin.imbue(locale);

		fin.open(filename, std::ios::in);
		
		if (!fin.is_open())
		{
			std::cerr << "Cannot open file: " << filename << std::endl;
		}
		else
		{
			while (getline(fin, word))
			{
				words.push_back(word);
			}
		}

		int i = start_tag;
		for (auto w : words) 
		{
			if (i > end_tag)
			{
				i = start_tag;
			}
			map[w] = i;
			i++;
		}

		return map;
	}

	
	std::vector<std::string> ContentParser::readFilePaths(std::string dirname, bool recursively)
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


	std::vector<std::string> ContentParser::selectHtmlFiles(std::string dirname, bool recursively)
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

}  // namespace news_clustering
#endif
