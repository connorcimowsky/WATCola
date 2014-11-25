#include "bank.h"

Bank::Bank(unsigned int numStudents) :
    numStudents(numStudents),
    balances(new unsigned int[numStudents]()),
    withdrawals(new uCondition[numStudents]) {
}

Bank::~Bank() {
    delete[] balances;
    delete[] withdrawals;
}

void Bank::deposit(unsigned int id, unsigned int amount) {
    balances[id] += amount;      // increment the appropriate account's balance by `amount`
    withdrawals[id].signal();    // allow blocked withdrawers to re-check their balance
}

void Bank::withdraw(unsigned int id, unsigned int amount) {
    // a busy wait is necessary here; `deposit` has no way of signalling a specific withdrawer,
    // as we do not keep track of each task's desired withdrawal amount
    while (balances[id] < amount) {
        withdrawals[id].wait();
    }

    balances[id] -= amount;    // decrement the appropriate account's balance by `amount`
}
