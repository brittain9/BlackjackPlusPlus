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

bool END_GAME{ false };

void BlackJack::Blackjack(int decks)
{
	// Main game loop. Create deck, dealer, players, and cleanup.

	deck_t* deckPtr{ new deck_t(_makeDeck(decks)) };

	shuffleDeck(deckPtr);
	shuffleDeck(deckPtr);
	shuffleDeck(deckPtr);
	shuffleDeck(deckPtr);

	Dealer* dealerPtr{ new Dealer() };
	Player* playerPtr{ new Player() };

	while (!END_GAME && m_bank > 0)
	{
		// Main game loop
		while (!END_GAME && m_bank > 0 && deckPtr->size() > MIN_CARDS_BEFORE_NEW)
		{
			// current deck loop
			int outcome = playBlackJack(playerPtr, dealerPtr, deckPtr);
			handleOutcomes(playerPtr, dealerPtr, outcome);

			if (m_bank > m_highestBank)
				m_highestBank = m_bank;

			if (m_bank < 0)
				printf("\nYou went broke.");

			if (!betsOn) {
				int again = getInput<int>("\nEnter 1 to play again: ");
				if (again != 1) {
					END_GAME = true;
				}
			}
		}

		// when deck gets too small. Loop breaks and creates new deck.
		if (deckPtr->size() < MIN_CARDS_BEFORE_NEW)
		{
			delete deckPtr; // delete old deck
			deckPtr = new deck_t(_makeDeck(decks));
			printf("\nCreated new deck.\n");
			// four shuffles for consistency
			shuffleDeck(deckPtr);
			shuffleDeck(deckPtr);
			shuffleDeck(deckPtr);
			shuffleDeck(deckPtr);

		}

		printf("\nPress enter to see game stats");
		// if we stop playing free the memory.
		delete deckPtr;
		delete playerPtr;
		delete dealerPtr;
	}
}
int BlackJack::playBlackJack(Player* playerPtr, Dealer* dealerPtr, deck_t* deckPtr)
{
	// Returns outcome as an int.

	// Clear player previous hands and bets.
	playerPtr->clearHand();
	clearDeck(playerPtr->split1);
	clearDeck(playerPtr->split2);
	playerPtr->split1Bet = 0;
	playerPtr->split2Bet = 0;
	m_insureBet = 0;

	dealerPtr->clearHand();

	if (betsOn)
	{
		printf("\nTotal Bank: $%i\n", getBank());
		bet();
	}
	if (END_GAME)
		return SPLIT_DECIDED; // in handleOutcomes this option just breaks

	playerPtr->setStartingHand(deckPtr);
	dealerPtr->setStartingHand(deckPtr);

	printHand(dealerPtr);
	printHand(playerPtr);
	printf("\tPlayer hand value: %i\n", playerPtr->getHandValue());

	if (betsOn && dealerPtr->getHand()[0].rankNumber() == 1)
	{
		// if dealer face card is ace
		insuranceBet();
	}

	if (betsOn)
		printBet();

	if (int winner = handleBlackjacks(playerPtr, dealerPtr))
		// returns 0 if no blackjacks
		return winner;

	getPlayerInput(playerPtr, dealerPtr, playerPtr->getHand(), deckPtr, false, false, false);
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

void BlackJack::bet()
{
	while(true)
	{
		int bet = getInput<int>("\nEnter bet or 0 for hotkeys: ");

		if(bet > m_bank)
		{
			printf("\nYou're too poor to be betting that amount.\n");
			continue;
		}
		if (bet < 0)
		{
			printf("\nYour bet must be positive.\n");
			continue;
		}

		switch (bet)
		{
		case 0: 
			printf("\nPress 1 for last bet size\nPress 2 for half last bet size\nPress 9 to end and show results\n");
			continue;
		case 1:
			if(m_lastBet == 0)
			{
				printf("\nYou don't have a last bet.\n");
				continue;
			}
			if (m_lastBet > m_bank) {
				printf("\nYou're too poor to bet your last bet.");
				continue;
					
			}
			m_bet = m_lastBet;
			break;
		case 2:
			if (m_lastBet == 0)
			{
				printf("\nYou don't have a last bet.\n");
				continue;
			}
			if (static_cast<int>(std::round(m_lastBet / 2) > m_bank)) {
				printf("\nYou're too poor to bet even half of your last bet.");
				continue;
			}
			m_bet = static_cast<int>(std::round(m_lastBet / 2));
			break;
		case 9:
			END_GAME = true;
			return;
		default:
			m_bet = bet;
			m_lastBet = m_bet;
		}
		return;
	}

}

void BlackJack::insuranceBet() 
{
	int insureBet{ 0 };
	while (true) 
	{
		int choice = getInput<int>("\nDealer is showing an ace. Would you like to place an insurance? 1 for yes, 0 for no: ");
		switch (choice) {
		case 1:
			insureBet = getInput<int>("\nPlease enter your insurance bet amount: ");
			if (m_bank < m_bet + insureBet)
			{
				printf("\nYou cannot afford this insurance bet.\n");
				continue;
			}
			if (insureBet > static_cast<int>(std::round(m_bet / 2))) {
				printf("\nThe maximum value for an insurance bet is half your original stake.\n");
				continue;
			}
			m_insureBet = insureBet;
			return;
		case 0:
			printf("\nNo insurance bet placed.");
			return;
		default:
			printf("\tInvalid Input.\n");
			continue;
		}
	}

}

int BlackJack::handleBlackjacks(Player* playerPtr, Dealer* dealerPtr)
{
	// Return 0 if no blackJacks. 1 if player, 2 if dealer, 3 if draw

	if (playerPtr->checkBlackJack())
	{
		if (dealerPtr->checkBlackJack())
		{
			printf("\nDealer also has blackjack.\n");
			return PUSH; // if draw?
		}
		return PLAYER_BLACKJACK;
	}

	if (dealerPtr->checkBlackJack())
	{
		printf("\n\tBlackjack: ");
		dealerPtr->showFullHand();
		return DEALER_BLACKJACK;
	}
	return 0;
}

void BlackJack::getPlayerInput(Player* playerPtr, Dealer* dealerPtr ,deck_t& playerhand, deck_t* deckPtr , bool splitAces, bool split1, bool split2)
{
	printf("\nPlayers turn.    ");
	int choice{ 0 };

	splitAces = false;

	while(true)
	{
		printf("Enter 1 to stand    Enter 2 to hit    ");
		if (betsOn && playerhand.size() == 2)
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
			if (splitAces && playerhand.size() > 2)
			{
				printf("\nYou can only hit ace splits once.");
				continue;
			}
			playerhand.push_back(getCardFromTop(deckPtr));
			printf("\n\tPlayer hand: ");
			printDeck(&playerhand); // split change
			printf("\n\tPlayer hand value: %i\n", getHandValue(playerhand));
			if (checkBust(playerhand))
				return;
			if (getHandValue(playerhand) == BUST_NUMBER) // 21
				return; // why do you want to hit if you have 21.
			continue;
		case DOUBLE_DOWN:
			if (playerhand.size() > 2)
			{
				printf("\tYou didn't even have the option to double down.\n");
				continue;
			}
			if (m_bank < m_bet * 2 + playerPtr->split1Bet + playerPtr->split2Bet)
			{
				printf("\tYou're too poor to double down.\n");
				continue;
			}
			if (split1)
				playerPtr->split1Bet *= 2;
			else if (split2)
				playerPtr->split2Bet *= 2;
			else
				m_bet *= 2;
			playerhand.push_back(getCardFromTop(deckPtr));
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
			if (m_bank < m_bet * 2 + playerPtr->split1Bet + playerPtr->split2Bet)
			{
				printf("\nYou don't have enough money to place your bet on another hand.\n");
				continue;
			}

			if (playerhand[0].rankNumber() == 1)
			{
				// If we are splitting aces.
				splitAces = true;
			}

			if (playerPtr->getSplit1().empty())
			{
				playerPtr->split1.push_back(playerhand[1]); // set first card to second card of initialHand
				playerPtr->split1.push_back(getCardFromTop(deckPtr)); // set next card to top card from deck
				playerPtr->split1Bet = m_bet;
			}
			else if (playerPtr->getSplit2().empty())
			{
				playerPtr->split2.push_back(playerPtr->split1[1]); // set first card to second card of initialHand
				playerPtr->split2.push_back(getCardFromTop(deckPtr));
				playerPtr->split2Bet = m_bet;
			}
			else if (!playerPtr->getSplit2().empty())
			{
				printf("\nYou can only split twice.\n");
				continue;
			}

			playerhand[1] = getCardFromTop(deckPtr); // set second card from first hand to next card from deck

			if (!playerPtr->split1.empty() && playerPtr->split2.empty())
			{
				printf("\n\tFirst hand: ");
				printDeck(&playerhand);
				getPlayerInput(playerPtr, dealerPtr, playerhand, deckPtr, splitAces, true);
			}
			if (!playerPtr->split1.empty() && playerPtr->split2.empty()) 
			{
				printf("\n\tSecond hand: ");
				printDeck(&playerPtr->split1);
				getPlayerInput(playerPtr, dealerPtr, playerPtr->split1, deckPtr, splitAces, true);
			}
			else if (!playerPtr->split1.empty() && !playerPtr->split2.empty()) 
			{
				printf("\n\tThird hand: ");
				printDeck(&playerPtr->split2);
				getPlayerInput(playerPtr, dealerPtr, playerPtr->split2, deckPtr, splitAces, false, true);
			}

			determineSplits(playerPtr, dealerPtr, deckPtr);
			return;
		}
	}
}


