#include "bank.h"

Bank::Bank(unsigned int numStudents) :
    numStudents(numStudents),
    balances(new unsigned int[numStudents]()),
    withdrawals(new uCondition*[numStudents]) {
    for (unsigned int i = 0; i < numStudents; i++) {
        withdrawals[i] = new uCondition();
    }
}

Bank::~Bank() {
    for (unsigned int i = 0; i < numStudents; i++) {
        delete withdrawals[i];
    }

    delete[] balances;
    delete[] withdrawals;
}

void Bank::deposit(unsigned int id, unsigned int amount) {
    balances[id] += amount;
    withdrawals[id]->signal();
}

void Bank::withdraw(unsigned int id, unsigned int amount) {
    while (balances[id] < amount) {
        withdrawals[id]->wait();
    }

    balances[id] -= amount;
}
