#include <unistd.h>   //for getlogin()
#include <cstdio>     //for c_str()

struct FilePaths 
{
	std::string originalPath;
	std::string newPath;
};

bool macOsJunkChecker(std::string &fileName)
{
	bool junk = false;
	std::vector<std::string> macOsJunk = {".DS_Store", ".localized", "$RECYCLE.BIN",
	".fseventsd", ".Spotlight-V100", "System Volume Information"};

	for (int i = 0; i < macOsJunk.size(); ++i)
	{
		if (fileName.find(macOsJunk[i]) != std::string::npos)
		{
			junk = true;
			break;
		};
	};

	if (fileName[0] == '.' && fileName[1] == '_')
	{
		junk = true;
	};

	return junk;
}

void progressBar(const int &filesParsed, const int &numberOfFiles, std::string massage = "")
{
	int maxSymbolsInFiller = 40;
	int currentPercent = (filesParsed*100)/numberOfFiles;
	int filled = (currentPercent*maxSymbolsInFiller)/100;
	int empty = maxSymbolsInFiller-filled;
	std::string filler;

	for (int i = 0; i < filled; ++i)
	{
		filler += "#";
	};

	for (int i = 0; i < empty; ++i)
	{
		filler += " ";
	};

	std::cout << massage << "[" << filler << "] " << currentPercent << "% | " << filesParsed << " of " << numberOfFiles << "\r";
}

void renameFiles(std::vector<FilePaths> &renameQueue, std::string &massage)
{
	int filesParsed = 0, totalNumber = renameQueue.size();
	for (int i = 0; i < totalNumber; ++i){
		const char *char_oldName = renameQueue[i].originalPath.c_str();
		const char *char_newName = renameQueue[i].newPath.c_str();

		int result = rename(char_oldName, char_newName);
		if (result != 0)
		{
			perror("Couldn't rename file (ʘдʘ╬)");
		};

		++filesParsed;
		progressBar(filesParsed, totalNumber, massage);
	};

	std::cout << std::endl;
}

std::size_t numberOfFilesInDirectory(std::string &path)
{
	std::size_t numberOfFiles = std::distance(std::filesystem::directory_iterator(path), std::filesystem::directory_iterator{});
	return numberOfFiles;
}

void folderParser(std::string &pathToFolder, char &option, std::string launchPath = "")
{
	std::vector<FilePaths> renameQueue;
	std::vector<std::string> triggers;
	FilePaths currentFile;
	std::string pathToFile = "", newFilePath = "", fileName = "", newFileName = "", username = "", massage = "Renaming: ";
	int indexOfFileNameBeg = 0, filesParsed = 0, randNumberLenght, randNumberLimit;

	int numberOfFilesInFolder = numberOfFilesInDirectory(pathToFolder);
	
	switch (option)
	{
		case 'r' :
		{
			randNumberLenght = std::to_string(numberOfFilesInFolder).size();
			randNumberLimit = pow(10, randNumberLenght)-1;
			break;
		}
		case 'f' :
		{
			loadFilterlist(triggers, launchPath);
			username = getlogin();
			break;
		}

	};

	for (const auto &entry : std::filesystem::directory_iterator(pathToFolder))
	{
		pathToFile = entry.path();
		indexOfFileNameBeg = pathToFile.find_last_of("/")+1;
		fileName = pathToFile.substr(indexOfFileNameBeg);

		if (macOsJunkChecker(fileName))
		{
			++filesParsed;
			progressBar(filesParsed, numberOfFilesInFolder, "Parsing: ");
			continue;
		};

		switch (option)
		{
			case 'r':
			{
				newFileName = randomizeFileName(fileName, randNumberLenght, randNumberLimit);
				newFilePath = pathToFile.substr(0,indexOfFileNameBeg) + newFileName;
				break;
			}
			case 'u':
			{
				newFileName = unrandomizeFileName(fileName);
				newFilePath = pathToFile.substr(0,indexOfFileNameBeg) + newFileName;
				break;
			}
			case 'f':
			{
				if (filterByFileName(fileName, triggers))
				{	
					newFileName = fileName;		
					newFilePath = "/Users/" + username + "/.Trash/" + fileName;
				}
				else
				{
					//newFilePath = pathToFile;
					++filesParsed;
					progressBar(filesParsed, numberOfFilesInFolder, "Parsing: ");
					continue;
				};
				massage = "Filtering: ";
				break;
			}
			default:
			{
				throw WRONG_OPTION;
				break;
			}
		};

		if (fileName != newFileName || pathToFile != newFilePath)
		{
			currentFile.originalPath = pathToFile;
			currentFile.newPath = newFilePath;
			renameQueue.push_back(currentFile);
		};
		
		++filesParsed;
		progressBar(filesParsed, numberOfFilesInFolder, "Parsing: ");
	};

	std::cout << std::endl;
	if (!renameQueue.empty())
	{
		renameFiles(renameQueue, massage);
	}
	else
	{
		std::cout << "File names are all good." << std::endl;
	};
}