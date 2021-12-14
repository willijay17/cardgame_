//
// Created by Kevin Funderburg on 2019-10-24.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <fstream>
#include "card.h"
using namespace std;

class Player
{
private:
    int num;
    int FULL_HAND = 2;
    int handSize;
    Card* hand;
    bool turn;
    bool winner;

public:
    Player(int);
    void draw(Card);
    void push(Card);
    Card discard();
    bool isPair();
    bool isWinner();
    bool isTurn();
    string getHand();
    void exit();
    void log(string);
    void out();
};

Player::Player(int n):num(n), handSize(0), winner(false)
{
    hand = new Card[2];
}

void Player::draw(Card card)
{
    hand[handSize] = card;
    handSize++;

    log("draws " + to_string(card.num));
    log(getHand());
}

void Player::push(Card card)
{
    hand[handSize] = card;
    handSize++;
    log(getHand());
}

string Player::getHand()
{
    string str;
    for (int i = 0; i < handSize; ++i) {
        if (i == 0) str = to_string(hand[i].num);
        else str += " " + to_string(hand[i].num);
    }
    return "HAND " + str;
}

Card Player::discard()
{
    Card *tmpHand = new Card[2];
    int i = rand() % FULL_HAND;
    Card remove = hand[i];
    Card keep;
    if (i == 0) keep = hand[1];
    else keep = hand[0];

    tmpHand[0] = keep;
    delete[] hand;
    hand = tmpHand;
    handSize = 1;
    log("discarded " + to_string(remove.num));
    return remove;
}

bool Player::isWinner()
{
    if (isPair()) {
        winner = true;
        log("WINS");
        out();
        return true;
    } else {
        winner = false;
        out();
        return false;
    }
}

/**
 * check if hand contains a pair
 * @return bool
 */
bool Player::isPair() { return hand[0].num == hand[1].num; }

void Player::exit() { log("exits round"); }

/**
 * output to log file
 */
void Player::log(string str)
{
    ofstream logger("log.data",  ios::out | ios::app);    //output to text file for viewing
    if (logger.is_open()) logger << "PLAYER " << num << ": " << str << endl;
}

/**
 * output to console
 */
void Player::out()
{
    string win = winner ? "yes" : "no";
    cout << "PLAYER " << num << ":\n"
         << getHand() << endl
         << "WIN " << win << endl;
}

#endif //PLAYER_H