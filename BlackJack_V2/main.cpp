#include "stdafx.h"

#include "blackjack.h"

int main()
{
	std::string name{ "Alex" };
	//printf("Please enter your name: ");
	//std::cin >> name;

	BlackJack start(name);
	start.Blackjack(8, true);

	getchar();

}

