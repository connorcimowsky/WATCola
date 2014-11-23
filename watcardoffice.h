#ifndef WATCARDOFFICE_H
#define WATCARDOFFICE_H

#include <queue>
#include "watcard.h"

_Monitor Printer;
_Monitor Bank;

_Task WATCardOffice {
    enum States {
        Start = 'S',
        Work = 'W',
        Create = 'C',
        Transfer = 'T',
        Finish = 'F'
    };

    struct Job {                           // marshalled arguments and return future
        unsigned int sid;
        unsigned int amount;
        WATCard *card;
        WATCard::FWATCard result;          // return future
        Job(unsigned int sid, unsigned int amount, WATCard *card);
    };

    _Task Courier {
        enum States {
            Start = 'S',
            StartTransfer = 't',
            CompleteTransfer = 'T',
            Finish = 'F'
        };

        Printer &printer;
        Bank &bank;
        WATCardOffice &office;

        void main();
    public:
        Courier(Printer &prt, Bank &bank, WATCardOffice &office);
    };                 // communicates with bank

    Printer &printer;
    Bank &bank;
    unsigned int numCouriers;

    Courier **couriers;
    std::queue<Job*> jobQueue;
    uCondition workAvailable;

    void main();
public:
    _Event Lost {};                        // lost WATCard
    WATCardOffice(Printer &prt, Bank &bank, unsigned int numCouriers);
    ~WATCardOffice();
    WATCard::FWATCard create(unsigned int sid, unsigned int amount);
    WATCard::FWATCard transfer(unsigned int sid, unsigned int amount, WATCard *card);
    Job *requestWork();
};

#endif
