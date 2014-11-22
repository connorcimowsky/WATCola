#include "bank.h"

Bank::Bank(unsigned int numStudents) :
    balances(new unsigned int[numStudents]()),
    withdrawals(new uCondition[numStudents]) {
}

Bank::~Bank() {
    delete[] balances;
    delete[] withdrawals;
}

void Bank::deposit(unsigned int id, unsigned int amount) {
    balances[id] += amount;
    withdrawals[id].signal();
}

void Bank::withdraw(unsigned int id, unsigned int amount) {
    while (balances[id] < amount) {
        withdrawals[id].wait();
    }

    balances[id] -= amount;
}
