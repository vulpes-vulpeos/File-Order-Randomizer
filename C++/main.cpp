//Copyright 2021 Vulpes-Vulpeos

//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at

//    http://www.apache.org/licenses/LICENSE-2.0

//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

#include <iostream>
#include <filesystem> //for directory_iterator() and std::filesystem::exists()
#include <random>     //for mersenne()
#include <cstdio>     //for c_str()
#include <cctype>     //for isdigit()
#include <unistd.h>   //for getLogin()
#include <cmath>      //for pow()
#include <array>      //for std::array<char, 128> buffer

int main();
void mainMenu(std::string pathToFolder);
void finishMenu(std::string pathToFolder);
std::string pathInput();
std::string showDialog(int type, std::string dialog,std::string title,std::string button1,std::string button2);
void dialogResProc(std::string dialogResult, std::string pathToFolder);

//----------------------------------------------------------------------------------------------------------------------------------Path checking function
bool existChecker(std::string pathToFolder){
  bool exist = std::filesystem::exists(pathToFolder.c_str());
  return exist;
  }

//----------------------------------------------------------------------------------------------------------------------------------MacOS junk checking function
bool macOsJunkChecker(std::string pathToFile){
  bool junk = false;
  std::string macOsJunk[] = {".DS_Store", ".localized", "$RECYCLE.BIN", ".fseventsd", ".Spotlight-V100", "System Volume Information"};

  for (int i = 0; i < 6; ++i){
    if (pathToFile.find(macOsJunk[i]) != std::string::npos){junk = true; break;};
  };

  return junk;
  }

//----------------------------------------------------------------------------------------------------------------------------------Files counter function
int filesCounter(std::string pathToFolder){
  int numberOfFiles = 0;

  for (const auto &entry : std::filesystem::directory_iterator(pathToFolder)){
    std::string pathToFile = entry.path();
    if(!macOsJunkChecker(pathToFile)){++numberOfFiles;}
  };

  return numberOfFiles;
  }

//----------------------------------------------------------------------------------------------------------------------------------Check if number function
bool numberCheck(std::string fileName){
    bool check = isdigit(fileName[0]);
    return check;
  }

//----------------------------------------------------------------------------------------------------------------------------------Find separator function
int findSeparator(std::string fileName){
    std::string separators[] = {"_",". "};

    int index = fileName.find(separators[0]);
    if (index == std::string::npos){
      index = fileName.find(separators[1]);
        if (index != std::string::npos){index += 1;}
        else if (index == std::string::npos){index = 0;};
    };

    return index;
  }

//----------------------------------------------------------------------------------------------------------------------------------Numbers remove function
std::string numberRemove(std::string fileName, int index){
    fileName.erase(0,index+1);
    return fileName;
  }

//----------------------------------------------------------------------------------------------------------------------------------File rename function
void renameFile(std::string oldFileName, std::string newFileName){
  const char *char_oldName = oldFileName.c_str();
  const char *char_newName = newFileName.c_str();

  int result = rename(char_oldName, char_newName);
  if (result != 0){perror("Couldn't rename file (ʘдʘ╬)");}
  }

//----------------------------------------------------------------------------------------------------------------------------------Random number generator function
int randomNumberGen(int randomNumberMax){
  int randomNumber;
  std::random_device rd; 
  std::mt19937 mersenne(rd());
  std::uniform_int_distribution dist{0, randomNumberMax};
  
  randomNumber = dist(mersenne);

  return randomNumber;
}

//----------------------------------------------------------------------------------------------------------------------------------Random number processing function
std::string ranNumProcessing(int randomNumber, int numberLenght){
  int digitsInRandomNumber = std::to_string(randomNumber).size();
  std::string procRandomNumber;

  if (digitsInRandomNumber < numberLenght){
    std::string prefix = "0"; 
    for (int i = 1; i < numberLenght-digitsInRandomNumber; ++i){
      prefix += "0";
    };
    procRandomNumber = prefix + std::to_string(randomNumber);
  }
  else {procRandomNumber = std::to_string(randomNumber);};

  return procRandomNumber;
}

//----------------------------------------------------------------------------------------------------------------------------------Randomize function
void randomizeFunction(std::string pathToFolder){
  if (!existChecker(pathToFolder)) {
    std::string pressedButton = showDialog(2, "ERROR: Folder does not exist (╯°□°)╯┻━┻", "File order randomizer", "OK", "Not used");
    dialogResProc(pressedButton, "Not used");
  };

  int numberLenght = std::to_string(filesCounter(pathToFolder)).size();
  int randomNumberMax = pow(10, numberLenght)-1;

  for (const auto &entry : std::filesystem::directory_iterator(pathToFolder)){
    std::string pathToFile = entry.path();
    int index = pathToFile.find_last_of("/\\");
    std::string fileName = pathToFile.substr(index+1);

    if(!macOsJunkChecker(fileName)){
      if(numberCheck(fileName)){
        int index = findSeparator(fileName);
        if(index != 0){
          fileName = numberRemove(fileName, index);
        };
      };

    std::string newName = pathToFolder + "/" + ranNumProcessing(randomNumberGen(randomNumberMax), numberLenght) + "_" + static_cast<std::string>(fileName);
  
    renameFile(pathToFile, newName);
    };
  };
}

