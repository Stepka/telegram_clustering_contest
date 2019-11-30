/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_NEWS_RANGER_HPP
#define _NEWS_CLUSTERING_NEWS_RANGER_HPP

#include "languages.hpp"
#include "content_parser.hpp"
#include "modules/text_embedding.hpp"

namespace news_clustering {

	/**
	 * @class NewsRanger
	 * 
	 * @brief NewsRanger
	 */
	class NewsRanger {
		
	public:

		using NewsThread = std::unordered_map<std::string, std::vector<std::string>>;
		
		NewsRanger(
			std::vector<Language>& languages, 
			std::unordered_map<Language, TextEmbedder>& embedders, 
			std::unordered_map<news_clustering::Language, std::locale>& locales, 
			std::vector<int>& today
		);

		/**
		 * @brief 
		 * @return 
		 */
		std::vector<std::unordered_map<std::string, std::vector<std::string>>> arrange(
			std::unordered_map<std::string, std::vector<std::string>>& clustered_articles, 
			std::unordered_map<std::string, std::vector<std::vector<int>>>& dates, 
			std::unordered_map<std::string, std::vector<std::string>>& name_entities
		);

	private:

		ContentParser content_parser = news_clustering::ContentParser();
		std::vector<Language>& languages_;
		std::unordered_map<news_clustering::Language, std::locale>& locales_;
		std::unordered_map<news_clustering::Language, TextEmbedder>& text_embedders_;
		
		std::vector<int>& today_;
		
		template <typename T>
		std::vector<size_t> sort_indexes(const std::vector<T> &v);
	};

}  // namespace news_clustering

#include "news_ranger.cpp"

#endif  // Header Guard
