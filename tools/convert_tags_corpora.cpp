/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>


int main(int argc, char *argv[]) 
{
    // Create system default locale
    boost::locale::generator gen;
	
	#if defined(__linux__)
		std::locale ru_boost_locale = gen("ru_RU.UTF-8");
	#endif

	#if defined(_WIN64)
		std::locale ru_boost_locale = gen("russian_russia.65001");
		std::locale ru_locale("russian_russia.65001");
		std::locale::global(ru_locale);	
	#endif

	std::cout << "Cutting rus corpora have started" << std::endl;
	std::cout << std::endl;
	
	std::string string_for_read_1, string_for_read_2, string_for_read_3;
	std::stringstream string_for_read_stream;

	long long cut_vocab_size, lemma_id;

	std::string original_file_name;
	
	// tags converter from OpenCorpora tags to Universal POS tags
	std::unordered_map<std::string, std::string> tags;
	tags["NOUN"] = "NOUN";
	tags["VERB"] = "VERB";
	tags["ADJF"] = "ADJ";
	tags["ADVB"] = "ADV";
	tags["ADJS"] = "ADJ";
	tags["INFN"] = "VERB";
	tags["COMP"] = "ADV";
	tags["PRTF"] = "VERB";
	tags["PRTS"] = "VERB";
	tags["GRND"] = "VERB";
	tags["CONJ"] = "CCONJ";
	tags["INTJ"] = "INTJ";
	tags["PRCL"] = "PART";
	tags["PREP"] = "ADP";
	tags["PRED"] = "NOUN";
	tags["NUMR"] = "NUM";
	tags["NPRO"] = "PRON";

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
		std::cout << "You haven't specified new vocab size, all words will be used" << std::endl;  
		cut_vocab_size = 0;
	}
	std::cout << std::endl;

	//
	
	std::size_t pos = original_file_name.find(".txt");  
	std::string cut_file_name;
	if (cut_vocab_size > 0)
	{
		cut_file_name = original_file_name.substr(0, pos) + "-upos-tags-" + std::to_string(cut_vocab_size) + "-words.voc";
	}
	else
	{
		cut_file_name = original_file_name.substr(0, pos) + "-upos-tags.voc";
	}

	std::cout << "cutting started..." << std::endl;

	
	std::ifstream file_reader;
	std::ofstream file_writer;
	
	file_reader.open(original_file_name, std::ios::out);	
	file_writer.open(cut_file_name, std::ios::out);	
	
	int i = 0;
	while (file_reader >> lemma_id)
	{
		getline(file_reader, string_for_read_1);
		
		file_writer << lemma_id << "\n";

		do
		{
			getline(file_reader, string_for_read_1);
			if (string_for_read_1.size() != 0)
			{
				string_for_read_stream = std::stringstream(string_for_read_1);
				getline(string_for_read_stream, string_for_read_2, '\t');
				
				getline(string_for_read_stream, string_for_read_3, ' ');
				string_for_read_stream = std::stringstream(string_for_read_3);
				getline(string_for_read_stream, string_for_read_3, ',');
			
				//std::cout << "lemma_id: " << lemma_id << ": " << string_for_read_2 + "_" << string_for_read_3<< std::endl;
				file_writer << boost::locale::to_lower(string_for_read_2, ru_boost_locale) << " " << tags[string_for_read_3] << "\n";
			}
		} while (string_for_read_1.size() != 0);
		
		file_writer << "\n";

		if ((i + 1) % 10000 == 0) std::cout << "progress: " << (i + 1) << std::endl;
		i++;

		if (cut_vocab_size > 0 && i >= cut_vocab_size) break;
	}
	
	file_reader.close();
	file_writer.close();

	std::cout << "cutting finished" << std::endl;
	std::cout << std::endl;

	//
	
	std::cout << "tags conversions:" << std::endl;
	std::cout << std::endl;
	for (auto i = tags.begin(); i != tags.end(); i++)
	{
		std::cout << i->first << " -> " << i->second << std::endl;
	}
	std::cout << std::endl;


	// check first 20 words	
	
	std::cout << "checking started..." << std::endl;	
	std::cout << std::endl;
	
	std::ifstream file_again_reader;
	
	file_again_reader.open(cut_file_name, std::ios::out);	
	
	i = 0;
	while (file_again_reader >> lemma_id)
	{
		getline(file_again_reader, string_for_read_1);

		do
		{
			getline(file_again_reader, string_for_read_1);
			if (string_for_read_1.size() != 0)
			{
				string_for_read_stream = std::stringstream(string_for_read_1);
				getline(string_for_read_stream, string_for_read_2, ' ');
				
				getline(string_for_read_stream, string_for_read_3);
			
				std::cout << "lemma_id: " << lemma_id << ": " << string_for_read_2 + "_" << string_for_read_3<< std::endl;
			}
		} while (string_for_read_1.size() != 0);

		i++;
		if (i > 20) break;
	}
	
	file_again_reader.close();
	
	std::cout << std::endl;
	std::cout << "checking finished" << std::endl;
	

	return 0;
}