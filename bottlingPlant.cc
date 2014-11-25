#include "bottlingPlant.h"
#include "nameServer.h"
#include "printer.h"
#include "truck.h"
#include "MPRNG.h"

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
    // throw a `Shutdown` exception if the plant is shutting down
    if (shuttingDown) {
        uRendezvousAcceptor();    // ensure the exception is raised on the correct stack
        _Throw Shutdown();
    }

    // copy the number of bottles of each flavour into `cargo`
    for (unsigned int i = 0; i < VendingMachine::NumberOfFlavours; i++) {
        cargo[i] = productionRun[i];
    }

    // indicate that we have completed the pickup
    printer.print(Printer::BottlingPlant, (char)Pickup);
}

void BottlingPlant::main() {
    printer.print(Printer::BottlingPlant, (char)Start);

    // create a truck to perform deliveries to vending machines
    Truck truck(printer, nameServer, *this, numVendingMachines, maxStockPerFlavour);

    while (true) {
        yield(timeBetweenShipments);

        unsigned int totalProduced = 0;    // keep track of the cumulative number of bottles produced
        for (unsigned int i = 0; i < (unsigned int)VendingMachine::NumberOfFlavours; i++) {
            productionRun[i] = generator(maxShippedPerFlavour);    // produce a random number of bottles
            totalProduced += productionRun[i];
        }

        printer.print(Printer::BottlingPlant, (char)Generating, totalProduced);

        _Accept(~BottlingPlant) {
            // allow the destructor to terminate the production loop
            shuttingDown = true;    // indicate that it is time to shut down (for callers to `getShipment`)
            break;
        } or _Accept(getShipment);
    }

    printer.print(Printer::BottlingPlant, (char)Finish);

    _Accept(getShipment);    // allow one final call to `getShipment` so that our truck can terminate
}
