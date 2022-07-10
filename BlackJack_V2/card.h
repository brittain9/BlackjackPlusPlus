#pragma once
#include <string>
#include <vector>
#include <memory>
#include "settings.h"

#define DEALER_FACE = 0; // dealer face card is index 0 of hand.

class HandInterface;
class Player;
class Dealer;
class Card;

//Typedefs
typedef std::vector<Card> deck_t;

//Declarations
deck_t _makeDeck(int numDecks = DEFAULT_NUM_DECKS);

void shuffleDeck(deck_t* deck);


void printDeck(const deck_t* deck);


Card getCardFromTop(deck_t* deck);
deck_t& createStartHand(deck_t* deck);

// Classes
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

	Card& operator=(const Card& card1)
	{
		rank = card1.rank;
		suit = card1.suit;

		return *this;
	}

	friend deck_t _makeDeck(int numDecks);
	friend void printCard(const Card& card);
	friend void printCard(Card* card);

	std::string suitString();
	constexpr int rankValue(); 
	int rankNumber(); // differentiates between 10, Jack, Queen, King
};

void printCard(const Card& card);
void printCard(Card* card);

// Had to make a lot of methods I had also into functions taking a hand for splitting
static bool isSplittable(deck_t hand) { return hand[0].rankNumber() == hand[1].rankNumber(); }
void drawCard( deck_t* deck, deck_t* hand);
inline void showGameHand(const deck_t& hand);
int getHandValue(deck_t hand);
bool checkBust(deck_t hand);

static void clearDeck(deck_t& deck) 
{
	deck.clear();
}


class HandInterface
{
protected:
	deck_t hand;
public:
	deck_t& getHand() { return hand; }
	int getHandValue();

	bool checkBust(); // Return true if busted
	bool checkBlackJack(); // Return true if BlackJack

	inline void drawCard(deck_t* deck);

	void setStartingHand(deck_t* deck) { hand.push_back(getCardFromTop(deck)); hand.push_back(getCardFromTop(deck)); }
	void clearHand() { hand.clear(); }



	// Pure virtual function
	virtual void showGameHand() = 0;

	virtual bool AI(deck_t* deck) = 0; // Return true if busted, false if not
	virtual std::string classString() = 0;


};


class Dealer : public HandInterface
{
public:

	 void showGameHand() override
	 {
		// Dealer has only one card facing so print the first card in his hand.
		 printCard(hand[0]);
		 std::cout << " ??"; // indicates hidden card. Graphics
	 }
	void showFullHand()
	{
		printDeck(&hand);
	}

	int dealerFaceCardValue()
	{
		return hand[0].rankValue();
	}

	bool AI(deck_t* deck) override;

	 std::string classString() override{ return "Dealer"; };
};

class Player :public HandInterface
{
public: // make public for now
	deck_t split1 = {};
	int split1Bet = 0;
	deck_t split2 = {};
	int split2Bet = 0;
public:

	 void showGameHand() override
	 {
		 printDeck(&hand);
	 }

	bool AI(deck_t* deck) override
	{
		return true; // not yet implemented. have to have as its a pure virtual function
	}

	std::string classString() override { return "Player"; };

	void setHand(deck_t newHand)
	{
		hand = newHand;
	}

	deck_t getSplit1() { return split1; }

	deck_t getSplit2() { return split2; }
};