#ifndef BOTTLINGPLANT_H
#define BOTTLINGPLANT_H

#include "vendingMachine.h"

_Monitor Printer;
_Task NameServer;

_Task BottlingPlant {
public:
    _Event Shutdown {};    // shut down the plant
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

    // as defined in ConfigParms
    unsigned int numVendingMachines;
    unsigned int maxShippedPerFlavour;
    unsigned int maxStockPerFlavour;
    unsigned int timeBetweenShipments;

    bool shuttingDown;    // required for notifying ourselves to throw a `Shutdown` exception
    unsigned int productionRun[VendingMachine::NumberOfFlavours];    // the number of bottles produced for each flavour

    void main();
};

#endif
