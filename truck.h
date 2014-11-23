#ifndef TRUCK_H
#define TRUCK_H

#include "vendingMachine.h"

_Monitor Printer;
_Task NameServer;
_Task BottlingPlant;

_Task Truck {
    enum States {
        Start = 'S',
        PickedUp = 'P',
        Begin = 'd',
        Unsuccessful = 'U',
        Delivered = 'D',
        Finish = 'F'
    };

    Printer &printer;
    NameServer &nameServer;
    BottlingPlant &plant;
    unsigned int numVendingMachines;
    unsigned int maxStockPerFlavour;
    unsigned int cargo[VendingMachine::NumberOfFlavours];
    unsigned int machineForRestock;

    void main();
    unsigned int bottlesLeft();
  public:
    Truck(Printer &prt, NameServer &nameServer, BottlingPlant &plant,
           unsigned int numVendingMachines, unsigned int maxStockPerFlavour);
};

#endif
