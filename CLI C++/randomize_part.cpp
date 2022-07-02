#include <random>     //for mersenne()
#include <cmath>      //for pow()

std::string randomNumberGenerator(int &randNumberLimit, int &randNumberLenght)
{
	std::string randomNumber;
	std::random_device rd; 
	std::mt19937 mersenne(rd());
	std::uniform_int_distribution dist{0, randNumberLimit};
	
	randomNumber = std::to_string(dist(mersenne));

	int digitsInRandomNumber = randomNumber.size();

	if (digitsInRandomNumber < randNumberLenght)
	{
		randomNumber.insert(0,randNumberLenght - digitsInRandomNumber,'0');
	};

	return randomNumber;
}

std::string randomizeFileName(std::string fileName, int &randNumberLenght, int &randNumberLimit)
{
	if(isdigit(fileName[0]))
	{
		fileName = unrandomizeFileName(fileName);
	};

	fileName = randomNumberGenerator(randNumberLimit, randNumberLenght) + "_" + fileName;

	return fileName;
}