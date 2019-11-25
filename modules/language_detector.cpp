/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_LANGUAGE_DETECTOR_CPP
#define _NEWS_CLUSTERING_LANGUAGE_DETECTOR_CPP

#include "language_detector.hpp"


namespace news_clustering {
	

	LanguageDetector::LanguageDetector(const std::vector<Language>& languages, const std::vector<std::string>& vocab_paths, std::unordered_map<news_clustering::Language, std::locale>& locales, 
		size_t num_language_samples, double language_score_min_level) :
		languages_(languages), num_language_samples_(num_language_samples), language_score_min_level_(language_score_min_level)
	{
		for (auto i = 0; i < languages.size(); i++)
		{			
			Vocab vocab = content_parser.read_simple_vocabulary(vocab_paths[i], locales[languages[i]]);
			
			vocabs.push_back(vocab);
		}
	}

	double LanguageDetector::count_vocab_frequency(std::vector<std::string> content, std::vector<size_t> sampling_indexes, std::vector<std::string> vocab)
	{
		int score = 0;

		for (auto i = 0; i < sampling_indexes.size(); i++)
		{
			std::string sample = content[sampling_indexes[i]];
			for (auto word : vocab)
			{
				if (sample == word)
				{
					score++;
				}
			}
		}

		return (double) score / sampling_indexes.size();
	}

	Language LanguageDetector::detect_language_by_content(std::vector<std::string> content)
	{		
		// Random sampleing 
		std::vector<size_t> randomized_samples(content.size());
		std::iota(randomized_samples.begin(), randomized_samples.end(), 0);
		// shuffle samples after all was processed		
		std::shuffle(randomized_samples.begin(), randomized_samples.end(), std::mt19937 { std::random_device {}() });

		if (num_language_samples_ < randomized_samples.size())
		{
			randomized_samples.resize(num_language_samples_);
		}	  
	
		std::vector<double> scores(languages_.size());
		
		for (auto i = 0; i < languages_.size(); i++)
		{
			scores[i] = count_vocab_frequency(content, randomized_samples, vocabs[i]);

			//for (auto word : content)
			//{
			//	std::cout << word << " ";  
			//}
			//std::cout << std::endl;   
			//std::cout << "Num words: " << content.size() << std::endl;  
			//std::cout << std::endl; 
			//std::cout << "Samples size: " << randomized_samples.size() << std::endl;  
			//std::cout << std::endl;
			//std::cout << "English score: " << scores[0] << std::endl;  
			//std::cout << std::endl;  
			//std::cout << "Russian score: " << scores[1] << std::endl;   
			//std::cout << std::endl;  

			auto max_score_iterator = std::max_element(scores.begin(), scores.end());
			auto max_score_index = std::distance(scores.begin(), max_score_iterator);

			if (scores[max_score_index] > language_score_min_level_)
			{
				return languages_[max_score_index];
			}
		}

		return UNKNOWN_LANGUAGE;
	}

	
	std::unordered_map<std::string, Language> LanguageDetector::detect_language_by_file_names(std::vector<std::string> file_names)
	{
		std::unordered_map<std::string, Language> result;
		std::vector<std::string> content;

		for (auto i = 0; i < file_names.size(); i++)
		{
			// use default locale for noww, until we do not know text's language
			content = content_parser.parse(file_names[i], std::locale(), ' ', 2);			
			result[file_names[i]] = detect_language_by_content(content);
		}

		return result;
	}

}  // namespace news_clustering
#endif
