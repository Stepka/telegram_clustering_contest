/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "metric/modules/distance.hpp"


int main(int argc, char *argv[]) 
{
    // Create system default locale	
	#if defined(_WIN64)
		std::locale ru_locale("russian_russia.65001");
		std::locale::global(ru_locale);	
	#endif

	std::cout << "Cutting vocab have started" << std::endl;
	std::cout << std::endl;
	
	std::string string_for_read;
	float value;
	long long original_vocab_size, embedding_dimensions;
	
	std::vector<std::string> words;
	std::vector<float> embedding;
	std::vector<std::vector<float>> embeddings;	
	std::unordered_map<std::string, std::vector<float>> vocab;

	long long cut_vocab_size;
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
		cut_vocab_size = std::atoll(argv[2]);
		std::cout << "new vocab size: " << cut_vocab_size << std::endl;  
	}
	else
	{
		std::cout << "You haven't specified new vocab size, pleaes specify it" << std::endl;  
		return EXIT_FAILURE;
	}
	std::cout << std::endl;

	//
	
	std::size_t pos = original_file_name.find(".bin");  
	std::string cut_file_name = original_file_name.substr(0, pos) + "-" + std::to_string(cut_vocab_size) + "-words.bin";

	std::cout << "cutting started..." << std::endl;

	
	std::ifstream file_reader;
	std::ofstream file_writer;
	
	file_reader.open(original_file_name, std::ios::binary | std::ios::out);	
	file_writer.open(cut_file_name, std::ios::binary | std::ios::out);	
	
	file_reader >> original_vocab_size >> embedding_dimensions;	
	getline(file_reader, string_for_read);
	file_writer << cut_vocab_size << " " << embedding_dimensions << "\n";	
	std::cout << "original vocab size: " << original_vocab_size << " cut vocab size: " << cut_vocab_size << " embedding dimension: " << embedding_dimensions << std::endl;
	for (auto i = 0; i < cut_vocab_size; i++)
	{
		getline(file_reader, string_for_read, ' ');
		if(i < 100) std::cout << string_for_read << " ";
		file_writer << string_for_read << ' ';	

		for (auto j = 0; j < embedding_dimensions; j++)
		{
			file_reader.read(reinterpret_cast<char*>(&value), sizeof(float));
			//std::cout << value << " ";
			file_writer.write(reinterpret_cast<char*>(&value), sizeof(float));
		}

		if ((i + 1) % 10000 == 0) std::cout << "progress: " << (i + 1) << " from " << cut_vocab_size << std::endl;

	}
	
	file_reader.close();
	file_writer.close();

	std::cout << "cutting finished" << std::endl;
	std::cout << std::endl;


	// check first 100 words	
	
	std::cout << "checking started..." << std::endl;	
	
	std::ifstream file_again_reader;
	
	file_again_reader.open(cut_file_name, std::ios::binary | std::ios::out);	
	
	file_again_reader >> original_vocab_size >> embedding_dimensions;	
	getline(file_again_reader, string_for_read);
	std::cout << "vocab size: " << original_vocab_size << " embedding dimension: " << embedding_dimensions << std::endl;
	for (auto i = 0; i < original_vocab_size; i++)
	{
		getline(file_again_reader, string_for_read, ' ');
		if(i < 100) std::cout << string_for_read << " ";
		words.push_back(string_for_read);
		embedding.clear();

		for (auto j = 0; j < embedding_dimensions; j++)
		{
			file_again_reader.read(reinterpret_cast<char*>(&value), sizeof(float));
			//std::cout << value << " ";
			embedding.push_back(value);
		}
		embeddings.push_back(embedding);

		vocab[words[i]] = embedding;

		//if ((i + 1) % 10000 == 0) std::cout << "progress: " << (i + 1) << " from " << cut_vocab_size << std::endl;

	}
	
	file_again_reader.close();
	
	std::cout << std::endl;
	std::cout << "checking finished" << std::endl;

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	
	


	std::string input = "";
	while (input != "exit")
	{
		std::cout << "Type word:" << std::endl;
		if (input == "")
		{
			input = words[2];
		}
		else
		{
			std::getline(std::cin, input);
		}
		std::cout << std::endl;
		std::cout << "Entered word: " << input << '\n';

		//std::vector<std::string>::iterator it = std::find(words.begin(), words.end(), input);

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
		std::cout << std::endl;
	}
	

	return 0;
}