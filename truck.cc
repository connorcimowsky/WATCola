#include "truck.h"
#include "printer.h"
#include "nameServer.h"
#include "bottlingPlant.h"
#include "MPRNG.h"

Truck::Truck(Printer &prt, NameServer &nameServer, BottlingPlant &plant,
           unsigned int numVendingMachines, unsigned int maxStockPerFlavour) :
    printer(prt),
    nameServer(nameServer),
    plant(plant),
    numVendingMachines(numVendingMachines),
    maxStockPerFlavour(maxStockPerFlavour),
    machineForRestock(0) {
}

void Truck::main() {
    printer.print(Printer::Truck, (char)Start);
    VendingMachine** vendingMachines = nameServer.getMachineList();

    while(true) {
        yield(generator(1, 10));
        try {
            plant.getShipment(cargo);
        } catch (BottlingPlant::Shutdown &s) {
            break;
        }

        printer.print(Printer::Truck, (char)PickedUp, bottlesLeft());

        for (unsigned int i = 0; i < numVendingMachines; i++) {
            if(cargo[0] == 0 && cargo[1] == 0 && cargo[2] == 0 && cargo[3] == 0) {
                break;
            }

            VendingMachine *machine = vendingMachines[machineForRestock];

            printer.print(Printer::Truck, (char)Begin, machine->getId(), bottlesLeft());

            unsigned int *inventory = machine->inventory();

            unsigned int notReplenished = 0;
            for (unsigned int j = 0; j < VendingMachine::NumberOfFlavours; j++) {
                unsigned int toStock = maxStockPerFlavour - inventory[i];

                if(cargo[i] < toStock) {
                    notReplenished += toStock - cargo[i];
                    toStock = cargo[i];
                }

                cargo[i] -= toStock;
                inventory[i] += toStock;
            }

            if (notReplenished > 0) {
                printer.print(Printer::Truck, (char)Unsuccessful, machine->getId(), notReplenished);
            } else {
                printer.print(Printer::Truck, (char)Delivered, machine->getId(), bottlesLeft());
            }

            machine->restocked();
            machineForRestock++;
        }
    }

    printer.print(Printer::Truck, (char)Finish);
}

unsigned int Truck::bottlesLeft() {
    unsigned int total = 0;
    for(unsigned int i = 0; i < VendingMachine::NumberOfFlavours; i++) {
        total += cargo[i];
    }

    return total;
}
