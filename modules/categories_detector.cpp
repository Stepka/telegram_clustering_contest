/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_CATEGORIES_DETECTOR_CPP
#define _NEWS_CLUSTERING_CATEGORIES_DETECTOR_CPP

#include "categories_detector.hpp"


namespace news_clustering {
	

	CategoriesDetector::CategoriesDetector(
		const std::vector<Language>& languages, 
		std::unordered_map<news_clustering::Language, std::locale>& locales, 
		std::unordered_map<news_clustering::Language, std::string> word2vec_vocab_paths, 
		std::unordered_map<news_clustering::Language, std::vector<std::vector<std::string>>> categories
	) : languages_(languages), locales_(locales), categories_(categories)
	{
		for (auto i = 0; i < languages.size(); i++)
		{			
			vocabs[languages[i]] = news_clustering::Word2Vec(word2vec_vocab_paths[languages[i]], languages[i]);
		}
	}

	
	std::unordered_map<std::string, std::vector<std::string>> CategoriesDetector::detect_categories(std::unordered_map<std::string, news_clustering::Language> file_names)
	{
		std::unordered_map<std::string, std::vector<std::string>> result;
		
		std::vector<std::string> content;
		std::vector<float> text_distances;

		std::vector<float>::iterator max_it;
		
		for (auto i = file_names.begin(); i != file_names.end(); i++) {

			content = content_parser.parse(i->first, locales_[i->second]);
			text_distances = vocabs[i->second].texts_distance(content, categories_[i->second], locales_[i->second]);
			
			max_it = std::max_element(text_distances.begin(), text_distances.end());
			// first element in the categories tags is a name of the category
			result[categories_[i->second][std::distance(text_distances.begin(), max_it)][0]].push_back(i->first);
		}

		return result;
	}

}  // namespace news_clustering
#endif
