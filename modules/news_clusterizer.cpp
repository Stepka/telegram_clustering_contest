/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_NEWS_CLUSTERIZER_CPP
#define _NEWS_CLUSTERING_NEWS_CLUSTERIZER_CPP

#include <numeric>      
#include <algorithm>    
#include "news_clusterizer.hpp"
#include "../metric/modules/mapping.hpp"


namespace news_clustering {
	

	NewsClusterizer::NewsClusterizer(
		std::vector<Language>& languages, 
		std::unordered_map<Language, TextEmbedder>& text_embedders, 
		//std::unordered_map<Language, Word2Vec>& word2vec_embedders, 
		std::unordered_map<news_clustering::Language, std::locale>& locales
	) : languages_(languages), locales_(locales), text_embedders_(text_embedders)
	{
	}


	template <typename T>
	std::vector<size_t> NewsClusterizer::sort_indexes(const std::vector<T> &v) {

	  // initialize original index locations
	  std::vector<size_t> idx(v.size());
	  std::iota(idx.begin(), idx.end(), 0);

	  // sort indexes based on comparing values in v
	  sort(idx.begin(), idx.end(),
		   [&v](size_t i1, size_t i2) {return v[i1] > v[i2];});

	  return idx;
	}
	
	std::unordered_map<std::string, std::vector<std::string>> NewsClusterizer::clusterize(
			std::unordered_map<std::string, news_clustering::Language>& file_names, 
			std::unordered_map<std::string, std::vector<std::string>>& contents, 
			std::unordered_map<std::string, std::string>& titles, 
			float eps, std::size_t minpts
		)
	{
		std::unordered_map<std::string, std::vector<std::string>> clustered_by_filename;
		std::unordered_map<std::string, std::vector<std::string>> result;
		
		std::vector<std::string> content;
		std::unordered_map<Language, std::vector<std::string>> indexed_file_names;
		std::vector<int> text_embedding;
		std::unordered_map<Language, std::vector<std::vector<int>>> text_embeddings;
		std::unordered_map<std::string, std::vector<int>> text_embeddings_by_filename;
		std::unordered_map<std::string, std::vector<int>> text_embeddings_for_titles;
		int seed;

		std::vector<float>::iterator max_it;
		
		for (auto i = file_names.begin(); i != file_names.end(); i++) 
		{			
			content = contents[i->first];   
			text_embedding = text_embedders_[i->second](content, locales_[i->second]);
			
			indexed_file_names[i->second].push_back(i->first);
			text_embeddings[i->second].push_back(text_embedding);
			text_embeddings_by_filename[i->first] = text_embedding;
		}

		
		for (auto i = text_embeddings.begin(); i != text_embeddings.end(); i++) 
		{
			metric::Matrix<std::vector<int>, metric::Euclidian<float>> distance_matrix(i->second);

			auto[assignments, seeds, counts] = metric::dbscan(distance_matrix, eps, minpts);


			for (size_t k = 0; k < indexed_file_names[i->first].size(); k++)
			{
				if (assignments[k] > 0)
				{
					seed = seeds[assignments[k] - 1];
					clustered_by_filename[indexed_file_names[i->first][seed]].push_back(indexed_file_names[i->first][k]);
				}
				else
				{
					clustered_by_filename[indexed_file_names[i->first][k]].push_back(indexed_file_names[i->first][k]);
				}
			}

			std::cout << "assignments:" << std::endl;
			for (size_t i = 0; i < assignments.size(); i++)
			{
				if (i < assignments.size() - 1)
				{
					std::cout << assignments[i] << ", ";
				}
				else
				{
					std::cout << assignments[i] << std::endl;
				}
			}
			std::cout << '\n';

			std::cout << "seeds:" << std::endl;
			for (size_t i = 0; i < seeds.size(); i++)
			{
				if (i < seeds.size() - 1)
				{
					std::cout << seeds[i] << ", ";
				}
				else
				{
					std::cout << seeds[i] << std::endl;
				}
			}
			std::cout << '\n';

			std::cout << "counts:" << std::endl;
			for (size_t i = 0; i < counts.size(); i++)
			{
				if (i < counts.size() - 1)
				{
					std::cout << counts[i] << ", ";
				}
				else
				{
					std::cout << counts[i] << std::endl;
				}
			}
			std::cout << '\n' << std::endl;
		}

		// sorting by relevamce
		//std::vector<std::vector<std::string>> title_from_cluster;
		std::vector<float> text_distances;
		auto cosineDistance = metric::Cosine<float>();
		for (auto k = clustered_by_filename.begin(); k != clustered_by_filename.end(); k++)
		{			
			// splitted title
			content = content_parser.split_string(titles[k->first]);   
			// title embedding
			text_embedding = text_embedders_[file_names[k->first]](content, locales_[file_names[k->first]]);
				
			//title_from_cluster.clear();
			if (k->second.size() > 1)
			{
				text_distances.clear();
				for (auto j = 0; j < k->second.size(); j++)
				{
					//title_from_cluster.push_back(content_parser.split_string(titles[k->second[j]]));
					text_distances.push_back(cosineDistance(text_embedding, text_embeddings_by_filename[k->second[j]]));
				}

				//text_distances = word2vec_embedders_[file_names[k->first]].texts_distance(content, title_from_cluster, locales_[file_names[k->first]]);

				auto sorted_indexes = sort_indexes(text_distances);
				for (auto j : sorted_indexes)
				{
					//std::cout << "    " << text_distances[j] << ": " << titles[k->second[j]] << " " << k->second[j] << std::endl;
					result[k->first].push_back(k->second[j]);
				}
			}
			else
			{
				result[k->first].push_back(k->second[0]);
			}
		}
		

		return result;
	}

}  // namespace news_clustering
#endif
