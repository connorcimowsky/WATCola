#ifndef TRUCK_H
#define TRUCK_H

#include "vendingMachine.h"

_Monitor Printer;
_Task NameServer;
_Task BottlingPlant;

_Task Truck {
public:
    Truck(Printer &prt, NameServer &nameServer, BottlingPlant &plant,
           unsigned int numVendingMachines, unsigned int maxStockPerFlavour);
private:
    enum States {
        Start        = 'S',
        PickedUp     = 'P',
        Begin        = 'd',
        Unsuccessful = 'U',
        Delivered    = 'D',
        Finish       = 'F'
    };

    Printer &printer;
    NameServer &nameServer;
    BottlingPlant &plant;

    // constants as defined in ConfigParms
    unsigned int numVendingMachines;
    unsigned int maxStockPerFlavour;

    // the number of bottles of each flavour being transported
    unsigned int cargo[VendingMachine::NumberOfFlavours];

    // the current vending machine that needs to be restocked
    unsigned int machineForRestock;

    // the total number of bottles (across all flavours) remaining on the truck
    unsigned int bottlesLeft();

    void main();
};

#endif
