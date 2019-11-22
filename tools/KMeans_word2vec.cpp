/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Panda Team
*/

#include <unordered_map>
#include <algorithm>

#include "metric/modules/mapping.hpp"
#include "metric/modules/distance.hpp"



int main()
{
	std::cout << "KMeans have started" << std::endl;
	std::cout << '\n';
	
	std::vector<std::string> words;
	std::vector<float> embedding;
	std::vector<std::vector<float>> embeddings;
	
	std::unordered_map<std::string, std::vector<float>> vocab;
	
	char str [80];
	FILE *read_again_file_pointer;
	float value;
	long long original_vocab_size, layer1_size;

	long cut_vocab_size = 10000;
	
	std::string cut_file_name = "../data/embedding/GoogleNews-vectors-" + std::to_string(cut_vocab_size) + "-words.bin";
	if ((read_again_file_pointer=fopen(cut_file_name.c_str(), "rb"))==NULL) {
		printf("Cannot open file.\n");
		exit (1);
	}

	fscanf (read_again_file_pointer, "%lld %lld\n", &original_vocab_size, &layer1_size);	
	std::cout << "vocab size: " << original_vocab_size << " embedding dimension: " << layer1_size << std::endl;
	for (auto i = 0; i < original_vocab_size; i++)
	{
		//if (i % 10000 == 0) std::cout << i << std::endl;
		fscanf (read_again_file_pointer, "%s ", str);
		words.push_back(std::string(str));
		embedding.clear();
		for (auto j = 0; j < layer1_size; j++)
		{
			fread(&value, sizeof(float), 1, read_again_file_pointer);
			embedding.push_back(value);
		}
		fscanf (read_again_file_pointer, "\n");
		embeddings.push_back(embedding);

		vocab[words[i]] = embedding;
	}
	fclose (read_again_file_pointer);

	std::cout << std::endl;
	std::cout << "finish" << std::endl;


	
	auto t0 = std::chrono::steady_clock::now();
	auto t1 = std::chrono::steady_clock::now();

	int num_clusters = 4;

	auto[assignments, means, counts] = metric::kmeans(embeddings, num_clusters); // clusters the data in 4 groups.

	t1 = std::chrono::steady_clock::now();
	std::cout << "Total (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cout << std::endl;  

	std::vector<std::vector<std::string>> clusters(num_clusters);

	std::cout << "assignments:" << std::endl;
	for (size_t i = 0; i < assignments.size(); i++) 
	{
	/*	if (i < assignments.size() - 1)
		{
			std::cout << assignments[i] << ", ";
		}
		else
		{
			std::cout << assignments[i] << std::endl;
		}*/

		clusters[assignments[i]].push_back(words[i]);
	}
	std::cout << '\n';

	//std::cout << "means:" << std::endl;
	//for (size_t i = 0; i < means.size(); i++)
	//{
	//	for (size_t j = 0; j < means[i].size(); j++)
	//	{
	//		if (j < means[i].size() - 1)
	//		{
	//			std::cout << means[i][j] << ", ";
	//		}
	//		else
	//		{
	//			std::cout << means[i][j] << std::endl;
	//		}
	//	}
	//}
	//std::cout << '\n';

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

	std::cout << "clusters:" << std::endl;
	for (size_t i = 0; i < clusters.size(); i++)
	{
		std::cout << "cluster #" << i << ":" << std::endl;
		if (clusters[i].size() < 100)
		{
			for (size_t j = 0; j < clusters[i].size(); j++)
			{
				if (j < clusters[i].size() - 1)
				{
					std::cout << clusters[i][j] << ", ";
				}
				else
				{
					std::cout << clusters[i][j] << std::endl;
				}
			}
		}
	}
	std::cout << '\n';
	


	std::string input = "";
	while (input != "exit")
	{
		std::cout << "Type word:" << std::endl;
		std::getline(std::cin, input);
		std::cout << '\n';

		if (vocab.find(input) != vocab.end())
		{
			std::vector<float> input_embedding = vocab[input];

			auto cosineDistance = metric::Cosine<double>();

			std::unordered_map<std::string, float> distances;
			for (auto it = vocab.begin(); it != vocab.end(); ++it)
			{
				distances[it->first] = cosineDistance(input_embedding, it->second);
			}

			using pair = std::pair<std::string, float>;
			// create a empty vector of pairs
			std::vector<pair> vec;

			// copy key-value pairs from the map to the vector
			std::copy(distances.begin(),
				distances.end(),
				std::back_inserter<std::vector<pair>>(vec));

			// sort the vector by increasing order of its pair's second value
			// if second value are equal, order by the pair's first value
			std::sort(vec.begin(), vec.end(),
				[](const pair& l, const pair& r) {
				if (l.second != r.second)
					return l.second > r.second;

				return l.first > r.first;
			});

			// print the vector
			for (size_t i = 0; i < 100; i++)
			{
				std::cout << vec[i].first << " = " << vec[i].second << '\n';
			}
		}
		else
		{
			
			std::cout << input << " not exist in the vocab" << std::endl;
		}
	}
	

	return 0;

}
