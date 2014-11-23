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
    currentCard = &card;
    currentFlavour = flavour;

    processing.wait();

    if(raiseFunds) {
        uRendezvousAcceptor();
        _Throw Funds();
    }

    if (raiseStock) {
        uRendezvousAcceptor();
        _Throw Stock();
    }
}

unsigned int* VendingMachine::inventory() {
    printer.print(Printer::Vending, id, (char)ReloadStart);
    isRestocking = true;
    return stock;
}

void VendingMachine::restocked() {
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
    nameServer.VMregister(this);

    while(true) {
        _Accept(~VendingMachine) {
            break;
        } or _When(isRestocking) _Accept(restocked) {
        } or _When(!isRestocking) _Accept(inventory) {
        } or _When(!isRestocking) _Accept(buy) {
            raiseStock = (stock[currentFlavour] == 0);
            raiseFunds = (currentCard->getBalance() < sodaCost);

            processing.signalBlock();

            if(!raiseFunds && !raiseStock) {
                currentCard->withdraw(sodaCost);
                stock[currentFlavour]--;

                printer.print(Printer::Vending, id, (char)Buy, (unsigned int)currentFlavour, stock[currentFlavour]);
            } else {
                raiseFunds = false;
                raiseStock = false;
            }

            currentCard = NULL;
        }
    }

    printer.print(Printer::Vending, id, (char)Finished);
}