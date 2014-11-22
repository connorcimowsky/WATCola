#ifndef BANK_H
#define BANK_H

_Monitor Bank {
public:
    Bank(unsigned int numStudents);
    void deposit(unsigned int id, unsigned int amount);
    void withdraw(unsigned int id, unsigned int amount);
private:
    unsigned int *balances;    // array of balances for each student
    uCondition *withdrawals;   // array of conditions used for synchronizing withdrawals
};

#endif
