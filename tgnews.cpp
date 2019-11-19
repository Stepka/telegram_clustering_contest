/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/

#include <vector>
#include <iostream>
#include "metric/modules/mapping.hpp"


int main() {
	
	std::cout << "tgnews have started" << std::endl;  
	std::cout << std::endl;  

	std::vector<std::vector<float>> data{
		   {0, 0, 0, 0, 0}, 
		   {1.74120000000000, 4.07812000000000, -0.0927036000000, 41.7888000000000, 41.7888000000000},
		   {7.75309000000000, 16.2466000000000, 3.03956000000000, 186.074000000000, 186.074000000000},
		   {2.85493000000000, 3.25380000000000, 2.50559000000000, 68.5184000000000, 68.5184000000000},
		   {5.81414000000000, 8.14015000000000, 3.22950000000000, 139.539000000000, 139.539000000000},
		   {2.57927000000000, 2.63399000000000, 2.46802000000000, 61.9026000000000, 61.9026000000000} };

	auto[assignments, means, counts] = metric::kmeans(data, 4);


	std::cout << "assignments:" << std::endl;
	for (size_t i = 0; i < assignments.size(); i++) 
	{
		if (i < assignments.size() - 1)
		{
			std::cout << assignments[i] << ", ";
		}
		else
		{
			std::cout << assignments[i] << std::endl;
		}
	}
	std::cout << '\n';

	std::cout << "means:" << std::endl;
	for (size_t i = 0; i < means.size(); i++)
	{
		for (size_t j = 0; j < means[i].size(); j++)
		{
			if (j < means[i].size() - 1)
			{
				std::cout << means[i][j] << ", ";
			}
			else
			{
				std::cout << means[i][j] << std::endl;
			}
		}
	}
	std::cout << '\n';

	std::cout << "counts:" << std::endl;
	for (size_t i = 0; i < counts.size(); i++)
	{
		if (i < counts.size() - 1)
		{
			std::cout << counts[i] << ", ";
		}
		else
		{
			std::cout << counts[i] << std::endl;
		}
	}
	std::cout << '\n' << std::endl;

    return 0;
}
