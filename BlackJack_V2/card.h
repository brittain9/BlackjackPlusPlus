#pragma once
#include <string>
#include <vector>
#include <memory>
#include "rng.h"
#include "settings.h"

class Player;
class Dealer;
class Card;


typedef std::vector<Card> deck_t;
typedef std::unique_ptr<deck_t> deckPtr_t;
typedef std::unique_ptr<Dealer> dealerPtr_t;
typedef std::unique_ptr<Player> playerPtr_T;

class Card
{
private:
	enum suit_t : unsigned char // Using unsigned char for suit so it will be 1 byte only.
	{
		hearts,
		diamonds,
		clubs,
		spades
	};

	enum rank_t : unsigned char
	{
		ace = 1,
		two,
		three,
		four,
		five,
		six,
		seven,
		eight,
		nine,
		ten,
		jack,
		queen,
		king
	};

	rank_t rank;
	suit_t suit;

public:
	Card(rank_t r, suit_t s)
		: rank(r), suit(s) {}

	friend deck_t _makeDeck(int numDecks);
	friend void printCard(const Card& card);

	std::string suitString();
	constexpr int rankValue();
};

deck_t _makeDeck(int numDecks = DEFAULT_NUM_DECKS);
deckPtr_t allocateDeck(int numDecks = DEFAULT_NUM_DECKS);
dealerPtr_t allocateDealer(deck_t* deck);
playerPtr_T allocatePlayer(deck_t* deck);


void shuffleDeck(deck_t* deck);

void printCard(const Card& card);
void printDeck(const deck_t* deck);
void printDeck(deck_t& deck);

const Card& getCardFromTop(deck_t* deck);

class HandInterface
{
protected:
	deck_t hand;
	bool isBusted = false;
public:
	// Dealer and Player both subclasses of HandInterface. Automatically when constructed they will be given a hand.
	HandInterface(deck_t* deck) : hand({ getCardFromTop(deck), getCardFromTop(deck) }) {}

	// Getter for hand
	deck_t getHand() { return hand; }
	bool Busted()
	{
		return isBusted;
	}

	int getHandValue();

	void checkBust();
	 inline void resetBust()
	{
		isBusted = false;
	}

	bool checkBlackJack();

	inline void drawCard(deck_t* deck);

	// Pure virtual function
	virtual void showGameHand() = 0;


	virtual void AI(deck_t* deck) = 0;
};

class Dealer : public HandInterface
{
public:
	Dealer(deck_t* deck) : HandInterface(deck) {}

	 void showGameHand() override
	 {
		// Dealer has only one card facing so print the first card in his hand.
		 printCard(hand[0]);
		 std::cout << " //"; // indicates hidden card. Graphics
	 }
	void showFullHand()
	{
		// for creating game
		printDeck(hand);
	}

	int dealerFaceCardValue()
	{
		return hand[DEALER_FACE].rankValue();
	}

	void AI(deck_t* deck) override;

	
};

class Player :public HandInterface
{
public:
	Player(deck_t* deck) : HandInterface(deck) {}

	 void showGameHand() override
	 {
		 printDeck(hand);
	 }

	void AI(deck_t* deck) override
	{
		return; // not yet implemented. have to have as its a pure virtual function
	}
};