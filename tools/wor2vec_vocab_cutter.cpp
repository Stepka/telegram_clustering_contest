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

int main(void)
{
	//FILE *fp;
	//float f=12.23;	
	//float f1=43.78;	
	//long long vocab_size = 555, layer1_size = 100;

	//if ((fp=fopen("../../telegram_clustering_contest/data/embedding/GoogleNews-vectors-negative300.bin", "wb"))==NULL) {
	//	printf("Cannot open file.\n");
	//	exit (1);
	//}
 //   fprintf(fp, "%lld %lld\n", vocab_size, layer1_size);
	//fwrite(&f, sizeof(float), 1, fp);
 //   fprintf(fp, "\n");
	//fwrite(&f1, sizeof(float), 1, fp);
 //   fprintf(fp, "\n");
	//fclose (fp);
	
	//std::vector<std::string> words;
	//std::vector<float> embedding;
	//std::vector<std::vector<float>> embeddings;
	
	char str [80];
	FILE *read_file_pointer;
	FILE *write_file_pointer;
	FILE *read_again_file_pointer;
	float value;
	long long original_vocab_size, layer1_size;
	long cut_vocab_size = 10000;
	
	if ((read_file_pointer=fopen("../../telegram_clustering_contest/data/embedding/GoogleNews-vectors-negative300.bin", "rb"))==NULL) {
		printf("Cannot open file.\n");
		exit (1);
	}
	
	std::string cut_file_name = "GoogleNews-vectors-" + std::to_string(cut_vocab_size) + "-words.bin";

	if ((write_file_pointer=fopen(cut_file_name.c_str(), "wb"))==NULL) {
		printf("Cannot open file.\n");
		exit (1);
	}

	fscanf (read_file_pointer, "%lld %lld\n", &original_vocab_size, &layer1_size);	
	fprintf(write_file_pointer, "%lld %lld\n", cut_vocab_size, layer1_size);
	std::cout << original_vocab_size << " - " << layer1_size << std::endl;
	for (auto i = 0; i < cut_vocab_size; i++)
	{
		if (i % 10000 == 0) std::cout << i << std::endl;;
		fscanf (read_file_pointer, "%s ", str);
		fprintf (write_file_pointer, "%s ", std::string(str).c_str());
		//std::cout << str << " ";
		//words.push_back(std::string(str));
		//embedding.clear();
		for (auto j = 0; j < layer1_size; j++)
		{
			fread(&value, sizeof(float), 1, read_file_pointer);
			fwrite(&value, sizeof(float), 1, write_file_pointer);
			//embedding.push_back(value);
			//std::cout << value << " ";
		}
		fscanf (read_file_pointer, "\n");
		fprintf(write_file_pointer, "\n");
		//embeddings.push_back(embedding);

		//std::cout << std::endl;
	}
	
	fclose (read_file_pointer);
	fclose (write_file_pointer);

	std::cout << "finish" << std::endl;;


	//
	 
	//std::cout << std::endl;
	//std::cout << std::endl;
	//std::cout << std::endl;
	//
	//if ((write_file_pointer=fopen("GoogleNews-vectors-cut.bin", "wb"))==NULL) {
	//	printf("Cannot open file.\n");
	//	exit (1);
	//}

	//fprintf(write_file_pointer, "%lld %lld\n", cut_vocab_size, layer1_size);
	//std::cout << cut_vocab_size << " - " << layer1_size << std::endl;
	//for (auto i = 0; i < cut_vocab_size; i++)
	//{
	//	fprintf (write_file_pointer, "%s ", words[i].c_str());
	//	//std::cout << words[i].c_str() << " ";
	//	for (auto j = 0; j < layer1_size; j++)
	//	{
	//		fwrite(&embeddings[i][j], sizeof(float), 1, write_file_pointer);
	//		//std::cout << embeddings[i][j] << " ";
	//	}
	//	fprintf(write_file_pointer, "\n");

	//	if (i % 10000 == 0) std::cout << i << std::endl;;
	//	//std::cout << std::endl;
	//}
	//
	//fclose (write_file_pointer);

	//
	 
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	
	if ((read_again_file_pointer=fopen(cut_file_name.c_str(), "rb"))==NULL) {
		printf("Cannot open file.\n");
		exit (1);
	}

	fscanf (read_again_file_pointer, "%lld %lld\n", &original_vocab_size, &layer1_size);	
	std::cout << original_vocab_size << " - " << layer1_size << std::endl;
	for (auto i = 0; i < 100; i++)
	{
		if (i % 10000 == 0) std::cout << i << std::endl;;
		fscanf (read_again_file_pointer, "%s ", str);
		std::cout << str << " ";
		for (auto j = 0; j < layer1_size; j++)
		{
			fread(&value, sizeof(float), 1, read_again_file_pointer);
			//std::cout << value << " ";
		}
		fscanf (read_again_file_pointer, "\n");
		//std::cout << std::endl;
	}
	fclose (read_again_file_pointer);

	std::cout << std::endl;
	std::cout << "finish" << std::endl;
	

	return 0;
}