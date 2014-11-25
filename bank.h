#ifndef BANK_H
#define BANK_H

_Monitor Bank {
public:
    Bank(unsigned int numStudents);
    ~Bank();
    void deposit(unsigned int id, unsigned int amount);
    void withdraw(unsigned int id, unsigned int amount);
private:
    unsigned int numStudents;    // total number of students as defined in the configuration file
    unsigned int *balances;      // an array of balances for each student's bank account

    // an array of condition variables, one for each bank account; used for blocking tasks from
    // withdrawing money until funds have been deposited into the respective bank account
    uCondition *withdrawals;
};

#endif
