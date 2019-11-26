/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_NEWS_CLUSTERIZER_CPP
#define _NEWS_CLUSTERING_NEWS_CLUSTERIZER_CPP

#include "news_clusterizer.hpp"
#include "../metric/modules/mapping.hpp"


namespace news_clustering {
	

	NewsClusterizer::NewsClusterizer(
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

	
	std::unordered_map<std::string, std::vector<std::string>> NewsClusterizer::clusterize(std::unordered_map<std::string, news_clustering::Language> file_names)
	{
		std::unordered_map<std::string, std::vector<std::string>> result;
		
		std::vector<std::string> content;
		std::unordered_map<Language, std::vector<std::string>> indexed_file_names;
		std::vector<int> text_embedding;
		std::unordered_map<Language, std::vector<std::vector<int>>> text_embeddings;

		std::vector<float>::iterator max_it;
		
		for (auto i = file_names.begin(); i != file_names.end(); i++) {
			
			content = content_parser.parse(i->first, locales_[i->second]);   
			text_embedding = vocabs[i->second](content, locales_[i->second]);
			
			
			indexed_file_names[i->second].push_back(i->first);
			text_embeddings[i->second].push_back(text_embedding);
		}

		
		for (auto i = text_embeddings.begin(); i != text_embeddings.end(); i++) 
		{
			metric::Matrix<std::vector<int>, metric::Euclidian<float>> distance_matrix(i->second);

			auto[assignments, seeds, counts] = metric::dbscan(distance_matrix, (float)16.0, 2);


			for (size_t k = 0; k < indexed_file_names[i->first].size(); k++)
			{
				result[i->first.to_string() + "_" + std::to_string(assignments[k])].push_back(indexed_file_names[i->first][k]);
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

		return result;
	}

}  // namespace news_clustering
#endif
