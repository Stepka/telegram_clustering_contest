/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_NEWS_DETECTOR_HPP
#define _NEWS_CLUSTERING_NEWS_DETECTOR_HPP

#include "languages.hpp"
#include "content_parser.hpp"

namespace news_clustering {

	/**
	 * @class LanguageDetector
	 * 
	 * @brief LanguageDetector
	 */
	struct NewsDetector {
		
		using Vocab = std::vector<std::string>;

		explicit NewsDetector();

		/**
		 * @brief 
		 * @return 
		 */
		std::unordered_map<std::string, bool> detect_news(std::unordered_map<std::string, news_clustering::Language> file_names);
			
		/**
		 * @brief 
		 * @return dd.mm.yyyy, f.e. 28.01.2019
		 */
		std::vector<int> checkIfDate(std::string part_1, std::string part_2, std::string part_3, std::unordered_map<std::string, int> day_names, std::unordered_map<std::string, int> month_names,
			std::locale locale, news_clustering::Language language);
		
		/**
		 * @brief 
		 * @return 
		 */
		int extractYear(const char *p);

		/**
		 * @brief 
		 * @return 
		 */
		std::vector<std::vector<int>> findDates(std::vector<std::string> content, std::unordered_map<std::string, int> day_names, std::unordered_map<std::string, int> month_names,
			std::locale locale, news_clustering::Language language);

	private:

		ContentParser content_parser = news_clustering::ContentParser();
		std::vector<Language> languages_;
		std::vector<Vocab> vocabs;
	};

}  // namespace news_clustering

#include "news_detector.cpp"

#endif  // Header Guard
