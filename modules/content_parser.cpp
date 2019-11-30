/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_CONTENT_PARSER_CPP
#define _NEWS_CLUSTERING_CONTENT_PARSER_CPP

#include "content_parser.hpp"
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>


namespace news_clustering {
	
	std::vector<std::string> ContentParser::parse(const std::string& filename, const std::locale& locale, char delimeter, int min_word_size)
	{
		std::vector<std::string> words, result;
	
		std::string line, word;

		std::fstream fin;
		fin.imbue(locale);

		fin.open(filename, std::ios::in);	

		while (getline(fin, line))
		{
			words = split_string(line, delimeter, min_word_size);
			result.insert(result.end(), words.begin(), words.end());
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

		while (getline(fin, word))
		{
			words[word] = word;
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

		while (getline(fin, line))
		{
			lines.push_back(line);
		}

		return lines;
	}

	std::unordered_map<std::string, int> ContentParser::read_vocabulary_and_tag(const std::string& filename, const std::locale& locale, int start_tag, int end_tag)
	{
		std::unordered_map<std::string, std::string> vocab = read_simple_vocabulary(filename, locale);
		std::unordered_map<std::string, int> map; 

		int i = start_tag;
		for (auto w = vocab.begin(); w != vocab.end(); w++) 
		{
			if (i > end_tag)
			{
				i = start_tag;
			}
			map[w->first] = i;
			i++;
		}

		return map;
	}

}  // namespace news_clustering
#endif
