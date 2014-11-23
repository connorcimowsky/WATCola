#ifndef BANK_H
#define BANK_H

_Monitor Bank {
    unsigned int numStudents;
    unsigned int *balances;
    uCondition **withdrawals;

public:
    Bank(unsigned int numStudents);
    ~Bank();
    void deposit(unsigned int id, unsigned int amount);
    void withdraw(unsigned int id, unsigned int amount);
};

#endif