int BlackJack::determineSplits(Player* playerPtr, Dealer* dealerPtr, deck_t* deckPtr)
{
	dealerPtr->AI(deckPtr);

	int firstOutcome = whoWon(playerPtr->getHand(), dealerPtr);
	if (checkBust(playerPtr->getHand())) {
		firstOutcome = DEALER_WINS;
	}

	int secondOutcome = whoWon(playerPtr->split1, dealerPtr);
	if (checkBust(playerPtr->getSplit1())) {
		secondOutcome = DEALER_WINS;
	}
	int thirdOutcome{ SPLIT_DECIDED };
	if (!playerPtr->split1.empty() && !playerPtr->split2.empty())
	{
		thirdOutcome = whoWon(playerPtr->split2, dealerPtr);
		if (checkBust(playerPtr->getSplit2())) {
			thirdOutcome = DEALER_WINS;
		}
	}

	if (dealerPtr->checkBust()) {
		// if dealer busted and player not already busted.
		if (!checkBust(playerPtr->getHand()))
			firstOutcome = PLAYER_WINS;
		if (!checkBust(playerPtr->getSplit1()))
			secondOutcome = PLAYER_WINS;
		if (!playerPtr->split1.empty() && !playerPtr->split2.empty())
			if (!checkBust(playerPtr->getSplit2()))
				thirdOutcome = PLAYER_WINS;
		}

	
	handleOutcomes(playerPtr, dealerPtr ,firstOutcome);
	handleOutcomes(playerPtr,dealerPtr, secondOutcome, true);
	if (!playerPtr->split1.empty() && !playerPtr->split2.empty())
		handleOutcomes(playerPtr,dealerPtr, thirdOutcome, false, true);



	return SPLIT_DECIDED;

}

