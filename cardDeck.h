#pragma once


class Card
{
public:
	enum CardSuit
	{
		club,
		diamond,
		heart,
		spade,

		max_suits
	};

	enum CardRank
	{
		rank_2,
		rank_3,
		rank_4,
		rank_5,
		rank_6,
		rank_7,
		rank_8,
		rank_9,
		rank_10,
		rank_jack,
		rank_queen,
		rank_king,
		rank_ace,

		max_ranks
	};

	CardRank rank{};
	CardSuit suit{};

};

using deck_type = std::array<Card, 52>;
using index_type = deck_type::size_type;

void printCard(const Card& card);
int getCardValue(const Card& card);
void printDeck(const deck_type& deck);
void shuffleDeck(deck_type& deck);
deck_type createDeck();