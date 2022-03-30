#pragma once
#include <iostream>
#include <array>
#include <algorithm>
#include <random>
#include <stdlib.h>
#include "cardDeck.h"
#include <limits>

void playerHitStand(int loop2, int& playerTotal, const deck_type& deck, bool& playerBust, int& playerAcesNum, Card dealer1, Card dealer2);