//----------------------------------------------------------------------------------------------------------------------------------Unrandomize function
void unrandomizeFunction(std::string pathToFolder){
  if (!existChecker(pathToFolder)) {
    std::string pressedButton = showDialog(2, "ERROR: Folder does not exist (╯°□°)╯┻━┻", "File order randomizer", "OK", "Not used");
    dialogResProc(pressedButton, "Not used");
  };

  for (const auto &entry : std::filesystem::directory_iterator(pathToFolder)){
    std::string pathToFile = entry.path();
    int index = pathToFile.find_last_of("/\\");
    std::string fileName = pathToFile.substr(index+1);

    if(numberCheck(fileName)){
      int index = findSeparator(fileName);
      if(index != 0){
        fileName = numberRemove(fileName, index);
      };

    std::string newName = pathToFolder + "/" + static_cast<std::string>(fileName);

    renameFile(pathToFile, newName);
    };
  };
}

//----------------------------------------------------------------------------------------------------------------------------------Show dialog via AppleScript function
std::string showDialog(int type, std::string dialog,std::string title,std::string button1,std::string button2){
  std::string pressedButton = "-128";
  std::array<char, 128> buffer;
  std::string command;

  switch (type){
    case 0: {command = "osascript -e 'set userFolder to path to home folder from user domain' -e 'set aliasWhichFolder to choose folder default location userFolder with prompt \""
                                   + dialog + "\"' -e 'set posixWhichFolder to POSIX path of aliasWhichFolder' -e 'return posixWhichFolder'";
             break;
    }
    case 1: {command = "osascript -e 'set pressedButton to display dialog \"" + dialog + "\" with title \"" + title + "\" buttons {\""
                                   + button1 + "\", \"" + button2 + "\", \"Exit\"} cancel button \"Exit\"' -e 'return pressedButton'";
             break;
    }
    case 2: {command = "osascript -e 'set pressedButton to display dialog \"" + dialog + "\" with title \"" + title + "\" buttons {\""
                                   + button1 + "\", \"Exit\"} cancel button \"Exit\"' -e 'return pressedButton'";
             break;
    }
  };

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {std::cerr << "Couldn't start command." << std::endl;}
  while (fgets(buffer.data(), 128, pipe) != NULL) {
  pressedButton = static_cast<std::string>(buffer.data());}

  pclose(pipe);
  return pressedButton;
}

//----------------------------------------------------------------------------------------------------------------------------------Dialog result processing function
void dialogResProc(std::string dialogResult, std::string pathToFolder){
  if (dialogResult.find("Randomize") != std::string::npos){randomizeFunction(pathToFolder);}
  else if (dialogResult.find("Unrandomize") != std::string::npos){unrandomizeFunction(pathToFolder);}
  else if (dialogResult.find("Yes") != std::string::npos){mainMenu(pathToFolder); finishMenu(pathToFolder);}
  else if (dialogResult.find("No") != std::string::npos){main();}
  else if (dialogResult.find("OK") != std::string::npos){main();}
  else if (dialogResult.find("-128") != std::string::npos){throw(-1);};
}

//----------------------------------------------------------------------------------------------------------------------------------Path input via AppleScript function
std::string pathInput(){
  std::string pathToFolder = showDialog(0, "Select folder:", "Not used", "Not used", "Not used");
  dialogResProc(pathToFolder, "Not used");
  
  pathToFolder.pop_back();
  pathToFolder.pop_back();
  return pathToFolder;
}

//----------------------------------------------------------------------------------------------------------------------------------Finish menu via AppleScript function
void finishMenu(std::string pathToFolder){
  std::string pressedButton = showDialog(1, "Continue with same folder?", "File order randomizer", "Yes", "No");
  dialogResProc(pressedButton, pathToFolder);
}

//----------------------------------------------------------------------------------------------------------------------------------Main menu via AppleScript function
void mainMenu(std::string pathToFolder){
  std::string pressedButton = showDialog(1, "Selected folder: " + pathToFolder, "File order randomizer", "Randomize", "Unrandomize");
  dialogResProc(pressedButton, pathToFolder);
}

int main(){

  std::string pathToFolder;
  try{
    pathToFolder = pathInput();
    mainMenu(pathToFolder);
    finishMenu(pathToFolder);
  }
  catch(int a){std::cout << "User pressed exit button" << std::endl; return 0;}

  return 0;
}
