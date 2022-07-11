#pragma once
/*Blackjack rules and sources:
 *Can split twice. Can only have up to 3 hands. https://www.onlineblackjackexplorer.com/how-to-play/blackjack-split/
 We can only split alike 10 valued cards. Q Q or K K. No 10 Q or 10 J. May make configurable in future.
if we split ace, we can only hit once. https://www.casinocitytimes.com/alan-krigman/article/in-blackjack-why-can-you-only-hit-aces-once-after-a-split-62324
if we split 10 valued card, and get ace. Not a Blackjack, considered standard 21 hand. if we split ace, we can only hit once. http://www.blackjack-masters.com/splitting-10s/
Dealer hits on 16, stands on 17. This is configurable in DEALER_STAND number. Pretty typical in most casinos.
Blackjacks Payout 3:2. Pretty standard. Some casinos payout 1:1. Also configurable. The higher payout for blackjack, the greater advantage for player obviously.
Double down: double bet on hand (if split hand only doubles one of your bets) then draw 1 card.
 */


constexpr int STARTING_BANK = 1000 ;

constexpr float BLACKJACK_PAYOUT_RATE = 1.5f; // 3:2 payout for blackjacks

constexpr int DEFAULT_NUM_DECKS = 8; // Deck * 52 = number of cards in deck. Casinos use 52 to 416 cards to play Blackjack
constexpr int BUST_NUMBER = 21; // Anything above this is bust. Can change.

constexpr int DEALER_STAND = 17;