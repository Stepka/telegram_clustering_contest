/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_CATEGORIES_DETECTOR_HPP
#define _NEWS_CLUSTERING_CATEGORIES_DETECTOR_HPP

#include "languages.hpp"
#include "content_parser.hpp"
#include "modules/text_embedding.hpp"

namespace news_clustering {

	/**
	 * @class CategoriesDetector
	 * 
	 * @brief CategoriesDetector
	 */
	struct CategoriesDetector {
		
		CategoriesDetector(
			std::vector<Language>& languages, 
			std::unordered_map<news_clustering::Language, Word2Vec>& embedders, 
			std::unordered_map<news_clustering::Language, std::locale>& locales, 
			std::unordered_map<news_clustering::Language, std::vector<std::vector<std::string>>>& categories
		);

		/**
		 * @brief 
		 * @return 
		 */
		std::unordered_map<std::string, std::vector<std::string>> detect_categories(
			std::unordered_map<std::string, news_clustering::Language>& file_names, 
			std::unordered_map<std::string, std::vector<std::string>>& contents
		);

	private:

		ContentParser content_parser = news_clustering::ContentParser();
		std::vector<Language>& languages_;
		std::unordered_map<news_clustering::Language, std::locale>& locales_;
		std::unordered_map<news_clustering::Language, Word2Vec>& text_embedders_;
		std::unordered_map<news_clustering::Language, std::vector<std::vector<std::string>>>& categories_;
	};

}  // namespace news_clustering

#include "categories_detector.cpp"

#endif  // Header Guard
