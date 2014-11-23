#include "bottlingPlant.h"
#include "nameServer.h"
#include "printer.h"
#include "truck.h"
#include "MPRNG.h"
#include <iostream>

using namespace std;

BottlingPlant::BottlingPlant(Printer &prt, NameServer &nameServer, unsigned int numVendingMachines,
                 unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour, unsigned int timeBetweenShipments) :
    printer(prt), nameServer(nameServer),
    numVendingMachines(numVendingMachines),
    maxShippedPerFlavour(maxShippedPerFlavour),
    maxStockPerFlavour(maxStockPerFlavour),
    timeBetweenShipments(timeBetweenShipments),
    shuttingDown(false) {
}

void BottlingPlant::getShipment(unsigned int cargo[]) {
    if(shuttingDown) {
        uRendezvousAcceptor();
        _Throw Shutdown();
    }

    for (unsigned int i = 0; i < VendingMachine::NumberOfFlavours; i++) {
        cargo[i] = productionRun[i];
    }

    printer.print(Printer::BottlingPlant, (char)Pickup);
}

void BottlingPlant::main() {
    printer.print(Printer::BottlingPlant, (char)Start);
    Truck truck(printer, nameServer, *this, numVendingMachines, maxStockPerFlavour);

    while(true) {
        yield(timeBetweenShipments);

        unsigned int totalProduced = 0;
        for (unsigned int i = 0; i < (unsigned int)VendingMachine::NumberOfFlavours; i++) {
            productionRun[i] = generator(maxShippedPerFlavour);
            totalProduced += productionRun[i];
        }

        printer.print(Printer::BottlingPlant, (char)Generating, totalProduced);

        _Accept(~BottlingPlant) {
            shuttingDown = true;
            break;
        } or _Accept(getShipment) {
        }
    }

    printer.print(Printer::BottlingPlant, (char)Finish);

    _Accept(getShipment) {
    }
}