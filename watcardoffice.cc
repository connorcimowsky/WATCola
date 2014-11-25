#include "watcardoffice.h"
#include "bank.h"
#include "printer.h"
#include "watcard.h"
#include "MPRNG.h"
#include <iostream>

using namespace std;

WATCardOffice::WATCardOffice(Printer &prt, Bank &bank, unsigned int numCouriers) :
    printer(prt),
    bank(bank),
    numCouriers(numCouriers) {
    couriers = new Courier*[numCouriers];

    for (unsigned int i = 0; i < numCouriers; i++) {
        couriers[i] = new Courier(printer, bank, *this, i);
    }
}

WATCardOffice::~WATCardOffice() {
    delete[] couriers;
}

WATCard::FWATCard WATCardOffice::create(unsigned int sid, unsigned int amount) {
    Job *job = new Job(sid, amount, NULL);
    jobQueue.push(job);
    printer.print(Printer::WATCardOffice, (char)Create, sid, amount);

    workAvailable.signal();

    return job->result;
}

WATCard::FWATCard WATCardOffice::transfer(unsigned int sid, unsigned int amount, WATCard *card) {
    Job *job = new Job(sid, amount, card);
    jobQueue.push(job);
    printer.print(Printer::WATCardOffice, (char)Transfer, sid, amount);
    workAvailable.signal();
    return job->result;
}

WATCardOffice::Job* WATCardOffice::requestWork() {
    if(jobQueue.empty()) {
        workAvailable.wait();
        if(jobQueue.empty()) {  //If we are woken up and have no jobs return NULL to indicate we are done
            return NULL;
        }
    }

    Job *job = jobQueue.front();
    jobQueue.pop();

    printer.print(Printer::WATCardOffice, (char)Work);

    return job;
}

void WATCardOffice::main() {
    printer.print(Printer::WATCardOffice, (char)Start);

    while(true) {
        _Accept(~WATCardOffice) {
            while(!workAvailable.empty()) {     // Force all couriers to finish requesting work
                workAvailable.signalBlock();
            }

            for (unsigned int i = 0; i < numCouriers; i++) {    //Force them to finish
                delete couriers[i];
            }

            delete[] couriers;

            break;                              // Finish
        } or _Accept(create, transfer) {
        } or _Accept(requestWork);
    }

    printer.print(Printer::WATCardOffice, (char)Finish);
}

WATCardOffice::Job::Job(unsigned int sid, unsigned int amount, WATCard *card) :
    sid(sid),
    amount(amount),
    card(card) {
}

WATCardOffice::Courier::Courier(Printer &prt, Bank &bank, WATCardOffice &office, unsigned int id) :
    printer(prt),
    bank(bank),
    office(office),
    id(id) {
}

void WATCardOffice::Courier::main() {
    printer.print(Printer::Courier, id, (char)Start);

    while(true) {
        Job *work = office.requestWork();

        if(work == NULL) {  // if we get no work we are done
            break;
        }

        if(work->card == NULL) {            // create a watcard
            work->card = new WATCard();
        }

        printer.print(Printer::Courier, id, (char)StartTransfer, work->sid, work->amount);

        bank.withdraw(work->sid, work->amount);
        work->card->deposit(work->amount);

        printer.print(Printer::Courier, id, (char)CompleteTransfer, work->sid, work->amount);

        if(generator(5) == 0) {                 // check if we lost the card
            delete work->card;
            work->result.exception(new Lost());
        } else {
            work->result.delivery(work->card);
        }

        delete work;
    }

    printer.print(Printer::Courier, id, (char)Finish);
    _Accept(~Courier) {     // Let the courier terminate
    }
}
