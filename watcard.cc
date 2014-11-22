#include "watcard.h"

WATCard::WATCard() : balance(0) { }

void deposit(unsigned int amount) {
    balance += amount;
}

void withdraw(unsigned int amount) {
    assert(balance >= amount);

    balance -= amount;
}

unsigned int WATCard::getBalance() {
    return balance;
}