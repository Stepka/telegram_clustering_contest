#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>

//namespace patch
//{
//    template < typename T > std::string to_string( const T& n )
//    {
//        std::ostringstream stm ;
//        stm << n ;
//        return stm.str() ;
//    }
//}

int main(int argc, char *argv[]) 
{
	std::cout << "Cutting vocab have started" << std::endl;
	std::cout << std::endl;
	
	char str [80];
	FILE *read_file_pointer;
	FILE *write_file_pointer;
	FILE *read_again_file_pointer;
	float value;
	long long original_vocab_size, embedding_dimensions;

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
	
	if ((read_file_pointer=fopen(original_file_name.c_str(), "rb"))==NULL) {
		printf("Cannot open file.\n");
		exit (1);
	}

	if ((write_file_pointer=fopen(cut_file_name.c_str(), "wb"))==NULL) {
		printf("Cannot open file.\n");
		exit (1);
	}

	std::cout << "cutting started..." << std::endl;

	fscanf (read_file_pointer, "%lld %lld\n", &original_vocab_size, &embedding_dimensions);	
	fprintf(write_file_pointer, "%lld %lld\n", cut_vocab_size, embedding_dimensions);
	std::cout << "original vocab size: " << original_vocab_size << " cut vocab size: " << cut_vocab_size << " embedding dimension: " << embedding_dimensions << std::endl;
	for (auto i = 0; i < cut_vocab_size; i++)
	{
		fscanf (read_file_pointer, "%s ", str);
		fprintf (write_file_pointer, "%s ", std::string(str).c_str());
		for (auto j = 0; j < embedding_dimensions; j++)
		{
			fread(&value, sizeof(float), 1, read_file_pointer);
			fwrite(&value, sizeof(float), 1, write_file_pointer);
		}
		fscanf (read_file_pointer, "\n");
		fprintf(write_file_pointer, "\n");

		if ((i + 1) % 10000 == 0) std::cout << "progress: " << (i + 1) << " from " << cut_vocab_size << std::endl;
	}
	
	fclose (read_file_pointer);
	fclose (write_file_pointer);

	std::cout << "cutting finished" << std::endl;
	std::cout << std::endl;

	// check first 100 words	 
	
	std::cout << "checking started..." << std::endl;

	if ((read_again_file_pointer=fopen(cut_file_name.c_str(), "rb"))==NULL) {
		printf("Cannot open file.\n");
		exit (1);
	}

	fscanf (read_again_file_pointer, "%lld %lld\n", &original_vocab_size, &embedding_dimensions);	
	std::cout << "vocab size: " << original_vocab_size << " embedding dimension: " << embedding_dimensions << std::endl;
	for (auto i = 0; i < 100; i++)
	{
		fscanf (read_again_file_pointer, "%s ", str);
		std::cout << str << " ";
		for (auto j = 0; j < embedding_dimensions; j++)
		{
			fread(&value, sizeof(float), 1, read_again_file_pointer);
			//std::cout << value << " ";
		}
		fscanf (read_again_file_pointer, "\n");
		//std::cout << std::endl;
	}
	fclose (read_again_file_pointer);
	
	std::cout << std::endl;
	std::cout << "checking finished" << std::endl;
	

	return 0;
}