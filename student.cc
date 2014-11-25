#include "student.h"
#include "printer.h"
#include "nameServer.h"
#include "watcard.h"
#include "watcardoffice.h"
#include "vendingMachine.h"
#include "MPRNG.h"

static const unsigned int FLAVOUR_UPPER_BOUND = 3;
static const unsigned int WATCARD_BALANCE = 5;

Student::Student(Printer &prt, NameServer &nameServer, WATCardOffice &cardOffice, unsigned int id,
    unsigned int maxPurchases) :
        printer(prt),
        nameServer(nameServer),
        cardOffice(cardOffice),
        id(id),
        maxPurchases(maxPurchases) {
}

void Student::main() {
    unsigned int bottles = generator(1, maxPurchases);           // select a random number of bottles to purchase
    unsigned int flavour = generator(0, FLAVOUR_UPPER_BOUND);    // select a random flavour to purchase

    printer.print(Printer::Student, id, (char)Starting, flavour, bottles);

    // create a WATCard with the WATCard office
    WATCard::FWATCard card = cardOffice.create(id, WATCARD_BALANCE);

    // get a pointer to this student's assigned vending machine
    VendingMachine *vendingMachine = nameServer.getMachine(id);

    printer.print(Printer::Student, id, (char)Selecting, vendingMachine->getId());

    // loop until we have drank the desired number of bottles as determined above
    while (bottles > 0) {
        yield(generator(1, 10));

        // ask the vending machine how much a soda costs
        unsigned int cost = vendingMachine->cost();

        // store a pointer to the future's value so that we may determine when it has been successfully acquired
        WATCard *wcard = NULL;

        do {
            try {
                wcard = card();                   // attempt to access the future's value
            } catch (WATCardOffice::Lost &l) {    // if the courier lost our WATCard, create a new one
                printer.print(Printer::Student, id, (char)Lost);
                card = cardOffice.create(id, WATCARD_BALANCE);
            }
        } while (wcard == NULL);    // continue attempting to access the future's value until we are successful

        try {
            // attempt to buy a bottle of soda
            vendingMachine->buy((VendingMachine::Flavours)flavour, *card);
        } catch (VendingMachine::Funds &f) {
            // we do not have sufficient funds on our WATCard; determine the amount to be transferred onto our card
            unsigned int deposit = cost + WATCARD_BALANCE;

            // ask the WATCard office to transfer `deposit` to our card
            card = cardOffice.transfer(id, deposit, card);

            // reset the temporary variable used for accessing the value of the future
            wcard = NULL;

            // since the courier may have lost our card during the transfer, repeatedly attempt to access the future's
            // value, creating new cards as necessary, until successful
            do {
                try {
                    wcard = card();
                } catch (WATCardOffice::Lost &l) {
                    printer.print(Printer::Student, id, (char)Lost);
                    card = cardOffice.create(id, WATCARD_BALANCE);
                }
            } while (wcard == NULL);

            // now that we have acquired the funds, re-attempt to purchase a soda (without decrementing `bottles`)
            continue;
        } catch (VendingMachine::Stock &s) {
            // the vending machine has run out of our desired flavour; request a new vending machine
            vendingMachine = nameServer.getMachine(id);

            // now that we have a new vending machine, re-attempt to purchase a soda (without decrementing `bottles`)
            continue;
        }

        printer.print(Printer::Student, id, (char)Bought, card()->getBalance());

        // now that we have successfully drank a bottle of soda, decrement `bottles` and loop around
        bottles -= 1;
    }

    // if we have made it this far, our WATCard was not lost by a courier; thus, we must free its memory
    delete card();

    printer.print(Printer::Student, id, (char)Finished);
}
