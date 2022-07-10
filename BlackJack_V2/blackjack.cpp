#include "stdafx.h"
#include "blackjack.h"
#include "card.h"

enum outcomes
{
	PLAYER_WINS,
	DEALER_WINS,
	PUSH,
	PLAYER_BLACKJACK,
	DEALER_BLACKJACK,
	SPLIT_DECIDED

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
	// create deck, players, and keep track of deck, stats, and bank. Main loop goes here.

	// Create deck and players.
	deck_t* deckPtr{ new deck_t(_makeDeck(decks)) };

	shuffleDeck(deckPtr);

	Dealer* dealerPtr{ new Dealer() };
	Player* playerPtr{ new Player() };

	bool play{ true };

	while(play && m_bank > 0)
	{
		do
		{
			int outcome = playBlackJack(playerPtr, dealerPtr, deckPtr, betsOn);
			handleOutcomes(outcome, betsOn);
			if (m_bank > 0)
				play = playAgain();
			else
				printf("\nYou went broke. Press enter to see results");
		} while (play && deckPtr->size() > 4 && m_bank > 0); // Bank should always be over 0 if bets are off.
		// If there are less than 4 cards in the deck, we are just going to create a new deck so we don't run out. probably will change this
		if(deckPtr->size() < 4)
		{
			delete deckPtr; // delete old deck
			deckPtr = new deck_t(_makeDeck(decks));
			printf("\nCreated new deck.\n");
			shuffleDeck(deckPtr); // shuffle deck and play more
		}
		
	}
	delete deckPtr; // if we stop playing free the memory.
	delete playerPtr;
	delete dealerPtr;
}
void BlackJack::handleOutcomes(int outcome, bool betsOn)
{
	switch (outcome)
	{
	case PLAYER_WINS:
		m_playerWins++;
		if (betsOn)
		{
			printf("\n\t%s wins $%i!\n", m_name.c_str(), m_bet);
			m_bank += m_bet;
		}
		else
			printf("\n\t%s wins!\n", m_name.c_str());

		break;
	case DEALER_WINS:
		m_dealerWins++;
		if (betsOn)
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
		if (betsOn)
			printf("\n\tDraw! Returned $%i.\n", m_bet);
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
	case SPLIT_DECIDED:
		break;
	}
}


int BlackJack::playBlackJack(Player* playerPtr, Dealer* dealerPtr, deck_t* deckPtr, bool betsOn)
{
	// Returns outcome as an int defined in outcomes enum.

	playerPtr->clearHand();
	clearDeck(playerPtr->split1);
	clearDeck(playerPtr->split2);
	playerPtr->split1Bet = 0;
	playerPtr->split2Bet = 0;

	dealerPtr->clearHand();

	if (betsOn)
	{
		printf("\nTotal Bank: $%i\n", getBank());
		bet();
	}

	playerPtr->setStartingHand(deckPtr);
	dealerPtr->setStartingHand(deckPtr);

	printHand(dealerPtr);
	printHand(playerPtr);
	printf("\tPlayer hand value: %i\n", playerPtr->getHandValue());
	if (betsOn)
		printBet();

	if (int winner = handleBlackjacks(playerPtr, dealerPtr, betsOn))
		// returns 0 if no blackjacks
		return winner;

	getPlayerInput(playerPtr, dealerPtr, playerPtr->getHand(), deckPtr, betsOn);
	if (!playerPtr->split1.empty())
	{
		// handle split win/lose and bets.

		return SPLIT_DECIDED;
	}
	if (checkBust(playerPtr->getHand()))
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

	return whoWon(playerPtr->getHand(), dealerPtr);
}

void printHand(HandInterface* hand)
{

	printf("\n\t%s hand:\t", hand->classString().c_str());
	hand->showGameHand();
}

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

void BlackJack::printBet() { printf("\nTotal Bet: $%i\n", m_bet); }

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
	return 0;
}

