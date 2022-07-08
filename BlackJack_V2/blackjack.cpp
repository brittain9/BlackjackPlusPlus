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
	dealerPtr->clearHand();

	if (betsOn)
	{
		printf("\nTotal Bank: $%i\n", m_bank);
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

	std::vector<std::vector<int[2]>> split = getPlayerInput(playerPtr->getHand(), deckPtr, betsOn);
	if (!split.empty())
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

std::vector<std::vector<int[2]>> BlackJack::getPlayerInput(deck_t& playerhand, deck_t* deckPtr, bool betsOn, std::vector<std::vector<int[2]>> didSplit)
{
	// empty vector if player did not split hand. Vector of outcomes and bet amounts if did... 
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
			return didSplit;
		case HIT:
			drawCard(deckPtr, &playerhand); // had to change this for split
			printf("\n\tPlayer hand: ");
			showGameHand(playerhand); // split change
			printf("\n\tPlayer hand value: %i\n", getHandValue(playerhand));
			if (checkBust(playerhand))
				return didSplit;
			if (getHandValue(playerhand) == BUST_NUMBER) // 21
				return didSplit; // why do you want to hit if you have 21.
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
			showGameHand(playerhand);
			printf("\n\tPlayer hand value: %i\n", getHandValue(playerhand));
			return didSplit;
		case SPLIT:
			if (!isSplittable(playerhand))
			{
				printf("\nYou weren't even given the option to split this hand.");
				continue;
			}
			//if(m_bet < m_bet * 2) // forget money for now. Splitting is complicated enough.
			//{
			//	printf("\nYou don't have enough money to place your bet on another hand.");
			//	continue;

			deck_t splitHand;
			splitHand.reserve(sizeof(Card) * 2);
			splitHand[0] = playerhand[1]; // set first card to second card of initialHand
			splitHand[1] = getCardFromTop(deckPtr); // set second card to next card from deck. Hit.

			// set up initialHand
			playerhand[1] = getCardFromTop(deckPtr);

			std::vector<int[2]> splitResults = Split(deckPtr, playerhand, betsOn, didSplit);
			if (!splitResults.empty()) // if function did not return an empty vector, push_back into vector
				didSplit.push_back(splitResults);
			getPlayerInput(splitHand, deckPtr, betsOn, didSplit); // My brain is dead, but the solution to splitting is recursion!

		}
	}
	while (true);
	
}
std::vector<int[2]> BlackJack::Split(deck_t* deckPtr, deck_t& initialHand, bool betsOn, std::vector<std::vector<int[2]>>& split)
{
	// return vector of array of 2 ints. 1 split = 2 outcomes. Max 2 splits = 3 outcomes. Read settings.h for rules

	std::vector<int[2]> splitResults{};
	// we've already checked if hand is splittable.
	//if (betsOn && m_bank < (m_bet * 2 + sizeof(split))) // sizeof(split) tracks iterations. If size = 0, we have not split yet.
	//{
	//	// can player afford split.
	//	printf("\tYou're too poor to split.\n");
	//	return splitResults;
	//}

	printDeck(&initialHand);
	printf("\tHand value: %i\n", getHandValue(initialHand));
	if (betsOn)
		printBet();

	_getSplitInput(initialHand, deckPtr, betsOn, split);
	
}

void BlackJack::_getSplitInput(deck_t& playerhand, deck_t* deckPtr, bool betsOn, std::vector<std::vector<int[2]>>& split)
{

	printf("Player Split #%i\t", split.size() + 1);
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
			drawCard(deckPtr, &playerhand); // add card to current hand
			printf("\n\tPlayer hand %i: ", split.size() + 1);
			showGameHand(playerhand); 
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
			m_bet *= 2; // has to multiply the bet on the current hand
			drawCard(deckPtr, &playerhand);
			printf("\n\tPlayer hand: ");
			showGameHand(playerhand);
			printf("\n\tPlayer hand value: %i\n", getHandValue(playerhand));
			return;
		case SPLIT:
			if (split.size() > 3)
			{
				printf("You can only split three times");
				continue;
			}
			if (!isSplittable(playerhand))
			{
				printf("\nYou weren't even given the option to split this hand.");
				continue;
			}
			//if(m_bet < m_bet * 2) // forget money for now. Splitting is complicated enough.
			//{
			//	printf("\nYou don't have enough money to place your bet on another hand.");
			//	continue;

			deck_t splitHand;
			splitHand.reserve(sizeof(Card) * 2);
			splitHand[0] = playerhand[1]; // set first card to second card of initialHand
			splitHand[1] = getCardFromTop(deckPtr); // set second card to next card from deck. Hit.

			// set up initialHand
			playerhand[1] = getCardFromTop(deckPtr);

			std::vector<int[2]> splitResults = Split(deckPtr, playerhand, betsOn, split);
			if (!splitResults.empty()) // if function did not return an empty vector, push_back into vector
				split.push_back(splitResults);
		}
	} while (true);
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
