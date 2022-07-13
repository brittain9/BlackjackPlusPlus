#pragma once
#include <string>
#include "card.h"
#include "settings.h"


class BlackJack
{
	std::string m_name;
	int m_bank;
	int m_bet;
	int m_lastBet;
	int m_insureBet;

	// Stats
	int m_playerWins;
	int m_dealerWins;
	int m_playerBlackjacks;
	int m_dealerBlackjacks;
	int m_pushes;
	int m_highestBank;
	
public:
	BlackJack(std::string name) : m_name(name), m_bank(STARTING_BANK), m_bet(0), m_lastBet(0),m_insureBet(0), m_playerWins(0), m_dealerWins(0), m_pushes(0), m_playerBlackjacks(0), m_dealerBlackjacks(0), m_highestBank(STARTING_BANK) { printf("Hello %s!\n", name.c_str()); }

	~BlackJack()
	{ printf("\n\tPlayer wins: %i\tDealer Wins: %i\tPushes: %i\tPlayer Blackjacks: %i\tDealer Blackjacks: %i\n\tHighest Bank: $%i\tProfit: $%i\n\n", m_playerWins, m_dealerWins, m_pushes,m_playerBlackjacks, m_dealerBlackjacks,m_highestBank, m_bank - STARTING_BANK); }

	void Blackjack(int decks = DEFAULT_NUM_DECKS);
	int playBlackJack(Player* playerPtr, Dealer* dealerPtr, deck_t* deckPtr);
	void bet();
	void insuranceBet();
	int handleBlackjacks(Player* playerPtr, Dealer* dealerPtr);
	void getPlayerInput(Player* playerPtr, Dealer* dealerPtr, deck_t& playerhand, deck_t* deckPtr, bool splitAces, bool split1 = false, bool split2 = false);
	void handleOutcomes(Player* playerPtr, Dealer* dealerPtr,int outcome, bool split1 = false, bool split2 = false);
	int determineSplits(Player* playerPtr, Dealer* dealerPtr, deck_t* deckPtr);

	void incPlayerWins() { m_playerWins++; }
	void incDealerWins() { m_dealerWins++; }
	void incPush() { m_pushes++; }

	const int getBank() const { return m_bank; }
	void printBet();
};

void printHand(HandInterface* hand);
int whoWon(deck_t& playerHand, Dealer* dealerPtr);

template<typename T>
T getInput(std::string message);
