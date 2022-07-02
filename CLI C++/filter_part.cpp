#include <fstream>    //for ifstream

void loadFilterlist(std::vector<std::string> &triggers, std::string &launchPath)
{

	std::string pathToFilterFile, pathToPATH = "/private/etc/paths";

	//checking for forand_filterlist.txt file in app launch folder
	if (launchPath[0] == '/')
	{
		int indexOfFileNameBeg = launchPath.find_last_of("/");
		pathToFilterFile = launchPath.substr(0, indexOfFileNameBeg) + "/forand_filterlist.txt";

		if (!std::filesystem::exists(pathToFilterFile))
		{
			pathToFilterFile = "";
		};
	};

	//checking for forand_filterlist.txt in PATH locations
	if (pathToFilterFile.empty() && std::filesystem::exists(pathToPATH))
	{
		std::string line;
		std::ifstream file (pathToPATH);

		if (file.is_open())
		{
			while (getline(file, line))
			{

				if(std::filesystem::exists(line+"/forand_filterlist.txt"))
				{
					pathToFilterFile = line+"/forand_filterlist.txt";
					break;
				};
			};
		};

		file.close();
	};

	//Loading built-in filters list if forand_filterlist.txt not found
	if (pathToFilterFile.empty())
	{
		std::string builtInTriggers[18] = {"а", "А", "е", "Е", "ё", "Ё", "и", "И", "о", "О", "у", "У", "э", "Э", "ю", "Ю", "я", "Я"};
		for (std::string trigger : builtInTriggers)
		{
			triggers.push_back(trigger);
		};
		std::cout << "Loaded built-in filters." << std::endl;
	}
	//Loading filters list from forand_filterlist.txt
	else
	{
		std::string line;
		int i = 0;
		std::ifstream file (pathToFilterFile);

		if (file.is_open())
		{
			while (getline(file, line, ','))
			{
				triggers.push_back(line);
				++i;
			};
		};

		file.close();

		std::cout << "Loaded filters list from: " << pathToFilterFile << std::endl;
	};
}

bool filterByFileName(std::string &fileName, std::vector<std::string> &triggers)
{
	for (int i = 0; i < triggers.size(); ++i)
	{
		if (fileName.find(triggers[i]) != std::string::npos)
		{

			return true;
		};
	};

	return false;
}