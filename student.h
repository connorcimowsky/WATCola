#ifndef STUDENT_H
#define STUDENT_H

_Monitor Printer;
_Task NameServer;
_Task WATCardOffice;

_Task Student {
    void main();
  public:
    Student(Printer &prt, NameServer &nameServer, WATCardOffice &cardOffice, unsigned int id,
            unsigned int maxPurchases);
};

#endif
