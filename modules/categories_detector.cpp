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
		std::vector<Language>& languages, 
		std::unordered_map<Language, TextEmbedder>& text_embedders, 
		//std::unordered_map<Language, Word2Vec>& word2vec_embedders, 
		std::unordered_map<news_clustering::Language, std::locale>& locales, 
		std::unordered_map<news_clustering::Language, std::vector<std::vector<std::string>>>& categories
	) : languages_(languages), locales_(locales), categories_(categories), text_embedders_(text_embedders)
	{
	}


	template <typename T>
	std::vector<size_t> sort_indexes(const std::vector<T> &v) {

	  // initialize original index locations
	  std::vector<size_t> idx(v.size());
	  iota(idx.begin(), idx.end(), 0);

	  // sort indexes based on comparing values in v
	  sort(idx.begin(), idx.end(),
		   [&v](size_t i1, size_t i2) {return v[i1] > v[i2];});

	  return idx;
	}

	
	std::unordered_map<int, std::vector<std::string>> CategoriesDetector::detect_categories(
		std::unordered_map<std::string, news_clustering::Language>& file_names, 
		std::unordered_map<std::string, std::vector<std::string>>& contents, 
		std::unordered_map<news_clustering::Language, std::vector<float>> category_detect_levels
	)
	{
		std::unordered_map<int, std::vector<std::string>> result;
		
		std::vector<std::string> content;
		std::vector<double> text_distances;
		std::vector<int> text_embedding;
		std::vector<int> category_embedding;
		std::unordered_map<news_clustering::Language, std::vector<std::vector<int>>> category_embeddings;
		Language language;

		auto cosineDistance = metric::Cosine<double>();

		std::vector<double>::iterator max_it;
		size_t max_index;
		
		for (auto i = categories_.begin(); i != categories_.end(); i++)
		{
			language = i->first;
			for (auto category : i->second)
			{
				category_embedding = text_embedders_[language](category, locales_[language]);
				category_embeddings[language].push_back(category_embedding);
			}
		}
		
		for (auto i = file_names.begin(); i != file_names.end(); i++) 
		{			
			content = contents[i->first];
			language = file_names[i->first];
			// content embedding
			text_embedding = text_embedders_[language](content, locales_[language]);
			
			text_distances.clear();
			//text_distances = text_embedders_[i->second].texts_distance(content, categories_[i->second], locales_[i->second]);
			for (auto category : category_embeddings[language])
			{
				text_distances.push_back(cosineDistance(text_embedding, category));
			}
			
			max_it = std::max_element(text_distances.begin(), text_distances.end());
			max_index = std::distance(text_distances.begin(), max_it);
			
			bool category_found = false;
			for (auto index: sort_indexes(text_distances)) 
			{
				if (text_distances[index] > category_detect_levels[language][index])
				{
					result[index].push_back(i->first);
					category_found = true;
					break;
				}
			}
			if (!category_found)
			{
				result[-1].push_back(i->first);
			}
		}

		return result;
	}

}  // namespace news_clustering
#endif
