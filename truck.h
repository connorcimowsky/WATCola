#ifndef TRUCK_H
#define TRUCK_H

_Monitor Printer;
_Task NameServer;
_Task BottlingPlant;

_Task Truck {
    void main();
  public:
    Truck(Printer &prt, NameServer &nameServer, BottlingPlant &plant,
           unsigned int numVendingMachines, unsigned int maxStockPerFlavour);
};

#endif
