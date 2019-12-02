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
	) : languages_(languages), locales_(locales), text_embedders_(embedders)
	{
	};
	

	std::unordered_map<std::string, std::vector<std::string>> NER::find_name_entities(
		std::unordered_map<std::string, news_clustering::Language> file_names, 
		std::unordered_map<std::string, std::vector<std::string>> contents
	)
	{

		// TODO: check problem with "«" symbol. Looks like it is interpretated as special symbol and return caret by one symbol back
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
			embedder = text_embedders_[c->second];
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

			
			for (const auto& n : ngrams)
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

	//

	DatesExtractor::DatesExtractor(
		std::vector<Language>& languages, 
		std::unordered_map<Language, std::locale>& locales, 
		std::unordered_map<Language, std::string>& day_names_path, 
		std::unordered_map<Language, std::string>& month_names_path, 
		int now_year
	) : locales_(locales), now_year_(now_year)
	{
		for (auto i = 0; i < languages.size(); i++)
		{
			day_names_[languages[i]] = content_parser.read_vocabulary_and_tag(day_names_path[languages[i]], locales[languages[i]], 1, 31);
			month_names_[languages[i]] = content_parser.read_vocabulary_and_tag(month_names_path[languages[i]], locales[languages[i]], 1, 12);
		}
	};

	
	std::vector<std::vector<int>> DatesExtractor::find_date(
		std::vector<std::string>& content,
		const Language& language
	)
	{
		std::vector<std::vector<int>> dates;		
		std::vector<int> date;

		auto day_names = day_names_[language];
		auto month_names = month_names_[language];
		auto locale = locales_[language];

		for (auto i = 0; i < content.size(); i++)
		{
			if (month_names.find(boost::locale::to_lower(content[i], locale)) != month_names.end())
			{
				date.clear();

				if (i > 0 && i < content.size() - 1)
				{
					date = check_if_date(content[i - 1], content[i], content[i + 1], language);
					//std::cout << content[i - 1] << "." << content[i] << "." << content[i + 1] << std::endl;
				}
				else if (i == 0)
				{
					date = check_if_date("", content[i], content[i + 1], language);
					//std::cout << "." << content[i] << "." << content[i + 1] << std::endl;
				}
				else if (i == content.size() - 1)
				{
					date = check_if_date(content[i - 1], content[i], "", language);
					//std::cout << content[i - 1] << "." << content[i] << "." << std::endl;
				}

				if (date.size() == 3)
				{
					//std::cout << date[0] << "." << date[1] << "." << date[2] << std::endl;
					dates.push_back(date);
					i += 2;
				}
			}
		}

		return dates;
	}

	
	std::unordered_map<std::string, std::vector<std::vector<int>>> DatesExtractor::find_dates(
		std::unordered_map<std::string, news_clustering::Language>& file_names, 
		std::unordered_map<std::string, std::vector<std::string>>& contents
	)
	{
		std::unordered_map<std::string, std::vector<std::vector<int>>> result;
		
		std::vector<std::string> content;
		Language language;
		
		for (auto f = file_names.begin(); f != file_names.end(); f++)
		{
			content = contents[f->first];
			language = f->second;

			result[f->first] = find_date(content, language);
		}

		return result;
	};


	std::vector<int> DatesExtractor::check_if_date(std::string part_1, std::string part_2, std::string part_3, news_clustering::Language language)
	{
		auto day_names = day_names_[language];
		auto month_names = month_names_[language];
		auto locale = locales_[language];

		std::vector<std::vector<int>> valid_masks;
	
		switch (language.id())
		{
			case RUSSIAN_LANGUAGE:
				// D M Y   - 1 ﬂÌ‚ 2000
				valid_masks.push_back({ 0, 1, 2 });
				// Y M D   - 2000 Jan 1
				valid_masks.push_back({ 2, 1, 0 });
			
				// Y D M   - 2000 1 ﬂÌ‚
				valid_masks.push_back({ 2, 0, 1 });

				// D M X   - 1 ﬂÌ‚ X
				valid_masks.push_back({ 0, 1, 3 });
				// X D M   - X 1 ﬂÌ‚
				valid_masks.push_back({ 3, 0, 1 });
				break;
			
			case ENGLISH_LANGUAGE:
			default:
				// M D Y   - Jan 1 2000
				valid_masks.push_back({ 1, 0, 2 });
				// Y M D   - 2000 Jan 1
				valid_masks.push_back({ 2, 1, 0 });
				// D M Y   - 1 Jan 2000
				valid_masks.push_back({ 0, 1, 2 });
				// Y D M   - 2000 1 Jan
				valid_masks.push_back({ 2, 0, 1 });

				// M D X   - Jan 1 X
				valid_masks.push_back({ 1, 0, 3 });
				// X M D   - X Jan 1
				valid_masks.push_back({ 3, 1, 0 });
				// D M X   - 1 Jan X
				valid_masks.push_back({ 0, 1, 3 });
				// X D M   - X 1 Jan
				valid_masks.push_back({ 3, 0, 1 });
				break;
		}

		// Positions for the mask: is a day, is a month, is a year, doesn't matter
		std::vector<int> part_1_mask = { -1, -1, -1, 0 };
		std::vector<int> part_2_mask = { -1, -1, -1, 0 };
		std::vector<int> part_3_mask = { -1, -1, -1, 0 };
	
		std::unordered_map<std::string, int>::const_iterator what_found;

		what_found = day_names.find(boost::locale::to_lower(part_1, locale));
		if (what_found != day_names.end())
		{
			part_1_mask[0] = what_found->second;
		}
		what_found = day_names.find(boost::locale::to_lower(part_2, locale));
		if (what_found != day_names.end())
		{
			part_2_mask[0] = what_found->second;
		}
		what_found = day_names.find(boost::locale::to_lower(part_3, locale));
		if (what_found != day_names.end())
		{
			part_3_mask[0] = what_found->second;
		}

		//

		what_found = month_names.find(boost::locale::to_lower(part_1, locale));
		if (what_found != month_names.end())
		{
			part_1_mask[1] = what_found->second;
		}
		what_found = month_names.find(boost::locale::to_lower(part_2, locale));
		if (what_found != month_names.end())
		{
			part_2_mask[1] = what_found->second;
		}
		what_found = month_names.find(boost::locale::to_lower(part_3, locale));
		if (what_found != month_names.end())
		{
			part_3_mask[1] = what_found->second;
		}

		//

		auto part_1_i = extract_year(part_1.c_str());
		if (part_1_i >= 0 && part_1_i < 100)
		{
			part_1_i += part_1_i + ((int)now_year_ / 100) * 100;
		}		
		if(part_1_i >= now_year_ - 1 && part_1_i <= now_year_ + 1)
		{
			part_1_mask[2] = part_1_i;
		}

		auto part_2_i = extract_year(part_2.c_str());
		if (part_2_i >= 0 && part_2_i < 100)
		{
			part_2_i += part_2_i + ((int)now_year_ / 100) * 100;
		}		
		if(part_2_i >= now_year_ - 1 && part_2_i <= now_year_ + 1)
		{
			part_2_mask[2] = part_2_i;
		}

		auto part_3_i = extract_year(part_3.c_str());
		if (part_3_i >= 0 && part_3_i < 100)
		{
			part_3_i += part_3_i + ((int)now_year_ / 100) * 100;
		}		
		if(part_3_i >= now_year_ - 1 && part_3_i <= now_year_ + 1)
		{
			part_3_mask[2] = part_3_i;
		}

		//
	
		for (auto i = 0; i < valid_masks.size(); i++)
		{
			if (part_1_mask[valid_masks[i][0]] >= 0 && part_2_mask[valid_masks[i][1]] >= 0 && part_3_mask[valid_masks[i][2]] >= 0)
			{
				int day = 0;
				int month = 0;
				int year = 0;

				switch (valid_masks[i][0])
				{
					case 0:
						// we expect day at first gram
						day = part_1_mask[valid_masks[i][0]];
						break;
					case 1:
						// we expect month at first gram
						month = part_1_mask[valid_masks[i][0]];
						break;
					case 2:
						// we expect year at first gram
						year = part_1_mask[valid_masks[i][0]];
						break;
				}
				switch (valid_masks[i][1])
				{
					case 0:
						// we expect day at second gram
						day = part_2_mask[valid_masks[i][1]];
						break;
					case 1:
						// we expect month at second gram
						month = part_2_mask[valid_masks[i][1]];
						break;
					case 2:
						// we expect year at second gram
						year = part_2_mask[valid_masks[i][1]];
						break;
				}
				switch (valid_masks[i][2])
				{
					case 0:
						// we expect day at third gram
						day = part_3_mask[valid_masks[i][2]];
						break;
					case 1:
						// we expect month at third gram
						month = part_3_mask[valid_masks[i][2]];
						break;
					case 2:
						// we expect year at third gram
						year = part_3_mask[valid_masks[i][2]];
						break;
				}
				return { day, month, year };
			}
		}

		return std::vector<int>();
	};

	
	int DatesExtractor::extract_year(const char *p) 
	{
		int x = 0;
		if (*p < '0' || *p > '9') {
			return -1;
		}
		while (*p >= '0' && *p <= '9') {
			x = (x*10) + (*p - '0');
			++p;
		}
		return x;
	};

	//

	TitleExtractor::TitleExtractor(std::unordered_map<Language, std::locale>& locales) : locales_(locales)
	{
	};
	

	std::unordered_map<std::string, std::string> TitleExtractor::find_titles(
		std::unordered_map<std::string, news_clustering::Language> file_names
	)
	{
		std::unordered_map<std::string, std::string> result;
		
		std::vector<std::string> content;
		std::locale locale;
		std::size_t found_start;
		std::size_t found_end;
		std::size_t found_size;

		std::string search_string = "<meta property=\"og:title\" content=\"";	

		int ngramms_found = 0;
		
		for (auto c = file_names.begin(); c != file_names.end(); c++)
		{
			locale = locales_[c->second];
			content = content_parser.parse_by_lines(c->first, locale);

			for (auto i = 0; i < content.size(); i++)
			{				
				found_start = content[i].find(search_string);
				found_end = content[i].find("\"/>", content[i].size() - 4);
				found_size = found_end - (found_start + search_string.size());
				if (found_start != std::string::npos)
				{
					result[c->first] = content[i].substr(found_start + search_string.size(), found_size);
					break;
				}
			}
		}

		return result;
	}

}  // namespace news_clustering
#endif
