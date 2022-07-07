#include "stdafx.h"
#include "blackjack.h"
#include "card.h"

enum outcomes
{
	PLAYER_WINS,
	DEALER_WINS,
	PUSH,
	PLAYER_BLACKJACK,
	DEALER_BLACKJACK

};

enum choices
{
	STAND = 1,
	HIT,
	DOUBLE_DOWN,
	SPLIT
};

void BlackJack::Blackjack(int decks, bool betsOn)
{
	// create deck, and keep track of deck. Main loop goes here.
	// if all cards in the deck are used. A new deck will be created and shuffled. Trying to simulate real life using a whole deck instead of creating a new shuffled deck each time.

	// Create deck and players.
	deck_t* deckPtr = allocateDeck(decks);

	shuffleDeck(deckPtr);

	Dealer* dealerPtr{ new Dealer() };
	Player* playerPtr{ new Player() };

	bool play{ true };

	while(play && m_bank > 0)
	{
		do
		{
			int outcome = playBlackJack(playerPtr, dealerPtr, deckPtr, betsOn);
			switch (outcome)
			{
			case PLAYER_WINS:
				m_playerWins++;
				if(betsOn)
				{
					printf("\n\t%s wins $%i!\n", m_name.c_str(), m_bet);
					m_bank += m_bet;
				}
				else
					printf("\n\t%s wins!\n", m_name.c_str());
				
				break;
			case DEALER_WINS:
				m_dealerWins++;
				if(betsOn)
				{
					printf("\n\tDealer wins! %s lost $%i!\n", m_name.c_str(), m_bet);
					m_bank -= m_bet;
				}
				else
				{
					printf("\n\tDealer wins!\n");
				}
				break;
			case PUSH:
				m_pushes++;
				if(betsOn)
					printf("\n\tDraw! Returned $%i.\n",m_bet);
				else
					printf("\n\tDraw!\n");
				break;
			case PLAYER_BLACKJACK:
				m_playerWins++;
				m_playerBlackjacks++;
				if (betsOn)
				{
					printf("\n\tBlackjack for %s! They win $%i!\n", m_name.c_str(), static_cast<int>(m_bet * BLACKJACK_PAYOUT_RATE));
					m_bank += static_cast<int>(m_bet * BLACKJACK_PAYOUT_RATE);
				}
				else
					printf("\nBlackjack for the player!\n");
				break;
			case DEALER_BLACKJACK:
				m_dealerWins++;
				m_dealerBlackjacks++;
				if (betsOn)
					m_bank -= m_bet;
				printf("\n\tBlackjack for the dealer!\n");
			}
			if (m_bank > 0)
				play = playAgain();
			else
				printf("You're broke.");
		} while (play && deckPtr->size() > 4 && m_bank > 0); // Bank should always be over 0 if bets are off.
		// If there are less than 4 cards in the deck, we are just going to create a new deck so we don't run out. probably will change this
		if(deckPtr->size() < 4)
		{
			delete deckPtr; // delete old deck
			deckPtr = allocateDeck(decks); // allocate new deck
			printf("\nCreated new deck.\n");
			shuffleDeck(deckPtr); // shuffle deck and play more
		}
		
	}
	delete deckPtr; // if we stop playing free the memory.
	delete playerPtr;
	delete dealerPtr;
}

int BlackJack::playBlackJack(Player* playerPtr, Dealer* dealerPtr, deck_t* deckPtr, bool betsOn)
{
	// Returns 0 if player wins , 1 if dealer wins, 2 if draw

	playerPtr->clearHand();
	dealerPtr->clearHand();

	
	if(betsOn)
	{
		printf("\nTotal Bank: $%i\n", m_bank);
		bet();
	}
		

	playerPtr->setStartingHand(deckPtr);
	dealerPtr->setStartingHand(deckPtr);

	printHand(dealerPtr);
	printHand(playerPtr);
	printf("\tPlayer hand value: %i\n", playerPtr->getHandValue());
	if(betsOn)
		printBet();

	if (int winner = handleBlackjacks(playerPtr, dealerPtr, betsOn))
		// returns 0 if no blackjacks
		return winner; 

	getPlayerInput(playerPtr, deckPtr, betsOn);
	if(playerPtr->checkBust())
	{
		printf("\nPlayer busted\n");
		return DEALER_WINS;
	}

	dealerPtr->AI(deckPtr);
	if (dealerPtr->checkBust())
	{
		printf("\nDealer busted.\n");
		return PLAYER_WINS;
	}

	return whoWon(playerPtr,dealerPtr);
}

