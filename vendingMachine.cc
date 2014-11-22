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
    raiseStock(false) {
}

void VendingMachine::buy(Flavours flavour, WATCard &card) {
    currentCard = &card;
    currentFlavour = flavour;

    processing.wait();

    if(raiseFunds) {
        raiseFunds = false;
        _Resume Funds();
    }

    processing.wait();

    if(raiseStock) {
        raiseStock = false;
        _Resume Stock();
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
    printer.print(Printer::Vending, (char)Start, id, sodaCost);
    nameServer.VMregister(this);

    while(true) {
        _Accept(~VendingMachine) {
            break;
        } or _Accept(inventory) {
        } or _When(isRestocking) _Accept(restocked) {
        } or _When(!isRestocking) _Accept(buy) {
            if (stock[currentFlavour] == 0) {
                raiseStock = true;
            }

            processing.signalBlock();

            if(currentCard->getBalance() < sodaCost) {
                raiseFunds = true;
            }

            processing.signalBlock();

            currentCard->withdraw(sodaCost);
            stock[currentFlavour]--;

            printer.print(Printer::Vending, id, (char)Buy, currentFlavour, stock[currentFlavour]);
        }
    }

    printer.print(Printer::Vending, id, (char)Finished);
}