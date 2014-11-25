#ifndef BOTTLINGPLANT_H
#define BOTTLINGPLANT_H

#include "vendingMachine.h"

_Monitor Printer;
_Task NameServer;

_Task BottlingPlant {
public:
    _Event Shutdown {};
    BottlingPlant(Printer &prt, NameServer &nameServer, unsigned int numVendingMachines,
        unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour, unsigned int timeBetweenShipments);
    void getShipment(unsigned int cargo[]);
private:
    enum States {
        Start      = 'S',
        Generating = 'G',
        Pickup     = 'P',
        Finish     = 'F'
    };

    Printer &printer;
    NameServer &nameServer;
    unsigned int numVendingMachines;
    unsigned int maxShippedPerFlavour;
    unsigned int maxStockPerFlavour;
    unsigned int timeBetweenShipments;
    bool shuttingDown;
    unsigned int productionRun[VendingMachine::NumberOfFlavours];

    void main();
};

#endif
