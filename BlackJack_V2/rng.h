#pragma once
#include <random>
#include <set>
#include <memory>

// Same rng() and algorithms I made in my Console RPG Simulation Game. Refactored test algorithms into 1 function. Also put everything to header file.

unsigned int rng()
{
	// Top performing simple random number generator
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(1, 42949672957); // distribution in range MAX_UNSIGNED_INT
	return dist(rng);
}

void testRandomNumberGeneration(int testSize, int distributionSize, bool printNumbers = false)
{
	// First checks for duplicates in testSize, then distribtion of numbers. Optional printNumbers flag if you want to see numbers to test algorithm output.
	// how evenly distributed the numbers will be when I modulo the rng() return by a specific number, distributionSize.
	if (distributionSize > testSize)
	{
		printf("If test size is not larger than distribution size, distribution may not be accurate.");
		return;
	}

	if (distributionSize > 10000)
	{
		printf("Distribution size too large.");
		return;
	}

	// store result dupes outside scope to print later
	int dupes{ 0 };
	{
		printf("\nFirst checking for # of duplicates in test size: %i\n", testSize);
		std::unique_ptr<std::set<unsigned int>> intSetPtr = std::make_unique<std::set<unsigned int>>();
		// use set because each element is unique. Dynamically allocate so I can use more memory.
		for (int i{ 0 }; i < testSize; i++)
		{
			unsigned int a = rng();
			intSetPtr->insert(a);

			if (printNumbers)
				//if set to false decrease time by 1/4!
			{
				printf("%u\t", a);
				if (i % 5 == 0)
					printf("\n");
			}
		}

		printf("\n\tSize of set: %i\t\tDuplicates: %i\n", intSetPtr->size(), testSize - intSetPtr->size());
		dupes = testSize - intSetPtr->size();
	}

	printf("\nNow testing distribution of numbers.\n");
	std::unique_ptr<std::vector<unsigned int>> intVecPtr = std::make_unique<std::vector<unsigned int>>();
	for (int i{ 0 }; i < testSize; i++)
	{
		// add testSize numbers in range [0, NumbersToTestPercentOf - 1] to intVec
		unsigned int a = rng() % distributionSize;
		intVecPtr->push_back(a);

		if (printNumbers)
		{
			printf("%u\t", a);
			if (i % 5 == 0)
				printf("\n");
		}
	}
	// After the numbers are in the vector, we need to create another vector of size NumbersToTestPercentOf with the elements and 0 being the number of elements found
	std::vector<std::array<unsigned int, 2>> distribVec;

	for (int i{ 0 }; i < distributionSize; i++)
	{
		// add range [0 - NumbersToTestPercentOf - 1] to vector
		std::array<unsigned int, 2> arr1;
		arr1[0] = i;
		arr1[1] = 0;
		distribVec.push_back(arr1);
	}

	// loop through 
	for (int i{ 0 }; i < intVecPtr->size(); i++)
	{
		if (intVecPtr->at(i) == distribVec[intVecPtr->at(i)][0])
		{
			distribVec[intVecPtr->at(i)][1]++;
		}
	}
	printf("\n\n");

	for (int i{ 0 }; i < distribVec.size(); i++)
	{
		printf("\tNumber:\t%u\t%.2f\t%%\n", distribVec[i][0], ((float)distribVec[i][1] / testSize) * 100.0);
	}
	printf("\n\tSize of set : % i\t\tDuplicates: % i\n", testSize, dupes);
}