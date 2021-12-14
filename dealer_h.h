//
// Created by Kevin Funderburg on 2019-10-26.
//

#ifndef DEALER_H
#define DEALER_H

#include "card.h"

class Dealer
{
    Card hand;

public:
    Dealer();
    Card deal();
    void draw(Card);
};

Dealer::Dealer() {}

Card Dealer::deal() { return hand; }

void Dealer::draw(Card card) { hand = card; }

#endif //DEALER_H