void BlackJack::getPlayerInput(Player* playerPtr, Dealer* dealerPtr ,deck_t& playerhand, deck_t* deckPtr, bool betsOn, bool split)
{
	printf("\nPlayers turn.    ");
	int choice{ 0 };
	do
	{
		printf("Enter 1 to stand    Enter 2 to hit    ");
		if (betsOn)
			printf("Enter 3 to double down    ");
		if (isSplittable(playerhand))
			printf("Enter 4 to split    ");

		choice = getInput<int>("Decision: ");

		switch (choice)
		{
		case STAND:
			printf("\n\tPlayer Stands.\n\n");
			return;
		case HIT:
			drawCard(deckPtr, &playerhand); // had to change this for split
			printf("\n\tPlayer hand: ");
			printDeck(&playerhand); // split change
			printf("\n\tPlayer hand value: %i\n", getHandValue(playerhand));
			if (checkBust(playerhand))
				return;
			if (getHandValue(playerhand) == BUST_NUMBER) // 21
				return; // why do you want to hit if you have 21.
			continue;
		case DOUBLE_DOWN:
			if (m_bank < m_bet * 2)
			{
				printf("\tYou're too poor to double down.\n");
				continue;
			}
			m_bet *= 2;
			drawCard(deckPtr, &playerhand);
			printf("\n\tPlayer hand: ");
			printDeck(&playerhand);
			printf("\n\tPlayer hand value: %i\n", getHandValue(playerhand));
			return;
		case SPLIT:
			if (!isSplittable(playerhand))
			{
				printf("\nYou weren't even given the option to split this hand.");
				continue;
			}
			if (m_bank < m_bet * 2) // forget money for now. Splitting is complicated enough.
			{
				printf("\nYou don't have enough money to place your bet on another hand.");
				continue;
			}

			if (playerPtr->getSplit1().empty())
			{
				playerPtr->split1.push_back(playerhand[1]); // set first card to second card of initialHand
				playerPtr->split1.push_back(getCardFromTop(deckPtr)); // set next card to top card from deck
			}
			else if(playerPtr->getSplit2().empty())
			{
				playerPtr->split2.push_back(playerPtr->split1[1]); // set first card to second card of initialHand
				playerPtr->split2.push_back(getCardFromTop(deckPtr)); // set next card to top card from deck
			} 
			else if (!playerPtr->getSplit2().empty())
			{
				printf("\nYou can only split twice.\n");
				continue;
			}


			
			

			playerhand[1] = getCardFromTop(deckPtr); // set second card from first hand to next card from deck

			printf("\n\tInitial Hand Split: ");
			printDeck(&playerhand);
			getPlayerInput(playerPtr,dealerPtr, playerhand, deckPtr, betsOn, true);
			if (!playerPtr->split1.empty() && playerPtr->split2.empty()) {
				printf("\n\tFirst Split: ");
				printDeck(&playerPtr->split1);
				getPlayerInput(playerPtr, dealerPtr, playerPtr->split1, deckPtr, betsOn, true);
			}
			if (!playerPtr->split1.empty() && !playerPtr->split2.empty()) {
				printf("\n\tSecond Split: ");
				printDeck(&playerPtr->split2);
				getPlayerInput(playerPtr, dealerPtr, playerPtr->split2, deckPtr, betsOn, true);
			}
			
			determineSplits(playerPtr, dealerPtr, deckPtr, betsOn);
			return;
		}
	}
	while (true);
}


int BlackJack::determineSplits(Player* playerPtr, Dealer* dealerPtr, deck_t* deckPtr, bool betsOn)
{
	int firstOutcome = whoWon(playerPtr->getHand(), dealerPtr);
	handleOutcomes(firstOutcome, betsOn);

	int secondOutcome = whoWon(playerPtr->split1, dealerPtr);
	handleOutcomes(secondOutcome, betsOn);

	if (!playerPtr->split1.empty() && !playerPtr->split2.empty())
	{
		int thirdOutcome = whoWon(playerPtr->split2, dealerPtr);
		handleOutcomes(thirdOutcome, betsOn);
	}
	return SPLIT_DECIDED;

}

int whoWon(deck_t& playerHand, Dealer* dealerPtr)
{
	if (getHandValue(playerHand )> dealerPtr->getHandValue())
		return PLAYER_WINS;
	if (dealerPtr->getHandValue() > getHandValue(playerHand))
		return DEALER_WINS;
	return PUSH;
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
