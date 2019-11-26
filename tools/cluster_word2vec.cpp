/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/

#include <unordered_map>
#include <algorithm>

#include "metric/modules/mapping.hpp"
#include "metric/modules/distance.hpp"



int main(int argc, char *argv[]) 
{
	std::cout << "Clustering have started" << std::endl;
	std::cout << '\n';
	
	std::vector<std::string> words;
	std::vector<float> embedding;
	std::vector<std::vector<float>> embeddings;
	
	std::unordered_map<std::string, std::vector<float>> vocab;
	
	char str [80];
	FILE *read_again_file_pointer;
	float value;
	long long original_vocab_size, layer1_size;

	long long num_clusters;
	
	std::string original_file_name;

	//

	if (argc > 1)
	{
		original_file_name = argv[1];
		std::cout << "Using data path: " << original_file_name << std::endl;  
	}
	else
	{
		std::cout << "You haven't specified original vocab path, pleaes specify path" << std::endl;  
		return EXIT_FAILURE;
	}

	if (argc > 2)
	{
		num_clusters = std::atoll(argv[2]);
		std::cout << "Number of clusters: " << num_clusters << std::endl;  
	}
	else
	{
		std::cout << "You haven't specified clusters number, pleaes specify it" << std::endl;  
		return EXIT_FAILURE;
	}
	std::cout << std::endl;

	// read

	if ((read_again_file_pointer = fopen(original_file_name.c_str(), "rb")) == NULL) {
		printf("Cannot open file.\n");
		return EXIT_FAILURE;
	}
	
	std::cout << "reading started..." << std::endl;
	fscanf (read_again_file_pointer, "%lld %lld\n", &original_vocab_size, &layer1_size);	
	std::cout << "vocab size: " << original_vocab_size << " embedding dimension: " << layer1_size << std::endl;
	for (auto i = 0; i < original_vocab_size; i++)
	{
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
		
		if ((i + 1) % 10000 == 0) std::cout << "progress: " << (i + 1) << " from " << original_vocab_size << std::endl;
	}
	fclose (read_again_file_pointer);
	
	std::cout << "reading finished" << std::endl;
	std::cout << std::endl;

	// cluster
	
	std::cout << "clustering started..." << std::endl;

	auto t0 = std::chrono::steady_clock::now();
	auto t1 = std::chrono::steady_clock::now();

	auto[assignments, means, counts] = metric::kmeans(embeddings, num_clusters); // clusters the data in num_clusters groups.


	std::vector<std::vector<std::string>> clusters(num_clusters);

	for (size_t i = 0; i < assignments.size(); i++) 
	{

		clusters[assignments[i]].push_back(words[i]);
	}

	std::cout << "counts: ";
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

	//std::cout << "clusters:" << std::endl;
	//for (size_t i = 0; i < clusters.size(); i++)
	//{
	//	std::cout << "cluster #" << i << ":" << std::endl;
	//	if (clusters[i].size() < 100)
	//	{
	//		for (size_t j = 0; j < clusters[i].size(); j++)
	//		{
	//			if (j < clusters[i].size() - 1)
	//			{
	//				std::cout << clusters[i][j] << ", ";
	//			}
	//			else
	//			{
	//				std::cout << clusters[i][j] << std::endl;
	//			}
	//		}
	//	}
	//}

	t1 = std::chrono::steady_clock::now();
	std::cout << "clustering finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cout << std::endl;

	// write
	
	FILE *write_file_pointer;
	int cluster_id;

	
	std::size_t pos = original_file_name.find(".bin");  
	std::string cluster_file_name = original_file_name.substr(0, pos) + "-" + std::to_string(num_clusters) + "-clusters.bin";

	if ((write_file_pointer = fopen(cluster_file_name.c_str(), "wb")) == NULL) {
		printf("Cannot open file.\n");
		return EXIT_FAILURE;
	}
	
	std::cout << "writing started..." << std::endl;
	fprintf(write_file_pointer, "%lld %lld\n", original_vocab_size, num_clusters);
	std::cout << "vocab size: " << original_vocab_size << " num clusters: " << num_clusters << std::endl;
	for (auto i = 0; i < original_vocab_size; i++)
	{
		fprintf (write_file_pointer, "%s ", words[i].c_str());

		long long c_id = assignments[i];
		if (assignments[i] > num_clusters) std::cout << "error: " << str << " " << c_id << std::endl;
		
		fprintf(write_file_pointer, "%lld\n", c_id);
		
		if ((i + 1) % 10000 == 0) std::cout << "progress: " << (i + 1) << " from " << original_vocab_size << std::endl;
	}
	
	fclose (write_file_pointer);
	
	std::cout << "writing finished" << std::endl;
	

	return 0;

}
