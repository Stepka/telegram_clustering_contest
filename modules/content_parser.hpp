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
	class ContentParser {
		
	public:

		explicit ContentParser() = default;

		/**
		 * @brief 
		 * @return 
		 */
		std::vector<std::string> parse(const std::string& filename, const std::locale& locale, char delimeter = ' ', int min_word_size = 1);

		/**
		 * @brief 
		 * @return 
		 */
		std::vector<std::string> parse_by_lines(const std::string& filename, const std::locale& locale);
		
		/**
		 * @brief 
		 * @return 
		 */
		std::unordered_map<std::string, std::string> read_simple_vocabulary(const std::string& filename, const std::locale& locale);
		
		/**
		 * @brief 
		 * @return 
		 */
		std::unordered_map<std::string, int> read_vocabulary_and_tag(const std::string& filename, const std::locale& locale, int start_tag, int end_tag);
		
		/**
		 * @brief 
		 * @return 
		 */
		std::vector<std::vector<std::string>> parse_categories(const std::string& filename, const std::locale& locale, char delimeter = ' ');
		
		/**
		 * @brief 
		 * @return 
		 */
		std::vector<std::string> split_string(std::string& line, char delimeter = ' ', int min_word_size = 1);
	};

}  // namespace news_clustering

#include "content_parser.cpp"

#endif  // Header Guard
