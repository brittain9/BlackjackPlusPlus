#include "stdafx.h"

#include "timer.h"
#include "card.h"
#include "blackjack.h"




int main()
{
	Timer t;
	bool loop{ true };

	int playerWins{ 0 };
	int dealerWins{ 0 };
	int draws{ 0 };

	do
	{
		int Winner = BlackJack(8);
		switch (Winner)
		{
		case 0:

			playerWins++;
			printf("\nPlayer wins.\n");
			break;
		case 1:
			dealerWins++;
			printf("\nDealer wins.\n"); \
				break;
		case 2:
			draws++;
			printf("\nDraw.\n");
			break;
		}
		printf("\nEnter 1 to play again: ");
		std::cin >> loop;
	}
	while (loop);
	

	printf("\n\tPlayer wins: %i\t\tDealer Wins: %i\tDraws: %i\n\nTime Elasped : ",playerWins, dealerWins, draws, t.elapsed());

}

