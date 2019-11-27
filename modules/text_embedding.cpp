/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_TEXT_EMBEDDING_CPP
#define _NEWS_CLUSTERING_TEXT_EMBEDDING_CPP

#include <fstream>
#include "text_embedding.hpp"
#include "../metric/modules/distance.hpp"


namespace news_clustering {

	TextEmbedder::TextEmbedder(const std::string path, Lemmatizer lemmatizer, Language language) : language_(language), lemmatizer_(lemmatizer)
	{		
		std::string string_for_read;
		long long original_vocab_size, cluster_id;
		std::vector<float> embedding;	
	
		std::ifstream file_reader;
	
		file_reader.open(path, std::ios::binary | std::ios::out);	
	
		file_reader >> original_vocab_size >> num_clusters;	
		getline(file_reader, string_for_read);
		for (auto i = 0; i < original_vocab_size; i++)
		{
			getline(file_reader, string_for_read, ' ');
			file_reader >> cluster_id;	
			vocab_clusters[string_for_read] = cluster_id;

			getline(file_reader, string_for_read);
		}
		file_reader.close();
	}

	std::vector<int> TextEmbedder::operator()(const std::vector<std::string>& words, std::locale locale)
	{
		std::vector<int> result(num_clusters, 0);
		std::string word_lower;

		for (auto word : words)
		{
			word_lower = boost::locale::to_lower(word, locale);
			word_lower = lemmatizer_(word_lower);
			if (vocab_clusters.find(word_lower) != vocab_clusters.end())
			{
				result[vocab_clusters[word_lower]]++;
			}
		}

		return result;
	}

	//

	Word2Vec::Word2Vec(const std::string path, Lemmatizer lemmatizer, Language language) : language_(language), lemmatizer_(lemmatizer)
	{		
		std::string string_for_read;
		float value;
		long long original_vocab_size, embedding_dimensions;
		std::vector<float> embedding;	
	
		std::ifstream file_reader;
	
		file_reader.open(path, std::ios::binary | std::ios::out);	
	
		file_reader >> original_vocab_size >> embedding_dimensions;	
		getline(file_reader, string_for_read);
		for (auto i = 0; i < original_vocab_size; i++)
		{
			getline(file_reader, string_for_read, ' ');
			embedding.clear();

			for (auto j = 0; j < embedding_dimensions; j++)
			{
				file_reader.read(reinterpret_cast<char*>(&value), sizeof(float));
				embedding.push_back(value);
			}
			vocab_embeddings[string_for_read] = embedding;
		}
		file_reader.close();
	}

	std::vector<float> Word2Vec::texts_distance(const std::vector<std::string>& long_text, const std::vector<std::vector<std::string>>& short_texts, std::locale locale)
	{
		std::vector<float> result;
		std::vector<float> distances;
		float mean_distance;
		float num_closest_distances = 5;
		float num_closest_distances_cut;
		
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
				single_word_lower = lemmatizer_(single_word_lower);
				if (vocab_embeddings.find(single_word_lower) != vocab_embeddings.end())
				{
					single_word_embedding = vocab_embeddings[single_word_lower];
					for (auto text_word : long_text)
					{
						text_word_lower = boost::locale::to_lower(text_word, locale);
						text_word_lower = lemmatizer_(text_word_lower);
						if (vocab_embeddings.find(text_word_lower) != vocab_embeddings.end())
						{
							text_word_embedding = vocab_embeddings[text_word_lower];
							distances.push_back(cosineDistance(single_word_embedding, text_word_embedding));
						}
					}

				}
			}

			std::sort(distances.begin(), distances.end(), std::greater<float>());
			mean_distance = 0;
			if (distances.size() < num_closest_distances)
			{
				num_closest_distances_cut = distances.size();
			}
			else
			{
				num_closest_distances_cut = num_closest_distances;
			}
			for (size_t i = 0; i < num_closest_distances_cut; i++)
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

	//

	Lemmatizer::Lemmatizer(const std::string path, Language language) : language_(language)
	{		
		std::string string_for_read, word, p_o_s, lemma = "";
		std::stringstream string_for_read_stream;

		long long lemma_id;	
	
		std::ifstream file_reader;
	
		file_reader.open(path, std::ios::out);	
		
		while (file_reader >> lemma_id)
		{
			getline(file_reader, string_for_read);

			do
			{
				getline(file_reader, string_for_read);
				if (string_for_read.size() != 0)
				{
					string_for_read_stream = std::stringstream(string_for_read);
					getline(string_for_read_stream, word, ' ');
					if (lemma == "")
					{
						lemma = word;
					}
				
					getline(string_for_read_stream, p_o_s);
			
					vocab[word] = lemma + "_" + p_o_s;
				}
				else
				{
					lemma = "";
				}
			} while (string_for_read.size() != 0);
		}

		file_reader.close();
	}

	std::string Lemmatizer::operator()(const std::string word)
	{
		if (vocab.find(word) != vocab.end())
		{
			return vocab[word];
		}

		return word;
	}

	//


	NER::NER(
			const std::vector<Language>& languages, 
			std::unordered_map<Language, std::locale>& locales
	) : languages_(languages), locales_(locales)
	{
	}
	

	std::unordered_map<std::string, std::vector<std::string>> NER::find_name_entities(
		std::unordered_map<std::string, news_clustering::Language> file_names, 
		std::unordered_map<std::string, std::vector<std::string>> contents
	)
	{

		// TODO: check problem with "�" symbol. Looks like it is interpretated as special symbol and return caret by one symbol back
		std::vector<std::string> content;
		std::vector<std::string> ngram;
		std::vector<std::vector<std::string>> ngrams;
		std::vector<std::string> name_entities;
		std::unordered_map<std::string, std::vector<std::string>> result;
		std::string entity;
		std::string lower;

		std::locale locale;

		bool ngramm_started = false;
		
		for (auto c = file_names.begin(); c != file_names.end(); c++)
		{
			content = contents[c->first];
			locale = locales_[c->second];
			for (auto i = 0; i < content.size(); i++)
			{
				lower = boost::locale::to_lower(content[i], locale);
				if (lower != content[i])
				{
					if (!ngramm_started)
					{
						ngramm_started = true;
					}
					ngram.push_back(lower);
				}
				else
				{
					if (ngram.size() > 1)
					{
						ngrams.push_back(ngram);
					}
					ngramm_started = false;
					ngram.clear();
				}
			}

			
			for (auto n : ngrams)
			{
				for (auto i = 0; i < n.size() - 1; i++)
				{
					entity = n[i] + "_";
				}
				entity += n[n.size() - 1];
				name_entities.push_back(entity);
			}
			
			result[c->first] = name_entities;
			name_entities.clear();
			ngrams.clear();
			ngram.clear();
			ngramm_started = false;
		}

		return result;
	};

}  // namespace news_clustering
#endif
