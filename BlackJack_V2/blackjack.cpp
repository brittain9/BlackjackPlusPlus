#include "stdafx.h"
#include "blackjack.h"
#include "card.h"

template<typename T>
T getInput(std::string message)
{
	while (true)
	{
		printf("%s", message.c_str());
		T returning;
		std::cin >> returning;
		if (std::cin.fail())
		{
			printf("\nFailed to get input.\n");
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return returning;
	}

}


int BlackJack(int decks)
{
	// Returns 0 if player wins , 1 if dealer wins, 2 if draw
	if (decks > 12)
	{
		printf("Why");
		return 2;
	}
	
	deckPtr_t deckPtr = allocateDeck(8); // Allocate really one 416 card deck or "eight" 52 card decks.
	shuffleDeck(deckPtr.get());

	dealerPtr_t dealerPtr = allocateDealer(deckPtr.get());
	playerPtr_T playerPtr = allocatePlayer(deckPtr.get());

	while (true)
	{
		std::cout << "\n\tDealer Hand:\t";
		dealerPtr->showGameHand();
		std::cout << "\n\tPlayer Hand:\t";
		playerPtr->showGameHand();
		std::cout << "\tPlayer hand value : " << playerPtr->getHandValue() << '\n';

		bool dealerBlackJack{ false };
		bool playerBlackJack{ false };
		dealerBlackJack = dealerPtr->checkBlackJack();
		playerBlackJack = playerPtr->checkBlackJack();

		if (playerBlackJack)
		{
			if (dealerPtr->dealerFaceCardValue() == 1)
			{
				// If dealer showing ace
				printf("\nSome special bet goes here.\n");
				if (dealerBlackJack)
					printf("\nDealer also has blackjack.\n");
				// add this feature later. Not important for simple game.
			}
			else
			{
				// Give player bet amount.
				printf("Player has BlackJack... WIP");
				// player won
				return 0;
			}
		}
		if (dealerBlackJack)
		{
			printf("Dealer blackjack");
			return 1;
		}


		while (true)
		{
			int choice{};
			choice = getInput<int>("\nPlayers turn.\n\nEnter 2 to hit\nEnter 1 to stand\nDecision: ");
			switch (choice)
			{
			case 1:
				printf("\nPlayer Stands.\n");
				break;
			case 2:
				playerPtr->drawCard(deckPtr.get());
				playerPtr->showGameHand();
				std::cout << "\n\tPlayer hand value : " << playerPtr->getHandValue() << '\n';
				playerPtr->checkBust();
				if (playerPtr->Busted())
				{
					printf("\nPlayer busted.\n");
					return 1;
				}
				continue;
			default:
				printf("\nNot a valid choice. Retry\n");
				continue;
			}
			break;
		}

		dealerPtr->AI(deckPtr.get());
		if (dealerPtr->Busted())
		{
			printf("\nDealer busted.\n"); \
				return 0;
		}
		if (dealerPtr->getHandValue() > playerPtr->getHandValue())
		{
			return 1;
		}
		if (dealerPtr->getHandValue() == playerPtr->getHandValue())
			return 2;
		break;
	}
	return 0;
}
