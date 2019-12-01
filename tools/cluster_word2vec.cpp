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
    // Create system default locale	
	#if defined(_WIN64)
		std::locale ru_locale("russian_russia.65001");
		std::locale::global(ru_locale);	
	#endif

	std::cout << "Clustering have started" << std::endl;
	std::cout << std::endl;
	
	std::vector<std::string> words;
	std::vector<float> embedding;
	std::vector<std::vector<float>> embeddings;	
	std::unordered_map<std::string, std::vector<float>> vocab;
	
	std::string string_for_read;
	float value;
	long long original_vocab_size, embedding_dimensions;

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
		
	std::ifstream file_reader;
	
	file_reader.open(original_file_name, std::ios::binary | std::ios::out);	
	
	file_reader >> original_vocab_size >> embedding_dimensions;	
	getline(file_reader, string_for_read);
	std::cout << "vocab size: " << original_vocab_size << " embedding dimensions: " << embedding_dimensions << std::endl;
	for (auto i = 0; i < original_vocab_size; i++)
	{
		getline(file_reader, string_for_read, ' ');
		words.push_back(string_for_read);
		embedding.clear();

		for (auto j = 0; j < embedding_dimensions; j++)
		{
			file_reader.read(reinterpret_cast<char*>(&value), sizeof(float));
			embedding.push_back(value);
		}
		embeddings.push_back(embedding);

		vocab[words[i]] = embedding;
		
		if ((i + 1) % 10000 == 0) std::cout << "progress: " << (i + 1) << " from " << original_vocab_size << std::endl;
	}
	file_reader.close();
	
	std::cout << "reading finished" << std::endl;
	std::cout << std::endl;


	// cluster
	
	std::cout << "clustering started..." << std::endl;

	auto t0 = std::chrono::steady_clock::now();
	auto t1 = std::chrono::steady_clock::now();

	auto[assignments, means, counts] = metric::kmeans(embeddings, num_clusters, 200, "cosine"); // clusters the data in num_clusters groups.


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

	std::cout << "clusters:" << std::endl;
	for (size_t i = 0; i < clusters.size(); i++)
	{
		std::cout << "cluster #" << i << ":" << std::endl;
		
		for (size_t j = 0; j < 20; j++)
		{
			if (j < clusters[i].size())
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
		if (clusters[i].size() > 20)
		{
			std::cout << "..." << std::endl;
		}
	}

	t1 = std::chrono::steady_clock::now();
	std::cout << "clustering finished (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()) / 1000000 << " s)" << std::endl;
	std::cout << std::endl;


	// write
	
	std::cout << "writing started..." << std::endl;	
	
	long long cluster_id;
	
	std::size_t pos = original_file_name.find(".bin");  
	std::string cluster_file_name = original_file_name.substr(0, pos) + "-" + std::to_string(num_clusters) + "-clusters.bin";
	
	std::ofstream file_writer;
	
	file_writer.open(cluster_file_name, std::ios::binary | std::ios::out);	
	
	file_writer << original_vocab_size << " " << num_clusters << "\n";	
	std::cout << "vocab size: " << original_vocab_size << " num clusters: " << num_clusters << std::endl;
	for (auto i = 0; i < original_vocab_size; i++)
	{
		file_writer << words[i] << ' ';	

		cluster_id = assignments[i];
		if (assignments[i] > num_clusters) std::cout << "error: " << words[i] << " " << cluster_id << std::endl;

		file_writer << cluster_id << '\n';	

		if ((i + 1) % 10000 == 0) std::cout << "progress: " << (i + 1) << " from " << original_vocab_size << std::endl;

	}
	
	file_writer.close();
	
	std::cout << "writing finished" << std::endl;
	

	return 0;

}
