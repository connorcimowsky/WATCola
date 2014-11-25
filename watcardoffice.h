#ifndef WATCARDOFFICE_H
#define WATCARDOFFICE_H

#include <queue>
#include "watcard.h"

_Monitor Printer;
_Monitor Bank;

_Task WATCardOffice {
struct Job;
public:
    _Event Lost {};    // courier lost a student's WATCard
    WATCardOffice(Printer &prt, Bank &bank, unsigned int numCouriers);
    WATCard::FWATCard create(unsigned int sid, unsigned int amount);
    WATCard::FWATCard transfer(unsigned int sid, unsigned int amount, WATCard *card);
    Job *requestWork();
private:
    struct Job {
        unsigned int sid;            // student ID of the associated student
        unsigned int amount;         // amount to be withdrawn from the student's bank account
        WATCard *card;               // a pointer to the student's WATCard
        WATCard::FWATCard result;    // the return value of the job (pointer to the student's WATCard)
        Job(unsigned int sid, unsigned int amount, WATCard *card);
    };

    _Task Courier {
    public:
        Courier(Printer &prt, Bank &bank, WATCardOffice &office, unsigned int id);
    private:
        enum States {
            Start            = 'S',
            StartTransfer    = 't',
            CompleteTransfer = 'T',
            Finish           = 'F'
        };

        Printer &printer;
        Bank &bank;
        WATCardOffice &office;

        unsigned int id;    // the ID of this courier

        void main();
    };

    enum States {
        Start    = 'S',
        Work     = 'W',
        Create   = 'C',
        Transfer = 'T',
        Finish   = 'F'
    };

    Printer &printer;
    Bank &bank;

    unsigned int numCouriers;    // as defined in ConfigParms

    Courier **couriers;          // a list of our couriers
    std::queue<Job*> jobQueue;   // a queue of jobs to be carried out by our couriers
    uCondition workAvailable;    // used for synchronizing couriers depending on whether there are jobs to be performed

    void main();
};

#endif
