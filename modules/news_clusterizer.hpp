/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_NEWS_CLUSTERIZER_HPP
#define _NEWS_CLUSTERING_NEWS_CLUSTERIZER_HPP

#include "languages.hpp"
#include "content_parser.hpp"
#include "modules/text_embedding.hpp"

namespace news_clustering {

	/**
	 * @class NewsClusterizer
	 * 
	 * @brief NewsClusterizer
	 */
	struct NewsClusterizer {
		
		NewsClusterizer(
			const std::vector<Language>& languages, 
			std::unordered_map<news_clustering::Language, std::locale>& locales, 
			std::unordered_map<news_clustering::Language, std::string> word2vec_clustered_vocab_paths
		);

		/**
		 * @brief 
		 * @return 
		 */
		std::unordered_map<std::string, std::vector<std::string>> clusterize(std::unordered_map<std::string, news_clustering::Language> file_names);

	private:

		ContentParser content_parser = news_clustering::ContentParser();
		std::vector<Language> languages_;
		std::unordered_map<news_clustering::Language, std::locale>& locales_;
		std::unordered_map<news_clustering::Language, TextEmbedder> vocabs;
	};

}  // namespace news_clustering

#include "news_clusterizer.cpp"

#endif  // Header Guard
