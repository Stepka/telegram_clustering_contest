/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_NER_CPP
#define _NEWS_CLUSTERING_NER_CPP

#include "name_entities_recognizer.hpp"


namespace news_clustering {
	

	NER::NER(
			const std::vector<Language>& languages, 
			std::unordered_map<Language, TextEmbedder>& embedders, 
			std::unordered_map<Language, std::locale>& locales
	) : languages_(languages), locales_(locales), embedders_(embedders)
	{
	}
	

	std::unordered_map<std::string, std::vector<std::string>> NER::find_name_entities(
		std::unordered_map<std::string, news_clustering::Language> file_names, 
		std::unordered_map<std::string, std::vector<std::string>> contents
	)
	{

		// TODO: check problem with "Ç" symbol. Looks like it is interpretated as special symbol and return caret by one symbol back
		std::vector<std::string> content;
		std::vector<std::string> ngram;
		std::vector<std::vector<std::string>> ngrams;
		std::vector<std::string> name_entities;
		std::unordered_map<std::string, std::vector<std::string>> result;
		std::string entity;
		std::string lower;

		std::locale locale;
		TextEmbedder embedder;

		bool ngramm_started = false;
		
		for (auto c = file_names.begin(); c != file_names.end(); c++)
		{
			content = contents[c->first];
			locale = locales_[c->second];
			embedder = embedders_[c->second];
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
					switch (c->second.id())
					{
						case RUSSIAN_LANGUAGE:
							entity = n[i] + "::";
							break;

						case ENGLISH_LANGUAGE:
						default:
							entity = n[i] + "_";
							break;
					}
				}
				entity += n[n.size() - 1];

				// check if found ngram exist in the vocab
				if (embedder.is_exist_in_vocab(entity, locale))
				{
					name_entities.push_back(entity);
				}
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
