/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2019 Stepan Mamontov (Panda Team)
*/


#if defined(__linux__)
	#include <dirent.h>
#endif

#if defined(_WIN64)
	#include <filesystem>
#endif

#include <vector>
#include <iostream>
#include <fstream>
#include "metric/modules/mapping.hpp"
#include "modules/json.hpp"


using json = nlohmann::json;
namespace  fs = std::filesystem;


const std::string LANGUAGES_MODE_COMMAND = "languages";
const std::string NEWS_MODE_COMMAND = "news";
const std::string CATEGORIES_MODE_COMMAND = "categories";
const std::string THREAD_MODE_COMMAND = "threads";
const std::string TOP_MODE_COMMAND = "top";

enum Mode { UNKNOWN_MODE, LANGUAGES_MODE, NEWS_MODE, CATEGORIES_MODE, THREAD_MODE, TOP_MODE };

////////////////////////////


std::vector<std::string> readAllPathsFromDir(std::string dirname)
{
	std::vector<std::string> path_names;

	#if defined(__linux__)
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dirname.c_str())) == NULL) {
			std::cout << "Error(" << errno << ") opening " << dirname << std::endl;
			return std::vector<std::vector<double>>();
		}

		while ((dirp = readdir(dp)) != NULL) {
			std::string fn = std::string(dirp->d_name);
			path_names.push_back(dirname + "/" + fn);
		}
		closedir(dp);
	#endif
	
	#if defined(_WIN64)
		for (const auto & entry : fs::directory_iterator(dirname))
		{
			std::string fn = entry.path().filename().string();
			
			path_names.push_back(entry.path().string());

			//std::fstream fin;

			//fin.open(filename, std::ios::in);
		}
	#endif

	return path_names;
}


std::vector<std::string> readFilePaths(std::string dirname, bool recursively = true)
{
	std::vector<std::string> files_paths;

	std::vector<std::string> all_paths = readAllPathsFromDir(dirname);

	for (auto fp : all_paths)
	{
		const fs::path path(fp);
		std::error_code ec; // For using the non-throwing overloads of functions below.
		if (fs::is_directory(path, ec))
		{ 
			std::vector<std::string> dir_files = readFilePaths(fp);
			files_paths.insert( files_paths.end(), dir_files.begin(), dir_files.end() );
		}
		if (ec)
		{
			std::cerr << "Error in is_directory: " << ec.message();
		}
		if (fs::is_regular_file(path, ec))
		{
			if (fp.size() > 5 && fp.substr(fp.size() - 5) == ".html")
			{
				files_paths.push_back(fp);
			}
		}
		if (ec) 
		{
			std::cerr << "Error in is_regular_file: " << ec.message();
		}
	}

	return files_paths;
}


////////////////////////////

int main(int argc, char *argv[]) 
{
	
	std::cout << "tgnews have started" << std::endl;  
	std::cout << std::endl;  
	
	/// Select working mode

	Mode mode = UNKNOWN_MODE;
	std::string data_path = "assets";

	if (argc > 1)
	{
		if (argv[1] == LANGUAGES_MODE_COMMAND)
		{
			mode = LANGUAGES_MODE;
		}
		else if (argv[1] == NEWS_MODE_COMMAND)
		{
			mode = NEWS_MODE;
		}
		else if (argv[1] == CATEGORIES_MODE_COMMAND)
		{
			mode = CATEGORIES_MODE;
		}
		else if (argv[1] == THREAD_MODE_COMMAND)
		{
			mode = THREAD_MODE;
		}
		else if (argv[1] == TOP_MODE_COMMAND)
		{
			mode = TOP_MODE;
		}
		else
		{
			std::cout << "Unknown command: " << argv[1] << std::endl; 
			return EXIT_FAILURE; 
		}
	}
	else
	{
		std::cout << "Unspecified mode: you should specify working mode. Possible modes are: 'languages', 'news', 'categories', 'threads', 'top'." << std::endl;  
		return EXIT_FAILURE;
	}

	if (argc > 2)
	{
		data_path = argv[2];
		std::cout << "Using data path: " << data_path << std::endl;  
	}
	else
	{
		std::cout << "You haven't specified data path, default path will be used instead: " << data_path << std::endl;  
	}

	std::cout << std::endl;  

	/// Load data

	auto file_names = readFilePaths(data_path);

	for (auto f : file_names)
	{
		std::cout << f << std::endl;  
	}
	std::cout << std::endl;  
		
	std::cout << file_names.size() << std::endl;  


	///

    return 0;
}
