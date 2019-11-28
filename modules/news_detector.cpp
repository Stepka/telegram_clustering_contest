/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_NEWS_DETECTOR_CPP
#define _NEWS_CLUSTERING_NEWS_DETECTOR_CPP

#include "news_detector.hpp"
#include <cctype> 


namespace news_clustering {
	

	NewsDetector::NewsDetector(
			std::vector<Language>& languages, 
			std::unordered_map<news_clustering::Language, std::locale>& locales,  
			DatesExtractor& dates_extractor
	) : languages_(languages), locales_(locales), dates_extractor_(dates_extractor)
	{
	}

	
	std::unordered_map<bool, std::vector<std::string>> NewsDetector::detect_news(
		std::unordered_map<std::string, news_clustering::Language>& file_names, 
		std::unordered_map<std::string, std::vector<std::string>>& contents, 
		std::unordered_map<std::string, std::vector<std::vector<int>>>& dates, 
		std::unordered_map<std::string, std::vector<std::string>>& name_entities
	)
	{
		std::unordered_map<bool, std::vector<std::string>> result;

		std::vector<std::string> content;
		std::vector<std::vector<int>> file_dates;

		for (auto i = file_names.begin(); i != file_names.end(); i++) 
		{ 		
			content = contents[i->first]; 
			file_dates = dates[i->first]; 
			
			result[file_dates.size() > 0].push_back(i->first);
		}

		return result;
	}

}  // namespace news_clustering
#endif
