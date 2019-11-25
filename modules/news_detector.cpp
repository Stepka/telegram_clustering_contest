/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_NEWS_DETECTOR_CPP
#define _NEWS_CLUSTERING_NEWS_DETECTOR_CPP

#include "news_detector.hpp"


namespace news_clustering {
	

	NewsDetector::NewsDetector() 
	{
	}

	
	std::unordered_map<std::string, bool> NewsDetector::detect_news(std::unordered_map<std::string, news_clustering::Language> file_names)
	{
		std::unordered_map<std::string, bool> result;

		for (auto i = file_names.begin(); i != file_names.end(); i++) { 
			/*std::cout << i->first << std::endl;  
		
			std::vector<std::string> content;
			std::vector<std::vector<int>> dates;
			switch (i->second.id())
			{
				case ENGLISH_LANGUAGE:
					content = content_parser.parse(i->first, en_boost_locale);   
					dates = findDates(content, english_day_names, english_month_names, en_boost_locale, i->second); 
					break;

				case RUSSIAN_LANGUAGE:
					content = content_parser.parse(i->first, ru_boost_locale); 
					dates = findDates(content, russian_day_names, russian_month_names, ru_boost_locale, i->second);
					break;

				default:
					break;
			}
			
			result[i->first] = dates.size() > 0;*/
			result[i->first] = true;
		}
	}
	

	std::vector<std::vector<int>> NewsDetector::findDates(std::vector<std::string> content, std::unordered_map<std::string, int> day_names, std::unordered_map<std::string, int> month_names, 
		std::locale locale, news_clustering::Language language)
	{
		std::vector<std::vector<int>> dates;

		for (auto i = 0; i < content.size(); i++)
		{ 
			if (month_names.find(boost::locale::to_lower(content[i], locale)) != month_names.end())
			{
				std::vector<int> date;
				if (i > 0 && i < content.size() - 1)
				{
					date = checkIfDate(content[i - 1], content[i], content[i + 1], day_names, month_names, locale, language );
				}
				else if(i == 0)
				{
					date = checkIfDate("", content[i], content[i + 1], day_names, month_names, locale, language );
				}
				else if(i == content.size() - 1)
				{
					date = checkIfDate(content[i - 1], content[i], "", day_names, month_names, locale, language );
				}

				if (date.size() == 3)
				{
					//std::cout << "valid date:    " << date[0] << "." << date[1] << "." << date[2] << std::endl;
					dates.push_back(date);
				}
			}
		}

		return dates;
	}

	
	int NewsDetector::extractYear(const char *p) {
		int x = 0;
		if (*p < '0' || *p > '9') {
			return -1;
		}
		while (*p >= '0' && *p <= '9') {
			x = (x*10) + (*p - '0');
			++p;
		}
		return x;
	}


	std::vector<int> NewsDetector::checkIfDate(std::string part_1, std::string part_2, std::string part_3, std::unordered_map<std::string, int> day_names, std::unordered_map<std::string, int> month_names, 
		std::locale locale, news_clustering::Language language)
	{
		int now_year = 2019;

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

		auto part_1_i = extractYear(part_1.c_str());
		if (part_1_i >= 0 && part_1_i < 100)
		{
			part_1_i += part_1_i + ((int)now_year / 100) * 100;
		}		
		if(part_1_i >= now_year - 1 && part_1_i <= now_year + 1)
		{
			part_1_mask[2] = part_1_i;
		}

		auto part_2_i = extractYear(part_2.c_str());
		if (part_2_i >= 0 && part_2_i < 100)
		{
			part_2_i += part_2_i + ((int)now_year / 100) * 100;
		}		
		if(part_2_i >= now_year - 1 && part_2_i <= now_year + 1)
		{
			part_2_mask[2] = part_2_i;
		}

		auto part_3_i = extractYear(part_3.c_str());
		if (part_3_i >= 0 && part_3_i < 100)
		{
			part_3_i += part_3_i + ((int)now_year / 100) * 100;
		}		
		if(part_3_i >= now_year - 1 && part_3_i <= now_year + 1)
		{
			part_3_mask[2] = part_3_i;
		}

		//
	
		for (auto i = 0; i < valid_masks.size(); i++)
		{
			//std::cout << "valid_mask " << i << ":    " << (part_1_mask[valid_masks[i][0]] && part_2_mask[valid_masks[i][1]] && part_3_mask[valid_masks[i][2]]) << std::endl;
			if (part_1_mask[valid_masks[i][0]] >= 0 && part_2_mask[valid_masks[i][1]] >= 0 && part_3_mask[valid_masks[i][2]] >= 0)
			{
				int day = 0;
				int month = 0;
				int year = 0;

				switch (valid_masks[i][0])
				{
					case 0:
						// we expext day at first gram
						day = part_1_mask[valid_masks[i][0]];
						break;
					case 1:
						// we expext month at first gram
						month = part_1_mask[valid_masks[i][0]];
						break;
					case 2:
						// we expext year at first gram
						year = part_1_mask[valid_masks[i][0]];
						break;
				}
				switch (valid_masks[i][1])
				{
					case 0:
						// we expext day at second gram
						day = part_2_mask[valid_masks[i][1]];
						break;
					case 1:
						// we expext month at second gram
						month = part_2_mask[valid_masks[i][1]];
						break;
					case 2:
						// we expext year at second gram
						year = part_2_mask[valid_masks[i][1]];
						break;
				}
				switch (valid_masks[i][2])
				{
					case 0:
						// we expext day at third gram
						day = part_3_mask[valid_masks[i][2]];
						break;
					case 1:
						// we expext month at third gram
						month = part_3_mask[valid_masks[i][2]];
						break;
					case 2:
						// we expext year at third gram
						year = part_3_mask[valid_masks[i][2]];
						break;
				}
				//std::cout << "valid date " << i << ":    " << day << "." << month << "." << year << std::endl;
				return { day, month, year };
			}
		}

		return std::vector<int>();
	}

}  // namespace news_clustering
#endif