int whoWon(deck_t& playerHand, Dealer* dealerPtr)
{
	if (getHandValue(playerHand )> dealerPtr->getHandValue())
		return PLAYER_WINS;
	else if (dealerPtr->getHandValue() > getHandValue(playerHand))
		return DEALER_WINS;
	return PUSH;
}

void BlackJack::handleOutcomes(Player* playerPtr,Dealer* dealerPtr, int outcome, bool split1, bool split2)
{
	switch (outcome)
	{
	case PLAYER_WINS:
		m_playerWins++;
		if (betsOn && !(split1 || split2))
		{
			printf("\n\t%s wins $%i!\n", m_name.c_str(), m_bet);
			m_bank += m_bet - m_insureBet;
		}
		else if (betsOn && split1)
		{
			printf("\n\t%s wins $%i on split 1!", m_name.c_str(), playerPtr->split1Bet);
			m_bank += playerPtr->split1Bet;

		}
		else if (betsOn && split2)
		{
			printf("\n\t%s wins $%i on split 2!", m_name.c_str(), playerPtr->split2Bet);
			m_bank += playerPtr->split2Bet;
		}
		else
			printf("\n\t%s wins!\n", m_name.c_str());

		break;
	case DEALER_WINS:
		m_dealerWins++;
		if (betsOn && !(split1 || split2))
		{
			printf("\n\tDealer wins! %s lost $%i!\n", m_name.c_str(), m_bet);
			m_bank -= m_bet - m_insureBet;
		}
		else if (betsOn && split1) {
			printf("\n\tDealer wins on split 1! %s lost $%i!\n", m_name.c_str(), playerPtr->split1Bet);
			m_bank -= playerPtr->split1Bet;
		}
		else if (betsOn && split2) {
			printf("\n\tDealer wins on split 2! %s lost $%i!\n", m_name.c_str(), playerPtr->split2Bet);
			m_bank -= playerPtr->split2Bet;
		}
		else
		{
			printf("\n\tDealer wins!\n");
		}
		break;
	case PUSH:
		m_pushes++;
		if (playerPtr->checkBlackJack() && dealerPtr->checkBlackJack() && m_insureBet > 0) {
			// if player places insurance bet while dealer and player both have Blackjack, pay up
			printf("%s did well making %i on their insurance bet!", m_name.c_str(), static_cast<int>(m_insureBet * INSURANCE_PAYOUT_RATE));
			m_bank += static_cast<int>(m_insureBet * INSURANCE_PAYOUT_RATE);
		}
		else 
		{
			m_bank -= m_insureBet;
		}
		if (betsOn) {
			printf("\n\tDraw! Returned $%i.\n", m_bet);
		}
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
		if (m_insureBet > 0) 
		{
			printf("%s did well making %i on their insurance bet!", m_name.c_str(), static_cast<int>(m_insureBet * INSURANCE_PAYOUT_RATE));
			m_bank += static_cast<int>(m_insureBet * INSURANCE_PAYOUT_RATE);
		}

	case SPLIT_DECIDED:
		break;
	}
}

void printHand(HandInterface* hand)
{

	printf("\n\t%s hand:\t", hand->classString().c_str());
	hand->showGameHand();
}

void BlackJack::printBet() 
{ 
	printf("\nTotal bet: $%i", m_bet);
	if(m_insureBet > 0){
		printf("  |  Insurance bet: %i", m_insureBet);
	}
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
