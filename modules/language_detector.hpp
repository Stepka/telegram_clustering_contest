/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_LANGUAGE_DETECTOR_HPP
#define _NEWS_CLUSTERING_LANGUAGE_DETECTOR_HPP

#include "languages.hpp"
#include "content_parser.hpp"

namespace news_clustering {

	/**
	 * @class LanguageDetector
	 * 
	 * @brief LanguageDetector
	 */
	struct LanguageDetector {
		
		using Vocab = std::vector<std::string>;

		LanguageDetector(const std::vector<Language>& languages, const std::vector<std::string>& vocab_paths, std::unordered_map<news_clustering::Language, std::locale>& locales, 
			size_t num_language_samples = 300, double language_score_min_level = 0.1);

		/**
		 * @brief 
		 * @return 
		 */
		std::unordered_map<Language, std::vector<std::string>> detect_language(std::unordered_map<std::string, std::vector<std::string>>& contents);

		/**
		 * @brief 
		 * @return 
		 */
		Language detect_language_by_single_content(std::vector<std::string> content);
		
		/**
		 * @brief 
		 * @return 
		 */
		double count_vocab_frequency(std::vector<std::string> content, std::vector<size_t> sampling_indexes, std::vector<std::string> vocab);

	private:
		/// Language consts
		size_t num_language_samples_ = 300;
		double language_score_min_level_ = 0.1;

		ContentParser content_parser = news_clustering::ContentParser();
		std::vector<Language> languages_;
		std::vector<Vocab> vocabs;
	};

}  // namespace news_clustering

#include "language_detector.cpp"

#endif  // Header Guard
