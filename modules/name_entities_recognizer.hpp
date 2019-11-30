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
	class NER {

	public:
		
		NER() = default;

		NER(
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
		std::unordered_map<Language, TextEmbedder>& text_embedders_;
	};


	/**
	 * @class DatesExtractor
	 * 
	 * @brief DatesExtractor
	 */
	class DatesExtractor {

	public:
		
		using Vocab = std::unordered_map<std::string, int>;
		
		DatesExtractor() = default;

		DatesExtractor( 
			std::vector<Language>& languages, 
			std::unordered_map<news_clustering::Language, std::locale>& locales, 
			std::unordered_map<news_clustering::Language, std::string>& day_names_path, 
			std::unordered_map<news_clustering::Language, std::string>& month_names_path, 
			int now_year = 2019
		);

		/**
		 * @brief 
		 * @return 
		 */
		std::unordered_map<std::string, std::vector<std::vector<int>>> find_dates(
			std::unordered_map<std::string, news_clustering::Language>& file_names, 
			std::unordered_map<std::string, std::vector<std::string>>& contents
		);
		
		/**
		 * @brief 
		 * @return 
		 */
		std::vector<std::vector<int>> DatesExtractor::find_date(
			std::vector<std::string>& content,
			const Language& language
		);
			
		/**
		 * @brief 
		 * @return dd.mm.yyyy, f.e. 28.01.2019
		 */
		std::vector<int> check_if_date(std::string part_1, std::string part_2, std::string part_3, news_clustering::Language language);
		
		/**
		 * @brief 
		 * @return 
		 */
		int extract_year(const char *p);


	private:

		int now_year_ = 2019;
		
		ContentParser content_parser = news_clustering::ContentParser();
		std::unordered_map<news_clustering::Language, std::locale>& locales_;
		std::unordered_map<news_clustering::Language, Vocab> day_names_;
		std::unordered_map<news_clustering::Language, Vocab> month_names_;
	};


	/**
	 * @class TitleExtractor
	 * 
	 * @brief TitleExtractor
	 */
	class TitleExtractor {
		
	public:
		
		TitleExtractor() = default;

		TitleExtractor(std::unordered_map<Language, std::locale>& locales);

		/**
		 * @brief 
		 * @return 
		 */
		std::unordered_map<std::string, std::string> find_titles(
			std::unordered_map<std::string, news_clustering::Language> file_names
		);

	private:

		ContentParser content_parser = news_clustering::ContentParser();
		std::unordered_map<news_clustering::Language, std::locale>& locales_;
	};

}  // namespace news_clustering

#include "name_entities_recognizer.cpp"

#endif  // Header Guard
