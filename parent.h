#ifndef PARENT_H
#define PARENT_H

_Monitor Printer;
_Monitor Bank;

_Task Parent {
public:
    Parent(Printer &prt, Bank &bank, unsigned int numStudents, unsigned int parentalDelay);
private:
    enum State {
        Start    = 'S',
        Deposit  = 'D',
        Finished = 'F'
    };

    Printer &printer;
    Bank &bank;
    unsigned int numStudents;    // total number of students as defined in the configuration file
    unsigned int parentalDelay;  // fixed number of times to yield before each deposit

    void main();
};

#endif
