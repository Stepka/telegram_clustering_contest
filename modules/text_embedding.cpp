/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_TEXT_EMBEDDING_CPP
#define _NEWS_CLUSTERING_TEXT_EMBEDDING_CPP

#include "text_embedding.hpp"


namespace news_clustering {

	TextEmbedder::TextEmbedder(const std::string path)
	{
		FILE *read_again_file_pointer;
		char str [80];
		long long cluster_id;	
		long long vocab_size;
				
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
		}
		fclose (read_again_file_pointer);
	}

	std::vector<int> TextEmbedder::operator()(const std::vector<std::string>& words, std::locale locale)
	{
		std::vector<int> result(num_clusters, 0);
		for (auto word : words)
		{
			if (vocab_clusters.find(boost::locale::to_lower(word, locale)) != vocab_clusters.end())
			{
				result[vocab_clusters[boost::locale::to_lower(word, locale)]]++;
			}
		}

		return result;
	}

	//

	Word2Vec::Word2Vec(const std::string path)
	{
		FILE *read_again_file_pointer;
		char str [80];
		long long vocab_size;
		long long layer1_size;
		float value;
		std::vector<float> embedding;		
				
		if ((read_again_file_pointer = fopen(path.c_str(), "rb")) == NULL) {
			std::cout << "Cannot open file.\n";
			//exit (1);
		}

		fscanf (read_again_file_pointer, "%lld %lld\n", &vocab_size, &layer1_size);	
		for (auto i = 0; i < vocab_size; i++)
		{
			fscanf (read_again_file_pointer, "%s ", str);
			embedding.clear();
			for (auto j = 0; j < layer1_size; j++)
			{
				fread(&value, sizeof(float), 1, read_again_file_pointer);
				embedding.push_back(value);
			}
			fscanf (read_again_file_pointer, "\n");

			vocab_embeddings[std::string(str)] = embedding;
		}
		fclose (read_again_file_pointer);
	}

	std::vector<float> Word2Vec::texts_distance(const std::vector<std::string>& long_text, const std::vector<std::vector<std::string>>& short_texts, std::locale locale)
	{
		std::vector<float> result;
		std::vector<float> distances;
		float mean_distance;
		float num_closest_distances = 5;
		
		std::string single_word_lower;
		std::string text_word_lower;
		
		std::vector<float> single_word_embedding;
		std::vector<float> text_word_embedding;

		auto cosineDistance = metric::Cosine<float>();
		
		for (auto short_text : short_texts)
		{
			distances.clear();
			for (auto single_word : short_text)
			{
				single_word_lower = boost::locale::to_lower(single_word, locale);
				if (vocab_embeddings.find(single_word_lower) != vocab_embeddings.end())
				{
					single_word_embedding = vocab_embeddings[single_word_lower];
					for (auto text_word : long_text)
					{
						text_word_lower = boost::locale::to_lower(text_word, locale);
						if (vocab_embeddings.find(text_word_lower) != vocab_embeddings.end())
						{
							text_word_embedding = vocab_embeddings[text_word_lower];
							//std::cout << boost::locale::to_lower(word, locale) << " " << vocab[boost::locale::to_lower(word, locale)] << std::endl;

							//std::cout << "    " << cosineDistance(single_word_embedding, text_word_embedding) << std::endl;
							distances.push_back(cosineDistance(single_word_embedding, text_word_embedding));
						}
					}

				}
			}

			std::sort(distances.begin(), distances.end(), std::greater<float>());
			mean_distance = 0;
			for (size_t i = 0; i < num_closest_distances; i++)
			{
				//std::cout << distances[i] << '\n';
				mean_distance += distances[i];
			}
			mean_distance /= num_closest_distances;
			//std::cout << "mean_distance = " << mean_distance << '\n';
			result.push_back(mean_distance);
		}

		return result;
	}

}  // namespace news_clustering
#endif
