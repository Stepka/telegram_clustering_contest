/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_NEWS_RANGER_CPP
#define _NEWS_CLUSTERING_NEWS_RANGER_CPP

#include "news_ranger.hpp"


namespace news_clustering {
	

	NewsRanger::NewsRanger(
		const std::vector<Language>& languages, 
		std::unordered_map<news_clustering::Language, std::locale>& locales, 
		std::unordered_map<news_clustering::Language, std::string> word2vec_clustered_vocab_paths
	) : languages_(languages), locales_(locales)
	{
		for (auto i = 0; i < languages.size(); i++)
		{			
			vocabs[languages[i]] = news_clustering::TextEmbedder(word2vec_clustered_vocab_paths[languages[i]]);
		}
	}

	
	std::unordered_map<std::string, std::vector<int>> NewsRanger::arrange(std::unordered_map<std::string, news_clustering::Language> file_names)
	{
		std::unordered_map<std::string, std::vector<int>> result;
		
		std::vector<std::string> content;
		std::vector<int> text_embedding;

		std::vector<float>::iterator max_it;
		
		for (auto i = file_names.begin(); i != file_names.end(); i++) {
			
			content = content_parser.parse(i->first, locales_[i->second]);   
			text_embedding = vocabs[i->second](content, locales_[i->second]);
			
			result[i->first] = text_embedding;
		}

		return result;
	}

}  // namespace news_clustering
#endif
