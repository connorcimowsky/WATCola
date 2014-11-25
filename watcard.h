#ifndef WATCARD_H
#define WATCARD_H

#include <uFuture.h>

class WATCard {
public:
    WATCard();
    typedef Future_ISM<WATCard *> FWATCard;    // future containing a pointer to a WATCard
    void deposit(unsigned int amount);
    void withdraw(unsigned int amount);
    unsigned int getBalance();
private:
    // prevent copying
    WATCard(const WATCard &);
    WATCard &operator=(const WATCard &);

    // the current balance of this WATCard
    unsigned int balance;
};

#endif
