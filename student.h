#ifndef STUDENT_H
#define STUDENT_H

_Monitor Printer;
_Task NameServer;
_Task WATCardOffice;

_Task Student {
public:
    enum State {
        Starting = 'S',
        Selecting = 'V',
        Bought = 'B',
        Lost = 'L',
        Finished = 'F'
    };
    Student(Printer &prt, NameServer &nameServer, WATCardOffice &cardOffice, unsigned int id,
        unsigned int maxPurchases);
private:
    Printer &printer;
    NameServer &nameServer;
    WATCardOffice &cardOffice;
    unsigned int id;
    unsigned int maxPurchases;
    void main();
};

#endif
