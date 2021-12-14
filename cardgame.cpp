//
// Created by Kevin Funderburg on 2019-10-24.
//

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>

#include "deck.h"
#include "card.h"
#include "player.h"
#include "dealer.h"

using namespace std;

void *drawCards(void *param);

#define NUM_ROUNDS 3

// global variables
Deck deck;  //shared data for all threads
bool winner;
ofstream logfile;
pthread_mutex_t player_mutex;
pthread_cond_t winnerExists;
pthread_t dlr, player1, player2, player3;

/**
 * This structure is passed as a parameter when creating a player thread
 */
struct thread_data
{
    int  thread_id; //integer value to name the player (1, 2, or 3)
    Card card;      //the first card dealt to player by the dealer
};

void init()
{
    deck = Deck();
    winner = false;
    logfile.open("log.data",  ios::out | ios::app);
}

void *drawCards(void *param)
{
    auto *arg = (thread_data *)param;   //cast param to thread_data structure
    Player player(arg->thread_id);      //create player object with thread id
    player.push(arg->card);             //add card to players hand

    while (!winner)
    {
        pthread_mutex_lock(&player_mutex);

        player.draw(deck.topCard());
        deck.popCard();

        if (player.isWinner()) {
            winner = true;
            player.exit();
            /*
            Check if winner exists and signal waiting thread when condition is
            reached.  Note that this occurs while mutex is locked.
            */
            pthread_cond_signal(&winnerExists);
        } else {
            deck.push(player.discard());
        }

        pthread_mutex_unlock(&player_mutex);

        /* Do some "work" so threads can alternate on _mutex lock */
        sleep(1);
    }
    player.exit();
    pthread_mutex_unlock(&player_mutex);

}

void *_dealer(void *param)
{
    auto *arg = (thread_data *)param;   //cast param to thread_data structure
    auto myID = arg->thread_id;
    /**
     * Lock mutex and wait for signal.
     */
    pthread_mutex_lock(&player_mutex);
    while(!winner)
    {
        pthread_cond_wait(&winnerExists, &player_mutex);
        cout << "Winner exists.\n";
    }
    pthread_mutex_unlock(&player_mutex);

}

int main(int argc, char *argv[])
{
    for (int j = 0; j < NUM_ROUNDS; ++j)
    {
        string _round = "\n----------------------------------------------\n"
                     "\t\tROUND: " + to_string(j + 1) + "\n"
                     "----------------------------------------------\n";
        cout << _round;

        init();
        deck.shuffle();
        deck.show();
        Dealer dealer;

        thread_data d;
        d.thread_id = 5;

        // create the thread_data with the thread ID and the first card
        thread_data td[3];
        for (int i = 0; i < 3; ++i) {
            td[i].thread_id = i+1;
            dealer.draw(deck.topCard());
            td[i].card = dealer.deal();
            deck.popCard();
        }
        deck.show();

        /* Initialize mutex and condition variable objects */
        pthread_mutex_init(&player_mutex, nullptr);
        pthread_cond_init (&winnerExists, nullptr);

        /* Create threads */
        pthread_create(&dlr, nullptr, _dealer, (void *) &d);
        pthread_create(&player1, nullptr, drawCards, (void *) &td[0]);
        pthread_create(&player2, nullptr, drawCards, (void *) &td[1]);
        pthread_create(&player3, nullptr, drawCards, (void *) &td[2]);

        /* Wait for all threads to complete */
        pthread_join(player1, nullptr);
        pthread_join(player2, nullptr);
        pthread_join(player3, nullptr);
    }

    logfile.close();

    /* Clean up and exit */
    pthread_mutex_destroy(&player_mutex);
    pthread_exit(nullptr);
}