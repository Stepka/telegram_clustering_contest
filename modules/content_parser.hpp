/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_CONTENT_PARSER_HPP
#define _NEWS_CLUSTERING_CONTENT_PARSER_HPP


namespace news_clustering {

	/**
	 * @class ContentParser
	 * 
	 * @brief ContentParser
	 */
	struct ContentParser {

		explicit ContentParser() = default;

		/**
		 * @brief 
		 * @return 
		 */
		std::vector<std::string> parse(std::string filename, std::locale locale, char delimeter = ' ', int min_word_size = 1);

		/**
		 * @brief 
		 * @return 
		 */
		std::vector<std::string> parse_by_lines(std::string filename, std::locale locale);
		
		/**
		 * @brief 
		 * @return 
		 */
		std::vector<std::string> read_simple_vocabulary(std::string filename, std::locale locale);
		
		/**
		 * @brief 
		 * @return 
		 */
		std::unordered_map<std::string, int> read_vocabulary_and_tag(std::string filename, std::locale locale, int start_tag, int end_tag);
	};

}  // namespace news_clustering

#include "content_parser.cpp"

#endif  // Header Guard
