#include "vendingMachine.h"
#include "printer.h"
#include "nameServer.h"
#include "watcard.h"

VendingMachine::VendingMachine(Printer &prt, NameServer &nameServer, unsigned int id, unsigned int sodaCost,
    unsigned int maxStockPerFlavour) :
        printer(prt),
        nameServer(nameServer),
        id(id),
        sodaCost(sodaCost),
        maxStockPerFlavour(maxStockPerFlavour),
        isRestocking(false),
        raiseFunds(false),
        raiseStock(false),
        currentCard(NULL) {

        for (unsigned int i = 0; i < NumberOfFlavours; i++) {
            stock[i] = 0;
        }
}

void VendingMachine::buy(Flavours flavour, WATCard &card) {
    // store the student's WATCard and desired flavour so that we can access them in `main`
    currentCard = &card;
    currentFlavour = flavour;

    // give `main` a chance to indicate that `Funds` or `Stock` should be thrown
    processing.wait();

    if (raiseFunds) {
        uRendezvousAcceptor();
        _Throw Funds();
    }

    if (raiseStock) {
        uRendezvousAcceptor();
        _Throw Stock();
    }
}

unsigned int* VendingMachine::inventory() {
    // this method is a cue that we are initiating a restock; set the `isRestocking` flag and return our stock
    printer.print(Printer::Vending, id, (char)ReloadStart);
    isRestocking = true;
    return stock;
}

void VendingMachine::restocked() {
    // we are done restocking the machine; reset the `isRestocking` flag
    printer.print(Printer::Vending, id, (char)ReloadEnd);
    isRestocking = false;
}

unsigned int VendingMachine::cost() {
    return sodaCost;
}

unsigned int VendingMachine::getId() {
    return id;
}

void VendingMachine::main() {
    printer.print(Printer::Vending, id, (char)Start, sodaCost);

    // start off by registering ourself with the name server
    nameServer.VMregister(this);

    while (true) {
        _Accept(~VendingMachine) {
            // allow the destructor to terminate the loop
            break;
        } or _When(isRestocking) _Accept(restocked);
          or _When(!isRestocking) _Accept(inventory);
          or _When(!isRestocking) _Accept(buy) {
            raiseStock = (stock[currentFlavour] == 0);              // if out of stock, throw a `Stock` exception
            raiseFunds = (currentCard->getBalance() < sodaCost);    // if insufficient funds, throw a `Funds` exception

            // now that we have determined whether to throw an exception, signal the `buy` method
            processing.signalBlock();

            // only process the transaction if we did not throw an exception
            if (!raiseFunds && !raiseStock) {
                // withdraw the cost of a soda from the student's WATCard and decrement our stock
                currentCard->withdraw(sodaCost);
                stock[currentFlavour]--;

                printer.print(Printer::Vending, id, (char)Buy, (unsigned int)currentFlavour, stock[currentFlavour]);
            } else {
                // if we raised an exception, reset the exception variables accordingly
                raiseFunds = false;
                raiseStock = false;
            }

            // reset `currentCard` to `NULL` now that we have completed the transaction
            currentCard = NULL;
        }
    }

    printer.print(Printer::Vending, id, (char)Finished);
}
