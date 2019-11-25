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
	
	std::vector<std::string> ContentParser::parse(std::string filename, std::locale locale, char delimeter, int min_word_size)
	{
		std::vector<std::string> words;
	
		std::string line, word;

		std::fstream fin;
		fin.imbue(locale);

		fin.open(filename, std::ios::in);	

		while (getline(fin, line))
		{
			boost::replace_all(line, ",", " ");
			boost::replace_all(line, ".", " ");
			boost::replace_all(line, ":", " ");
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
		}

		return words;
	}

	std::vector<std::string> ContentParser::read_simple_vocabulary(std::string filename, std::locale locale)
	{
		std::vector<std::string> words;
		std::string word;

		std::fstream fin;
		fin.imbue(locale);

		fin.open(filename, std::ios::in);

		while (getline(fin, word))
		{
			words.push_back(word);
		}

		return words;
	}

	std::unordered_map<std::string, int> ContentParser::read_vocabulary_and_tag(std::string filename, std::locale locale, int start_tag, int end_tag)
	{
		std::vector<std::string> vocab = read_simple_vocabulary(filename, locale);
		std::unordered_map<std::string, int> map; 

		int i = start_tag;
		for (auto word : vocab)
		{
			if (i > end_tag)
			{
				i = start_tag;
			}
			map[word] = i;
			i++;
		}

		return map;
	}

}  // namespace news_clustering
#endif
