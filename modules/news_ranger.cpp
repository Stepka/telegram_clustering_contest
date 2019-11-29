/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/
#ifndef _NEWS_CLUSTERING_NEWS_RANGER_CPP
#define _NEWS_CLUSTERING_NEWS_RANGER_CPP

#include "news_ranger.hpp"


namespace news_clustering {
	

	NewsRanger::NewsRanger(
		std::vector<Language>& languages, 
		std::unordered_map<Language, TextEmbedder>& embedders, 
		std::unordered_map<news_clustering::Language, std::locale>& locales, 
		std::vector<int>& today
	) : languages_(languages), locales_(locales), text_embedders_(embedders), today_(today)
	{
	}


	template <typename T>
	std::vector<size_t> NewsRanger::sort_indexes(const std::vector<T> &v) {

	  // initialize original index locations
	  std::vector<size_t> idx(v.size());
	  std::iota(idx.begin(), idx.end(), 0);

	  // sort indexes based on comparing values in v
	  sort(idx.begin(), idx.end(),
		   [&v](size_t i1, size_t i2) {return v[i1] > v[i2];});

	  return idx;
	}

	
	 std::vector<std::unordered_map<std::string, std::vector<std::string>>> NewsRanger::arrange(
		std::unordered_map<std::string, std::vector<std::string>>& clustered_articles, 
		std::unordered_map<std::string, std::vector<std::vector<int>>>& dates, 
		std::unordered_map<std::string, std::vector<std::string>>& name_entities
	)
	{
		std::vector<NewsThread> result;
		
		std::vector<std::string> content;
		std::vector<std::string> indexed_file_names;
		std::vector<float> threads_all_points;
		std::vector<float> threads_freq_points;
		std::vector<float> threads_fresh_points;
		NewsThread single_thread;
		
		std::vector<std::vector<int>> days;
		float date_distance;

		std::vector<float>::iterator max_it;
		float max_value;

		
		for (auto i = clustered_articles.begin(); i != clustered_articles.end(); i++) 
		{
			indexed_file_names.push_back(i->first);   
			threads_freq_points.push_back(i->second.size());

			date_distance = 0;
			days = dates[i->first];
			for (auto day : days)
			{
				// if year == 0 then it means current year
				if (day[2] > 0)
				{
					date_distance += abs(today_[0] - day[0]) + abs(today_[1] - day[1]) * 30 + abs(today_[2] - day[2]) * 365;
				}
				else
				{
					date_distance += abs(today_[0] - day[0]) + abs(today_[1] - day[1]) * 30;
				}
			}
			
			//std::cout << i->first << " " << date_distance << " " << (date_distance /= days.size()) << std::endl;
			date_distance /= days.size();
			threads_fresh_points.push_back(date_distance);
		}

		threads_all_points = std::vector<float>(threads_freq_points.size(), 0);

		max_it = std::max_element(threads_freq_points.begin(), threads_freq_points.end());
		max_value = threads_freq_points[std::distance(threads_freq_points.begin(), max_it)];
		for (auto i = 0; i < threads_freq_points.size(); i++)
		{
			threads_freq_points[i] = threads_freq_points[i] / max_value;
			threads_all_points[i] += threads_freq_points[i];
		}

		max_it = std::max_element(threads_fresh_points.begin(), threads_fresh_points.end());
		max_value = threads_fresh_points[std::distance(threads_fresh_points.begin(), max_it)];
		for (auto i = 0; i < threads_fresh_points.size(); i++)
		{
			// more date diff means less points
			threads_fresh_points[i] = 1 - threads_fresh_points[i] / max_value;
			threads_all_points[i] += threads_fresh_points[i];
		}

		auto sorted_indexes = sort_indexes(threads_all_points);
		for (auto j : sorted_indexes)
		{
			single_thread.clear();
			single_thread[indexed_file_names[j]] = clustered_articles[indexed_file_names[j]];
			result.push_back(single_thread);
			//std::cout << indexed_file_names[j] << " " << threads_all_points[j] << " " << threads_freq_points[j] << " " << threads_fresh_points[j] << std::endl;
		}

		return result;
	}

}  // namespace news_clustering
#endif
