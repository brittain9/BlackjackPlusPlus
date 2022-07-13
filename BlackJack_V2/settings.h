#pragma once
/*Blackjack rules and sources:
 *Can split twice. Can only have up to 3 hands. https://www.onlineblackjackexplorer.com/how-to-play/blackjack-split/
 *Can split by value or rank.
 *if we split ace, we can only hit once. https://www.casinocitytimes.com/alan-krigman/article/in-blackjack-why-can-you-only-hit-aces-once-after-a-split-62324
 
*if we split 10 valued card, and get ace. Not a Blackjack, considered standard 21 hand http://www.blackjack-masters.com/splitting-10s/
*Dealer hits on 16, stands on 17. This is configurable in DEALER_STAND number. Pretty typical in most casinos.

*Blackjacks Payout 3:2. Pretty standard. Some casinos payout 1:1. Also configurable. The higher payout for blackjack, the greater advantage for player obviously.
*Double down: double bet on hand (if split hand only doubles one of your bets) then draw 1 card.

* Insurance bets can be placed if the dealer is showing an ace. This bet involves betting a max of half of your original stake and pays out at 2:1 if the dealer has Blackjack. The insurance bet reduces the overall house edge. 

*Perfect Pairs – this side bet uses the player’s cards only, and pays out if you are dealt two of a kind as follows:
    Mixed pair (two of the same value but different suit and colour) – pays 5:1
    Coloured pair (two of the same value and the same colour) – pays 12:1
    Perfect pair (two of the same card) – pays 25:1

*21+3 – this side bet involves the player’s two cards and the upturned card of the dealer. It will pay out for a number of different combinations:
    Flush – (all cards are suited) – pays 5:1
    Straight – (all cards consecutive) – pays 10:1
    Three of a kind – (not the same suit) – pays 30:1
    Straight flush – (consecutive cards same suit) – pays 40:1
    Suited triple – (three of the same card) – pays 100:1
https://www.pokernews.com/casino/blackjack-side-bets.htm
 */

static bool SPLIT_BY_VALUE{ true }; // if true, can split any 10 valued cards

static bool betsOn{ true }; // enable or disable betting

constexpr int STARTING_BANK = 1000;

constexpr float BLACKJACK_PAYOUT_RATE = 1.5f; // 3:2 payout for blackjacks
constexpr float INSURANCE_PAYOUT_RATE = 2.0f;

constexpr int DEFAULT_NUM_DECKS = 8; // Deck * 52 = number of cards in deck. Casinos use 52 to 416 cards to play Blackjack
constexpr int BUST_NUMBER = 21; // Anything above this is bust. Can change.

constexpr int DEALER_STAND = 17;

constexpr int MIN_CARDS_BEFORE_NEW = 4;