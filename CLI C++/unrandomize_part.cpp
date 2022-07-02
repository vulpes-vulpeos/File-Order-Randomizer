#include <cctype>     //for isdigit()

int findSeparator(std::string &fileName)
{
	int index = 0;
	std::vector<std::string> separators = {"_",". "};

	for (int i = 0; i < separators.size(); ++i)
	{
		index = fileName.find(separators[i]);
		if (index != std::string::npos)
		{
			index += separators[i].size();
			break;
		}
		else
		{
			index = 0;
		};
	};

	return index;
}

std::string unrandomizeFileName(std::string fileName)
{
	if(isdigit(fileName[0]))
	{
		int index = findSeparator(fileName);
		if(index != 0)
		{
			fileName.erase(0,index);
		};
	};

	return fileName;
}