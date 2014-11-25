#ifndef STUDENT_H
#define STUDENT_H

_Monitor Printer;
_Task NameServer;
_Task WATCardOffice;

_Task Student {
public:
    enum State {
        Starting  = 'S',
        Selecting = 'V',
        Bought    = 'B',
        Lost      = 'L',
        Finished  = 'F'
    };

    Student(Printer &prt, NameServer &nameServer, WATCardOffice &cardOffice, unsigned int id,
        unsigned int maxPurchases);
private:
    Printer &printer;
    NameServer &nameServer;
    WATCardOffice &cardOffice;

    unsigned int id;              // the ID number of this student
    unsigned int maxPurchases;    // the maximum number of bottles a student may purchase, as defined in ConfigParms

    void main();
};

#endif
