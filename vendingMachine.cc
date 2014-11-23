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

    for (unsigned int i = 0; i < NumberOfFlavours; i++) {
        stock[i] = 0;
    }
}

void VendingMachine::buy(Flavours flavour, WATCard &card) {
    raiseFunds = (card.getBalance() < sodaCost);
    raiseStock = (stock[(unsigned int)flavour] == 0);


    if(!raiseFunds && !raiseStock) {
        printer.print(Printer::Vending, id, (char)Buy, (unsigned int)flavour, stock[(unsigned int)flavour]);
        card.withdraw(sodaCost);
        stock[(unsigned int)flavour]--;
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
            if (raiseFunds) {
                raiseFunds = false;
                raiseStock = false;
                throw Funds();
            }

            if(raiseStock) {
                raiseFunds = false;
                raiseStock = false;
                throw Stock();
            }
        }
    }

    printer.print(Printer::Vending, id, (char)Finished);
}