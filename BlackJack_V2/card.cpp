#include "stdafx.h"
#include "card.h"

#define ACE_ADDITONAL_VALUE 10

std::string Card::suitString()
{
	switch (suit)
	{
	case suit_t::hearts:
		return "hearts";
	case suit_t::diamonds:
		return "diamonds";
	case suit_t::clubs:
		return "clubs";
	case suit_t::spades:
		return "spades";
	}
}
constexpr int Card::rankValue()
{
	switch (rank)
	{
	case rank_t::ace:
		return 1;
	case rank_t::two:
		return 2;
	case rank_t::three:
		return 3;
	case rank_t::four:
		return 4;
	case rank_t::five:
		return 5;
	case rank_t::six:
		return 6;
	case rank_t::seven:
		return 7;
	case rank_t::eight:
		return 8;
	case rank_t::nine:
		return 9;
	case rank_t::ten:
	case rank_t::jack:
	case rank_t::queen:
	case rank_t::king:
		return 10;
	}
}

void printCard(const Card& card)
{
	switch (card.rank)
	{
	case Card::rank_t::ace:		std::cout << 'A';	break;
	case Card::rank_t::two:		std::cout << '2';	break;
	case Card::rank_t::three:	std::cout << '3';	break;
	case Card::rank_t::four:	std::cout << '4';	break;
	case Card::rank_t::five:	std::cout << '5';	break;
	case Card::rank_t::six:		std::cout << '6';	break;
	case Card::rank_t::seven:	std::cout << '7';	break;
	case Card::rank_t::eight:	std::cout << '8';	break;
	case Card::rank_t::nine:	std::cout << '9';	break;
	case Card::rank_t::ten:		std::cout << "10";	break;
	case Card::rank_t::jack:	std::cout << 'J';	break;
	case Card::rank_t::queen:	std::cout << 'Q';	break;
	case Card::rank_t::king:	std::cout << 'K';	break;
	}
	switch (card.suit)
	{
	case Card::suit_t::hearts:      std::cout << 'H';   break;
	case Card::suit_t::diamonds:    std::cout << 'D';   break;
	case Card::suit_t::clubs:		std::cout << 'C';   break;
	case Card::suit_t::spades:      std::cout << 'S';   break;
	}
}

void printDeck(const deck_t* deck)
{
	for (Card card : *deck)
	{
		printCard(card);
		std::cout << ' ';
	}

	std::cout << '\n';
}
void printDeck(deck_t& deck)
{
	for (const auto& card : deck)
	{
		printCard(card);
		std::cout << ' ';
	}

	std::cout << '\n';
}

bool HandInterface::checkBlackJack()
{
	if ((hand[0].rankValue() == 1 || hand[1].rankValue() == 1) && !(hand[0].rankValue() == 1 && hand[1].rankValue() == 1))
		// if one of players card is an ace but not both.
		if (hand[0].rankValue() == 10 || hand[1].rankValue() == 10)
			// if player also has a card with rankValue 10. Player has blackjacck
			return true;

	return false;

}
inline void HandInterface::drawCard(deck_t* deck)
{
	hand.push_back(getCardFromTop(deck));
}
int HandInterface::getHandValue()
{
	int value{ 0 };
	bool hasAce = false;
	for (Card Card : hand)
	{
		value += Card.rankValue();
		if (Card.rankValue() == 1)
		{
			// if has ace in hand
			hasAce = true;
		}
	}
	if (hasAce && (value + ACE_ADDITONAL_VALUE) <= BUST_NUMBER)
		value += ACE_ADDITONAL_VALUE;
	return value;
}
void HandInterface::checkBust()
{
	if (getHandValue() > BUST_NUMBER)
		isBusted = true;
}

void Dealer::AI(deck_t* deck)
{
	// Return true if dealer busts, false if not.

	showFullHand();
	while(true)
	{
		if (getHandValue() > DEALER_STAND)
		{
			printf("\nDealer stands.\n");
			break;
		}
	
		drawCard(deck);
		showFullHand();
		std::cout << "\n\tDealer hand value : " << getHandValue() << '\n';
		checkBust();
		if (isBusted)
			return;
	}
}



deck_t _makeDeck(int numDecks)
{
	// Many casinos use up to 8 - 10 decks to play BlackJack. numDecks will allow you to create a 52 card deck by default or 416 (8 deck) card deck.
	constexpr auto allRanks = std::array{
		Card::rank_t::ace,
		Card::rank_t::two,
		Card::rank_t::three,
		Card::rank_t::four,
		Card::rank_t::five,
		Card::rank_t::six,
		Card::rank_t::seven,
		Card::rank_t::eight,
		Card::rank_t::nine,
		Card::rank_t::ten,
		Card::rank_t::jack,
		Card::rank_t::queen,
		Card::rank_t::king
	};

	constexpr auto allSuits = std::array{
		Card::suit_t::hearts,
		Card::suit_t::diamonds,
		Card::suit_t::clubs,
		Card::suit_t::spades
	};

	deck_t deck{};

	for(int i{0}; i < numDecks; i++)
	{
		for (auto&& rank : allRanks)
		{
			for (auto&& suit : allSuits)
			{
				Card card(rank, suit);
				deck.push_back(card);
			}
		}
	}
	
	return deck;
}
deckPtr_t allocateDeck(int numDecks)
{
	deckPtr_t returnDeckPtr = std::make_unique<deck_t>(_makeDeck(numDecks));
	return returnDeckPtr;
}

void shuffleDeck(deck_t* deck)
{
	static std::mt19937 mt{rng() };
	std::shuffle(deck->begin(), deck->end(), mt);
}

const Card& getCardFromTop(deck_t* deck)
{
	Card returnCard = deck->at(0); // return first card from deck.
	deck->erase(deck->begin()); // remove that card from deck
	return returnCard; // After player is done with the card, card should be added back to deck. Then next turn deck reshuffled.
}

dealerPtr_t allocateDealer(deck_t* deck)
{
	dealerPtr_t returnPtr = std::make_unique<Dealer>(deck);
	return returnPtr;
}

playerPtr_T allocatePlayer(deck_t* deck)
{

	playerPtr_T returnPtr = std::make_unique<Player>(deck);
	return returnPtr;
}