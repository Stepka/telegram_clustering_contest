/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_LANGUAGE_DETECTOR_CPP
#define _NEWS_CLUSTERING_LANGUAGE_DETECTOR_CPP

#include <random>
#include <numeric>
#include "language_detector.hpp"


namespace news_clustering {
	

	LanguageDetector::LanguageDetector(const std::vector<Language>& languages, const std::vector<std::string>& vocab_paths, std::unordered_map<news_clustering::Language, std::locale>& locales) :
		languages_(languages)
	{
		for (auto i = 0; i < languages.size(); i++)
		{			
			Vocab vocab = content_parser.read_simple_vocabulary(vocab_paths[i], locales[languages[i]]);
			
			vocabs.push_back(vocab);
		}
	}

	
	std::unordered_map<Language, std::vector<std::string>> LanguageDetector::detect_language(
		std::unordered_map<std::string, std::vector<std::string>>& contents, 
		size_t num_language_samples, 
		double language_score_min_level
	)
	{
		std::unordered_map<Language, std::vector<std::string>> result;
		std::vector<std::string> content;

		Language language;
		
		for (auto c = contents.begin(); c != contents.end(); c++)
		{
			content = c->second;
			language = detect_language_by_single_content(content, num_language_samples, language_score_min_level);
			result[language].push_back(c->first);
		}

		return result;
	}


	Language LanguageDetector::detect_language_by_single_content(std::vector<std::string>& content, size_t num_language_samples, double language_score_min_level)
	{		
		// Random sampleing 
		std::vector<size_t> randomized_samples(content.size());
		std::iota(randomized_samples.begin(), randomized_samples.end(), 0);
		// shuffle samples after all was processed		
		std::shuffle(randomized_samples.begin(), randomized_samples.end(), std::mt19937 { std::random_device {}() });

		if (num_language_samples < randomized_samples.size())
		{
			randomized_samples.resize(num_language_samples);
		}	  
	
		std::vector<double> scores(languages_.size());
		
		for (auto i = 0; i < languages_.size(); i++)
		{
			scores[i] = count_vocab_frequency(content, randomized_samples, vocabs[i]);
		}

		auto max_score_iterator = std::max_element(scores.begin(), scores.end());
		auto max_score_index = std::distance(scores.begin(), max_score_iterator);
		
		if (scores[max_score_index] > language_score_min_level)
		{
			return languages_[max_score_index];
		}

		return UNKNOWN_LANGUAGE;
	}


	double LanguageDetector::count_vocab_frequency(std::vector<std::string> content, std::vector<size_t> sampling_indexes, std::unordered_map<std::string, std::string>& vocab)
	{
		int score = 0;

		for (auto i = 0; i < sampling_indexes.size(); i++)
		{
			std::string sample = content[sampling_indexes[i]];
			
			if (vocab.find(sample) != vocab.end()) 
			{
				score++;
			}
		}

		return (double) score / sampling_indexes.size();
	}

}  // namespace news_clustering
#endif