void printHand(HandInterface* hand)
{

	printf("\n\t%s hand:\t", hand->classString().c_str());
	hand->showGameHand();
}
void BlackJack::printBet() { printf("\nTotal Bet: $%i\n", m_bet); }
void BlackJack::bet()
{
	while(true)
	{
		int bet = getInput<int>("Enter a nice bet amount. 1 for last bet size: ");
		if(bet > m_bank)
		{
			printf("\tYou're too poor to be betting that amount.\n");
			continue;
		}
		if (bet <= 0)
		{
			printf("\tYour bet must be greater than 0.\n");
			continue;
		}
		switch (bet)
		{
		case 1:
			if(m_lastBet == 0)
			{
				printf("\tYou don't have a last bet.\n");
				continue;
			}
			m_bet = m_lastBet;
			break;
		default:
			if(!(bet % 2 == 0 && bet % 5 == 0))
			{
				// not a pretty number
				printf("\tTry entering an even number divisible by 5.\n");
				continue;
			}
			m_bet = bet;
		}
		break;
	}
	m_lastBet = m_bet;
}

int BlackJack::handleBlackjacks(Player* playerPtr, Dealer* dealerPtr, bool betsOn)
{
	// Return 0 if no blackJacks. 1 if player, 2 if dealer, 3 if draw

	if (playerPtr->checkBlackJack())
	{
		if (dealerPtr->dealerFaceCardValue() == 1)
		{
			// If dealer showing ace
			if(betsOn)
				printf("\nSome special bet goes here.\n");
			if (dealerPtr->checkBlackJack())
			{
				printf("\nDealer also has blackjack.\n");
				return PUSH; // if draw?
				// add this feature later. Not important for simple game.
			}
		}
		else
		{
			return PLAYER_BLACKJACK;
		}
	}
	if (dealerPtr->checkBlackJack())
	{
		printf("\n\tBlackjack: ");
		dealerPtr->showFullHand();
		return DEALER_BLACKJACK;
	}
}

void BlackJack::getPlayerInput(Player* playerPtr, deck_t* deckPtr, bool betsOn)
{
	printf("\nPlayers turn.    ");
	int choice{ 0 };
	do
	{
		printf("Enter 1 to stand    Enter 2 to hit    ");
		if (betsOn)
			printf("Enter 3 to double down    ");
		if (playerPtr->isSplittable())
			printf("Enter 4 to split    ");

		choice = getInput<int>("Decision: ");

		switch (choice)
		{
		case STAND:
			printf("\tPlayer Stands.\n\n");
			return;
		case HIT:
			playerPtr->drawCard(deckPtr);
			playerPtr->showGameHand();
			printf("\n\tPlayer hand value: %i\n", playerPtr->getHandValue());
			if (playerPtr->checkBust())
				return;
			if (playerPtr->getHandValue() == BUST_NUMBER) // 21
				return; // why do you want to hit if you have 21.
			continue;

		case DOUBLE_DOWN:
			if (m_bank < m_bet * 2)
			{
				printf("\tYou're too poor to double down.\n");
				continue;
			}
			m_bet *= 2;
			playerPtr->drawCard(deckPtr);
			playerPtr->showGameHand();
			printf("\n\tPlayer hand value: %i\n", playerPtr->getHandValue());
			return;
		case SPLIT:
			if (!playerPtr->isSplittable())
			{
				printf("You weren't even given the option to split this hand.");
				continue;
			}
			printf("Not implemented yet.");
			continue;
		default:
			printf("\nNot a valid choice. Retry\n");
		}
	}
	while (true);
}

int whoWon(Player* playerPtr, Dealer* dealerPtr)
{
	if (playerPtr->getHandValue() > dealerPtr->getHandValue())
		// player wins
		return 0;
	if (dealerPtr->getHandValue() > playerPtr->getHandValue())
		//dealer wins
		return 1;
	// if draw
	return 2;
}

bool playAgain()
{
	return getInput<bool>("\nEnter 1 to play again. 0 for exit: ");
}

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
