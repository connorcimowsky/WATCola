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
    printer(prt), nameServer(nameServer), cardOffice(cardOffice), id(id), maxPurchases(maxPurchases) {
}

void Student::main() {
    unsigned int bottles = generator(1, maxPurchases);
    unsigned int flavour = generator(0, FLAVOUR_UPPER_BOUND);

    printer.print(Printer::Student, id, (char)Starting, flavour, bottles);

    WATCard::FWATCard card = cardOffice.create(id, WATCARD_BALANCE);

    VendingMachine *vendingMachine = nameServer.getMachine(id);
    printer.print(Printer::Student, id, (char)Selecting, vendingMachine->getId());

    while (bottles > 0) {
        yield(generator(1, 10));

        unsigned int cost = vendingMachine->cost();

        WATCard *wcard = NULL;

        do {
            try {
                wcard = card();
            } catch (WATCardOffice::Lost &l) {
                printer.print(Printer::Student, id, (char)Lost);
                card = cardOffice.create(id, WATCARD_BALANCE);
            }
        } while (wcard == NULL);

        try {
            vendingMachine->buy((VendingMachine::Flavours)flavour, *card);
        } catch (VendingMachine::Funds &f) {
            unsigned int deposit = cost + WATCARD_BALANCE;

            card = cardOffice.transfer(id, deposit, card);

            wcard = NULL;

            do {
                try {
                    wcard = card();
                } catch (WATCardOffice::Lost &l) {
                    printer.print(Printer::Student, id, (char)Lost);
                    card = cardOffice.create(id, WATCARD_BALANCE);
                }
            } while (wcard == NULL);

            continue;
        } catch (VendingMachine::Stock &s) {
            vendingMachine = nameServer.getMachine(id);
            continue;
        }

        printer.print(Printer::Student, id, (char)Bought, card()->getBalance());

        bottles -= 1;
    }

    printer.print(Printer::Student, id, (char)Finished);
}
