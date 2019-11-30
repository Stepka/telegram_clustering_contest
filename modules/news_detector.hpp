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
	class NewsDetector {
		
	public:

		NewsDetector(
			std::vector<Language>& languages, 
			std::unordered_map<news_clustering::Language, std::locale>& locales
		);

		/**
		 * @brief 
		 * @return 
		 */
		std::unordered_map<bool, std::vector<std::string>> detect_news(
			std::unordered_map<std::string, news_clustering::Language>& file_names, 
			std::unordered_map<std::string, std::vector<std::string>>& contents, 
			std::unordered_map<std::string, std::vector<std::vector<int>>>& dates, 
			std::unordered_map<std::string, std::vector<std::string>>& name_entities
		);

	private:

		ContentParser content_parser = news_clustering::ContentParser();
		std::vector<Language>& languages_;
		std::unordered_map<news_clustering::Language, std::locale>& locales_;
	};

}  // namespace news_clustering

#include "news_detector.cpp"

#endif  // Header Guard
