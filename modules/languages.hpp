/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_LANGUAGES_HPP
#define _NEWS_CLUSTERING_LANGUAGES_HPP


namespace news_clustering {

	/**
	 * @enum Language
	 * 
	 * @brief Language
	 */
	enum LanguageId { UNKNOWN_LANGUAGE, ENGLISH_LANGUAGE, RUSSIAN_LANGUAGE };

	/**
	 * @class Language
	 * 
	 * @brief Language
	 */
	struct Language {
		
		Language() : language_id_(UNKNOWN_LANGUAGE)
		{
		};
		
		Language(LanguageId language_id) : language_id_(language_id)
		{
		};

		/**
		 * @brief 
		 * @return 
		 */
		std::string to_string() const
		{
			switch (language_id_)
			{
				case ENGLISH_LANGUAGE:
					return "en";

				case RUSSIAN_LANGUAGE:
					return "ru";

				default:
					return "unknown language"; 
			}
		};

		/**
		 * @brief 
		 * @return 
		 */
		LanguageId id() const
		{
			return language_id_;
		};

		
		bool operator==(const Language &other) const
		{
			return language_id_ == other.id();
		};

	private:
		LanguageId language_id_;
	};

}  // namespace news_clustering

namespace std {

	template <>
	struct hash<news_clustering::Language>
	{
		std::size_t operator()(const news_clustering::Language& k) const
		{
			return std::hash<std::string>()(k.to_string());
		}
	};

	template <>
	struct hash<const news_clustering::Language>
	{
		std::size_t operator()(const news_clustering::Language& k) const
		{
			return std::hash<std::string>()(k.to_string());
		}
	};
}


#endif  // Header Guard
