#include "watcard.h"


WATCard::WATCard() : balance(0) {

}

WATCard::WATCard(const WATCard &toCopy) {
    balance = toCopy.balance;
}

WATCard& WATCard::operator=(const WATCard &toAssign) {
    WATCard newCard = new WATCard(toAssign);

    return newCard;
}

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