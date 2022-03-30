#include "blackjack.h"


Card dealCard(const deck_type& deck)
{
    static std::mt19937 mt{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	int randomNumber = rand() % 53; //get a random number 1-52
    return deck[randomNumber]; //return random card from deck
}


void playBlackjack(const deck_type& deck)
{
    //deal cards to dealer and player
    Card dealer1 = dealCard(deck);
    Card dealer2 = dealCard(deck);

    Card player1 = dealCard(deck);
    Card player2 = dealCard(deck);

    int playerAcesNum{ 0 };
    int dealerAcesNum{ 0 };

    if (dealer1.rank == Card::rank_ace) ++dealerAcesNum;
    if (dealer2.rank == Card::rank_ace) ++dealerAcesNum;
    if (player1.rank == Card::rank_ace) ++playerAcesNum;
    if (player2.rank == Card::rank_ace) ++playerAcesNum;

    //get total value of hands of player and dealer
    int playerTotal{ getCardValue(player1) + getCardValue(player2) };
    int dealerTotal{ getCardValue(dealer1) + getCardValue(dealer2) };
    int dealerFaceCard{ getCardValue(dealer1) };

    //Show dealer face up card and print the score of it.
    printf("Dealer Cards:\n");
    printCard(dealer1);
    printf(" | showing %i out of 21\n\n", dealerFaceCard);

    //Show your cards and what the score is out of 21.
    printf("Your cards:\n");
    printCard(player1);
    printf(" ");
    printCard(player2);
    printf(" | %i out of 21", playerTotal);

    bool playerBust{ false };
    int loop2{ 1 };
    while (loop2 != 2 && !playerBust) //while we haven't stood
    {
        printf("\n\n1 for hit. 2 for stand: ");
        std::cin >> loop2;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if(playerTotal <= 21)
        {
            playerHitStand(loop2, playerTotal, deck, playerBust, playerAcesNum, dealer1, dealer2);
        }
        else
        {
            printf("\nYou busted!");
            break;
        }
        
    }

    //Dealer always stands over 17
    //drunk idea: make function that returns a card and saves it to a variable so after the player\
    //hits we can see the cards.
    if(!playerBust)
    {
        int dealerStandNumber{ 17 };
        int counter{ 1 };
        bool dealerBust{ false };
        if (dealerTotal < dealerStandNumber)
        {

            do
            {
                Card dealerExtra = dealCard(deck);
                if (dealerExtra.rank == Card::rank_ace) dealerAcesNum++;
                int dealerExtraValue = getCardValue(dealerExtra);
                dealerTotal += dealerExtraValue;
                printf("\nDealer draw %i: ", counter);
                printCard(dealerExtra);
                if (dealerTotal > 21)
                {
                    if (dealerAcesNum == 0)
                    {
                        printf("\n\nDealer busts with %i\n\nYou win!", dealerTotal);
                        dealerBust = true;
                    }
                    if (dealerAcesNum > 0)
                    {
                        dealerTotal -= (dealerAcesNum * 10); // Ace has value 11 or 1. Subtract 10 to go from 11 to 1.
                        dealerAcesNum = 0;
                    }
                }

                counter++;
            } while (dealerTotal < dealerStandNumber);
        }

        if (playerTotal < dealerTotal && !dealerBust)
        {
            printf("\nYou lose!\nYou has %i, while the dealer had %i", playerTotal, dealerTotal);
        }
        else if (playerTotal > dealerTotal && !dealerBust)
        {
            printf("\nCongrats! You win!\nYou have %i beaten the dealer by %i", playerTotal, playerTotal - dealerTotal);
        }
        else if (playerTotal == dealerTotal && !dealerBust)
        {
            printf("\nWow! A tie!.");
        }
    }
	
}

void playerHitStand(int loop2, int& playerTotal, const deck_type& deck, bool& playerBust, int& playerAcesNum, Card dealer1, Card dealer2)
{
    if (loop2 == 1 && !playerBust) {

        Card playerExtra = dealCard(deck);

        if (playerExtra.rank == Card::rank_ace) ++playerAcesNum;

        int playerExtraValue = getCardValue(playerExtra);
        playerTotal += playerExtraValue;
        printf("\nPlayer draw: ");
        printCard(playerExtra);
    	if (playerTotal > 21)
        {
            if (playerAcesNum == 0)
            {
                printf("\n\nYou busted!");
                playerBust = true;
            }
            if (playerAcesNum > 0)
            {
                playerTotal -= (playerAcesNum * 10); // Ace has value 11 or 1. Subtract 10 to go from 11 to 1.
                playerAcesNum = 0;
            }
        }
    	
        printf(" | %i out of 21", playerTotal);

        

    }
    else if (loop2 == 2) {
        std::cout << "\nPlayer stands. Reavling dealers cards: " << std::endl;
        printCard(dealer1);
        std::cout << " ";
        printCard(dealer2);
        std::cout << std::endl;
    }
    else {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a valid entry";
    }
}


int main()
{
    //create and shuffle deck
    auto deck{ createDeck() };
	shuffleDeck(deck);

    //begin main function in while loop
    int loop{ 1 };
    do
    {
        playBlackjack(deck);
        printf("\n\nWould you like to play again? If so, type 1: ");
        std::cin >> loop;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        printf("\n\n");
    }
    while (loop == 1);
    


}