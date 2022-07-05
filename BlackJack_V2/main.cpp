#include "stdafx.h"
#include "timer.h"
#include "rng.h"

int main()
{
	Timer t;
	testRandomNumberGeneration(10, 2, false);
	std::cout << t.elapsed();
}