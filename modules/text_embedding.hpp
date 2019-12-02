/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_TEXT_EMBEDDING_HPP
#define _NEWS_CLUSTERING_TEXT_EMBEDDING_HPP

namespace news_clustering {

	/**
	 * @class Lemmatizer
	 * 
	 * @brief Lemmatizer
	 */
	class Lemmatizer {
		
	public:
		
		using Vocab = std::unordered_map<std::string, std::string>;
		
		Lemmatizer() = default;

		explicit Lemmatizer(const std::string& path, const Language& language, const std::string& default_suffix = "");

		/**
		 * @brief 
		 * @return 
		 */
		std::string operator()(const std::string& word);
		
		
		std::string default_suffix_ = "";
		Language language_;		
		Vocab vocab;
	};

	/**
	 * @class TextEmbedder
	 * 
	 * @brief TextEmbedder
	 */
	class TextEmbedder {
		
	public:
		
		using VocabClusters = std::unordered_map<std::string, long long>;
		
		TextEmbedder() = default;

		explicit TextEmbedder(const std::string& path, const Lemmatizer& lemmatizer, const Language& language);

		/**
		 * @brief 
		 * @return 
		 */
		std::vector<int> operator()(const std::vector<std::string>& words, const std::locale& locale, bool increment = true);

		/**
		 * @brief 
		 * @return 
		 */
		bool is_exist_in_vocab(const std::string& word, const std::locale& locale);
		

		
		long long num_clusters;	

		Language language_;
		Lemmatizer lemmatizer_;

		VocabClusters vocab_clusters;
	};

	/**
	 * @class Word2Vec
	 * 
	 * @brief Word2Vec
	 */
	class Word2Vec {
		
	public:
		
		using VocabEmbeddings = std::unordered_map<std::string, std::vector<float>>;
		
		Word2Vec() = default;

		explicit Word2Vec(const std::string& path, const Lemmatizer& lemmatizer, const Language& language);

		/**
		 * @brief 
		 * @return 
		 */
		std::vector<float> texts_distance(const std::vector<std::string>& long_text, const std::vector<std::vector<std::string>>& short_texts, const std::locale& locale, float num_closest_distances = 5);
		
		

		Language language_;
		Lemmatizer lemmatizer_;
		
		VocabEmbeddings vocab_embeddings;
	};

}  // namespace news_clustering

#include "text_embedding.cpp"

#endif  // Header Guard
