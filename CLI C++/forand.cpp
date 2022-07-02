#include <iostream>
#include <filesystem> //for std::filesystem
#include <vector>     //for vector

enum Errors 
{
	WRONG_OPTION = 1,
	WRONG_PATH = 2,
	PATH_DO_NOT_EXIST = 3
};

#include "unrandomize_part.cpp"
#include "randomize_part.cpp"
#include "filter_part.cpp"
#include "folderParser_part.cpp"

void argumentsProcessing(std::vector<char> &optionsArray, std::vector<std::string> &pathsArray, std::string &launchPath)
{
	for (int pathNumber = 0; pathNumber < pathsArray.size(); ++pathNumber)
	{

		for (int optionNumber = 0; optionNumber < optionsArray.size(); ++optionNumber)
		{
			switch(optionsArray[optionNumber])
			{
				case 'r':
				{
					folderParser(pathsArray[pathNumber], optionsArray[optionNumber]);
					break;
				}
				case 'u':
				{
					folderParser(pathsArray[pathNumber], optionsArray[optionNumber]);
					break;
				}
				case 'f':
				{
					folderParser(pathsArray[pathNumber], optionsArray[optionNumber], launchPath);
					break;
				}
				default:
				{
					throw WRONG_OPTION;
					break;
				}
			};
		};
	};
}

void argumentsValidator(int argc, char const *argv[], std::vector<char> &optionsArray, std::vector<std::string> &pathsArray)
{
	if (argv[1][0] != '-')
	{
		throw WRONG_OPTION;
	};

	for (int optionNumber = 1; optionNumber < strlen(argv[1]); ++optionNumber)
	{
		switch(argv[1][optionNumber])
		{
			case 'r':
			case 'u':
			case 'f':
			{
				optionsArray.push_back(argv[1][optionNumber]);
				break;
			}
			default:
			{
				throw WRONG_OPTION;
				break;
			}
		};
	};

	for (int pathNumber = 2; pathNumber < argc; ++pathNumber)
	{
		switch(argv[pathNumber][0])
		{
			case '/':
			{
				if (!std::filesystem::exists(argv[pathNumber]) && !std::filesystem::is_directory(argv[pathNumber]))
				{
					throw PATH_DO_NOT_EXIST;
				}
				else
				{
					pathsArray.push_back(argv[pathNumber]);
				};
				break;
			}
			default:
			{
				throw WRONG_PATH;
				break;
			}
		};
	};
}

void printHelp(){
	std::cout
	<< "usage: for [-ru] [\"/path/to/folder\"] ...\n-r  randomize files in folder." << std::endl 
	<< "-u  unrandomize files in folder.\n-f  filter files in folder." << std::endl;
}

int main(int argc, char const *argv[])
{
	std::vector<char> optionsArray;
	std::vector<std::string> pathsArray;
	std::string launchPath = argv[0];

	switch(argc)
	{
		case 0:
		case 1:
		case 2:
		{
			printHelp();
			break;
		}
		default:
		{
			try
			{
				argumentsValidator(argc, argv, optionsArray, pathsArray);
				argumentsProcessing(optionsArray, pathsArray, launchPath);	
			}
			catch(Errors errorNumber)
			{
				switch(errorNumber)
				{
					case WRONG_OPTION:
					{
						std::cout << "ERROR: Unrecognized option(s)." << std::endl;
						printHelp();
					}
					case WRONG_PATH:
					{
						std::cout << "ERROR: Unrecognized path(s)." << std::endl;
						printHelp();
					}
					case PATH_DO_NOT_EXIST:
					{
						std::cout << "ERROR: Path do not exist or leads to file." << std::endl;
						printHelp();
					}
				};
			}
			catch (const std::length_error& le) 
			{
      			std::cerr << "Length error: " << le.what() << '\n';
   			};
		}
	};

	return 0;
}