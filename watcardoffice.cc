#include "watcardoffice.h"
#include "bank.h"
#include "printer.h"
#include "watcard.h"
#include "MPRNG.h"

using namespace std;

WATCardOffice::WATCardOffice(Printer &prt, Bank &bank, unsigned int numCouriers) :
    printer(prt),
    bank(bank),
    numCouriers(numCouriers) {
    couriers = new Courier*[numCouriers];

    for (unsigned int i = 0; i < numCouriers; i++) {
        couriers[i] = new Courier(printer, bank, *this);
    }
}

WATCardOffice::~WATCardOffice() {
    for (unsigned int i = 0; i < numCouriers; i++) {
        delete couriers[i];
    }

    delete[] couriers;
}

WATCard::FWATCard WATCardOffice::create(unsigned int sid, unsigned int amount) {
    Job *job = new Job(sid, amount, NULL);
    jobQueue.push(job);
    printer.print(Printer::WATCardOffice, (char)Create, sid, amount);
    return job->result;
}

WATCard::FWATCard WATCardOffice::transfer(unsigned int sid, unsigned int amount, WATCard *card) {
    Job *job = new Job(sid, amount, card);
    jobQueue.push(job);
    printer.print(Printer::WATCardOffice, (char)Transfer, sid, amount);
    return job->result;
}

WATCardOffice::Job* WATCardOffice::requestWork() {
    if(jobQueue.empty()) {
        workAvailable.wait();
    }

    Job *job = jobQueue.front();
    jobQueue.pop();
    return job;
}

void WATCardOffice::main() {
    printer.print(Printer::WATCardOffice, (char)Start);
    while(true) {
        _Accept(~WATCardOffice) {
            break;
        } or _Accept(create, transfer) {
            workAvailable.signal();
        } or _Accept(requestWork) {
            printer.print(Printer::WATCardOffice, (char)Work);
        }
    }
    printer.print(Printer::WATCardOffice, (char)Finish);
}

WATCardOffice::Job::Job(unsigned int sid, unsigned int amount, WATCard *card) :
    sid(sid),
    amount(amount),
    card(card) {
}

WATCardOffice::Courier::Courier(Printer &prt, Bank &bank, WATCardOffice &office) :
    printer(prt),
    bank(bank),
    office(office) {
}

void WATCardOffice::Courier::main() {
    while(true) {
        Job* work = office.requestWork();

        if(work->card == NULL) {
            work->card = new WATCard();
        }

        bank.withdraw(work->sid, work->amount);
        work->card->deposit(work->amount);

        if(generator(5) == 0) {
            delete work->card;
            work->result.exception(new Lost());
        }

        delete work;
    }
}
