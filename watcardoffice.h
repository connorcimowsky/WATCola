#ifndef WATCARDOFFICE_H
#define WATCARDOFFICE_H

#include <queue>
#include "watcard.h"

_Monitor Printer;
_Monitor Bank;

_Task WATCardOffice {
public:
    _Event Lost {};                        // lost WATCard
    WATCardOffice(Printer &prt, Bank &bank, unsigned int numCouriers);
    ~WATCardOffice();
    WATCard::FWATCard create(unsigned int sid, unsigned int amount);
    WATCard::FWATCard transfer(unsigned int sid, unsigned int amount, WATCard *card);
    Job *requestWork();

private:
    struct Job {                           // marshalled arguments and return future
        unsigned int sid;
        unsigned int amount;
        WATCard *card;
        WATCard::FWATCard result;          // return future
        Job(unsigned int sid, unsigned int amount, WATCard *card);
    };

    _Task Courier {
    public:
        Courier(Printer &prt, Bank &bank, WATCardOffice &office, unsigned int id);

    private:
        enum States {
            Start = 'S',
            StartTransfer = 't',
            CompleteTransfer = 'T',
            Finish = 'F'
        };

        Printer &printer;
        Bank &bank;
        WATCardOffice &office;
        unsigned int id;

        void main();
    };                 // communicates with bank

    enum States {
        Start = 'S',
        Work = 'W',
        Create = 'C',
        Transfer = 'T',
        Finish = 'F'
    };

    Printer &printer;
    Bank &bank;
    unsigned int numCouriers;

    Courier **couriers;
    std::queue<Job*> jobQueue;
    uCondition workAvailable;

    void main();
};

#endif
