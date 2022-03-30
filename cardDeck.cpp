#include <cassert>

#include "blackjack.h"

void printCard(const Card& card)
{
    switch (card.rank)
    {
    case Card::CardRank::rank_2:      std::cout << '2';   break;
    case Card::CardRank::rank_3:      std::cout << '3';   break;
    case Card::CardRank::rank_4:      std::cout << '4';   break;
    case Card::CardRank::rank_5:      std::cout << '5';   break;
    case Card::CardRank::rank_6:      std::cout << '6';   break;
    case Card::CardRank::rank_7:      std::cout << '7';   break;
    case Card::CardRank::rank_8:      std::cout << '8';   break;
    case Card::CardRank::rank_9:      std::cout << '9';   break;
    case Card::CardRank::rank_10:     std::cout << 'T';   break;
    case Card::CardRank::rank_jack:   std::cout << 'J';   break;
    case Card::CardRank::rank_queen:  std::cout << 'Q';   break;
    case Card::CardRank::rank_king:   std::cout << 'K';   break;
    case Card::CardRank::rank_ace:    std::cout << 'A';   break;
    default:
        std::cout << '?';
        break;
    }

    switch (card.suit)
    {
    case Card::CardSuit::club:       std::cout << 'C';   break;
    case Card::CardSuit::diamond:    std::cout << 'D';   break;
    case Card::CardSuit::heart:      std::cout << 'H';   break;
    case Card::CardSuit::spade:      std::cout << 'S';   break;
    default:
        std::cout << '?';
        break;
    }
}


int getCardValue(const Card& card)
{
    switch (card.rank)
    {
    case Card::CardRank::rank_2:        return 2;
    case Card::CardRank::rank_3:        return 3;
    case Card::CardRank::rank_4:        return 4;
    case Card::CardRank::rank_5:        return 5;
    case Card::CardRank::rank_6:        return 6;
    case Card::CardRank::rank_7:        return 7;
    case Card::CardRank::rank_8:        return 8;
    case Card::CardRank::rank_9:        return 9;
    case Card::CardRank::rank_10:       return 10;
    case Card::CardRank::rank_jack:     return 10;
    case Card::CardRank::rank_queen:    return 10;
    case Card::CardRank::rank_king:     return 10;
    case Card::CardRank::rank_ace:      return 11;
    default:
        assert(false && "should never happen");
        return 0;
    }
}

void printDeck(const deck_type& deck)
{
    for (const auto& card : deck)
    {
        printCard(card);
        std::cout << ' ';
    }

    std::cout << '\n';
}

void shuffleDeck(deck_type& deck)
{
    // mt is static so it only gets seeded once.
    static std::mt19937 mt{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };

    std::shuffle(deck.begin(), deck.end(), mt);
}

deck_type createDeck()
{
    deck_type deck{};
    index_type index{ 0 };

    for (int suit{ 0 }; suit < static_cast<int>(Card::CardSuit::max_suits); ++suit)
    {
        for (int rank{ 0 }; rank < static_cast<int>(Card::CardRank::max_ranks); ++rank)
        {
            deck[index].suit = static_cast<Card::CardSuit>(suit);
            deck[index].rank = static_cast<Card::CardRank>(rank);
            ++index;
        }

    }
    return deck;
}
