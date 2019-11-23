/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2018 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_TEXT_EMBEDDING_CPP
#define _NEWS_CLUSTERING_TEXT_EMBEDDING_CPP

#include "text_embedding.hpp"


namespace news_clustering {

	TextEmbedder::TextEmbedder(const std::vector<std::string>& vocab_paths)
	{
		FILE *read_again_file_pointer;
		char str [80];
		long long cluster_id;
		long long vocab_size;
		long long num_clusters;
		for (auto path : vocab_paths)
		{			
				
			std::cout << path << std::endl;
			VocabClusters vocab_clusters;
			if ((read_again_file_pointer = fopen(path.c_str(), "rb")) == NULL) {
				std::cout << "Cannot open file.\n";
				//exit (1);
			}

			fscanf (read_again_file_pointer, "%lld %lld\n", &vocab_size, &num_clusters);	
			for (auto i = 0; i < vocab_size; i++)
			{
				fscanf (read_again_file_pointer, "%s ", str);
				fscanf (read_again_file_pointer, "%lld\n", &cluster_id);	
				//fread(&cluster_id, sizeof(int), 1, read_again_file_pointer);
				//fscanf (read_again_file_pointer, "\n");
				vocab_clusters[std::string(str)] = cluster_id;
				if(cluster_id >= num_clusters)
					std::cout << std::string(str) << " " << cluster_id << std::endl;
			}
			fclose (read_again_file_pointer);
			
			vocabs.push_back(vocab_clusters);
		}
	}

	std::vector<int> TextEmbedder::operator()(const std::vector<std::string>& words, std::locale locale)
	{
		std::vector<int> result(30, 0);
		auto vocab = vocabs[0];
		for (auto word : words)
		{
			if (vocab.find(boost::locale::to_lower(word, locale)) != vocab.end())
			{
				//std::cout << boost::locale::to_lower(word, locale) << " " << vocab[boost::locale::to_lower(word, locale)] << std::endl;
				result[vocab[boost::locale::to_lower(word, locale)]]++;
			}
		}

		return result;
	}

}  // namespace news_clustering
#endif
