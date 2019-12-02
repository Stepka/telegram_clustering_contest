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
#include "../metric/modules/distance.hpp"


namespace news_clustering {
	

	NewsDetector::NewsDetector(
			std::vector<Language>& languages, 
			std::unordered_map<news_clustering::Language, std::locale>& locales, 
			std::vector<int>& today
	) : languages_(languages), locales_(locales), today_(today)
	{
	}

	
	std::unordered_map<bool, std::vector<std::string>> NewsDetector::detect_news(
		std::unordered_map<std::string, news_clustering::Language>& file_names, 
		std::unordered_map<std::string, std::vector<std::string>>& contents, 
		std::unordered_map<std::string, std::vector<std::vector<int>>>& dates, 
		std::unordered_map<std::string, std::vector<std::string>>& name_entities, 
		int freshness_days
	)
	{
		std::unordered_map<bool, std::vector<std::string>> result;

		std::vector<std::string> content;
		std::vector<std::vector<int>> file_dates;
		std::vector<std::vector<double>> file_dates_for_entropy;
		
		float date_distance;
		
		for (auto i = file_names.begin(); i != file_names.end(); i++) 
		{ 		
			content = contents[i->first]; 
			file_dates = dates[i->first]; 
			
			//for (auto date : file_dates)
			//{
			//	file_dates_for_entropy.push_back({(double) date[0] + date[1] * 30.0 + date[2] * 365.0});
			//}
			//
			//auto e = metric::entropy(file_dates_for_entropy, 2, 2.0, metric::Euclidian<double>());
			//std::cout << "entropy: " << e << std::endl;
			

			date_distance = 0;
			for (auto date : file_dates)
			{
				// if year == 0 then it means current year
				if (date[2] > 0)
				{
					date_distance += abs(today_[0] - date[0]) + abs(today_[1] - date[1]) * 30 + abs(today_[2] - date[2]) * 365;
				}
				else
				{
					date_distance += abs(today_[0] - date[0]) + abs(today_[1] - date[1]) * 30;
				}
			}
			
			date_distance /= file_dates.size();

			result[file_dates.size() > 0 && date_distance < freshness_days].push_back(i->first);
		}

		return result;
	}

}  // namespace news_clustering
#endif
