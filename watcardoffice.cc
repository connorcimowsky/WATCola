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
    numCouriers(numCouriers),
    couriers(new Courier*[numCouriers]) {
        for (unsigned int i = 0; i < numCouriers; i++) {
            couriers[i] = new Courier(printer, bank, *this, i);
        }
}

WATCard::FWATCard WATCardOffice::create(unsigned int sid, unsigned int amount) {
    // create a new job and push it onto the job queue
    Job *job = new Job(sid, amount, NULL);
    jobQueue.push(job);

    printer.print(Printer::WATCardOffice, (char)Create, sid, amount);

    // now that we have pushed a new job into our queue, there is work available for our couriers
    workAvailable.signal();

    // return the future synchronously; the student will block when it tries to access the value
    return job->result;
}

WATCard::FWATCard WATCardOffice::transfer(unsigned int sid, unsigned int amount, WATCard *card) {
    // create a new job for one of our couriers; push it onto the job queue
    Job *job = new Job(sid, amount, card);
    jobQueue.push(job);

    printer.print(Printer::WATCardOffice, (char)Transfer, sid, amount);

    // now that we have pushed a new job into our queue, there is work available for our couriers
    workAvailable.signal();

    // return the future synchronously; the student will block when it tries to access the value
    return job->result;
}

WATCardOffice::Job* WATCardOffice::requestWork() {
    if (jobQueue.empty()) {
        // if there is no work to be done, block until a job is created
        workAvailable.wait();
        if(jobQueue.empty()) {
            // if we are woken up and no jobs are available, the WATCardOffice is shutting down; return NULL to
            // indicate to the courier that it is time to terminate
            return NULL;
        }
    }

    // pop a job from the job queue and return it to the courier

    Job *job = jobQueue.front();
    jobQueue.pop();

    printer.print(Printer::WATCardOffice, (char)Work);

    return job;
}

void WATCardOffice::main() {
    printer.print(Printer::WATCardOffice, (char)Start);

    while (true) {
        _Accept(~WATCardOffice) {
            while(!workAvailable.empty()) {
                // force all of the couriers to see that no more work is available
                workAvailable.signalBlock();
            }

            for (unsigned int i = 0; i < numCouriers; i++) {
                // block on the termination of the couriers
                delete couriers[i];
            }

            delete[] couriers;

            // break from the loop and allow ourselves to terminate
            break;
        } or _Accept(create, transfer, requestWork);
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

    while (true) {
        // request a new job from the WATCardOffice
        Job *work = office.requestWork();

        if (work == NULL) {
            // if the WATCard office returns `NULL` from `requestWork`, it is shutting down; we need to terminate
            break;
        }

        if (work->card == NULL) {
            // if `card` is `NULL`, we need to create a new WATCard on behalf of the WATCard office
            work->card = new WATCard();
        }

        printer.print(Printer::Courier, id, (char)StartTransfer, work->sid, work->amount);

        // withdraw the specified amount from the student's bank account and deposit it into the WATCard
        bank.withdraw(work->sid, work->amount);
        work->card->deposit(work->amount);

        printer.print(Printer::Courier, id, (char)CompleteTransfer, work->sid, work->amount);

        // the courier loses the card one in every six times
        if (generator(5) == 0) {
            delete work->card;

            // if the courier lost the card, place a `Lost` exception into the future
            work->result.exception(new Lost());
        } else {
            // if the courier did not lose the card, place a pointer to it in the future
            work->result.delivery(work->card);
        }

        // when we have completed a job, it is our responsibility to delete it
        delete work;
    }

    printer.print(Printer::Courier, id, (char)Finish);

    // if we reach this, we have broken out of the above loop and have no more work to do
    _Accept(~Courier);
}
