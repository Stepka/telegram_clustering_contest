/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/

#ifndef _NEWS_CLUSTERING_NER_HPP
#define _NEWS_CLUSTERING_NER_HPP

#include "languages.hpp"
#include "modules/text_embedding.hpp"

namespace news_clustering {


	/**
	 * @class NER
	 * 
	 * @brief NER
	 */
	struct NER {
		
		NER() = default;

		explicit NER(
			const std::vector<Language>& languages, 
			std::unordered_map<Language, TextEmbedder>& embedders, 
			std::unordered_map<Language, std::locale>& locales
		);

		/**
		 * @brief 
		 * @return 
		 */
		std::unordered_map<std::string, std::vector<std::string>> find_name_entities(
			std::unordered_map<std::string, news_clustering::Language> file_names, 
			std::unordered_map<std::string, std::vector<std::string>> content
		);

		
		std::vector<Language> languages_;
		std::unordered_map<Language, std::locale>& locales_;
		std::unordered_map<Language, TextEmbedder>& embedders_;
	};

}  // namespace news_clustering

#include "name_entities_recognizer.cpp"

#endif  // Header Guard
