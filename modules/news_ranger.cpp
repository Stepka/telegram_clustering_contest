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
		std::vector<Language>& languages, 
		std::unordered_map<Language, TextEmbedder>& embedders, 
		std::unordered_map<news_clustering::Language, std::locale>& locales
	) : languages_(languages), locales_(locales), embedders_(embedders)
	{
	}

	
	std::unordered_map<std::string, std::vector<int>> NewsRanger::arrange(
		std::unordered_map<std::string, news_clustering::Language>& file_names, 
		std::unordered_map<std::string, std::vector<std::string>>& contents
	)
	{
		std::unordered_map<std::string, std::vector<int>> result;
		
		std::vector<std::string> content;
		std::vector<int> text_embedding;

		std::vector<float>::iterator max_it;
		
		for (auto i = file_names.begin(); i != file_names.end(); i++) {
			
			content = contents[i->first];   
			text_embedding = embedders_[i->second](content, locales_[i->second]);
			
			result[i->first] = text_embedding;
		}

		return result;
	}

}  // namespace news_clustering
#endif
