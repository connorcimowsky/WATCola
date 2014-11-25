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

    // get the list of registered vending machines from the name server; if any machines haven't been registered yet,
    // NameServer will block `getMachineList` until the remaining machines have been registered
    VendingMachine** vendingMachines = nameServer.getMachineList();

    while (true) {
        yield(generator(1, 10));
        try {
            // pick up the next shipment from the bottling plant
            plant.getShipment(cargo);
        } catch (BottlingPlant::Shutdown &s) {
            // if the plant has shut down, break out of the loop and allow ourselves to terminate
            break;
        }

        printer.print(Printer::Truck, (char)PickedUp, bottlesLeft());

        for (unsigned int i = 0; i < numVendingMachines; i++) {
            if (cargo[VendingMachine::BluesBlackCherry] == 0
            && cargo[VendingMachine::ClassicalCreamSoda] == 0
            && cargo[VendingMachine::RockRootBeer] == 0
            && cargo[VendingMachine::JazzLime] == 0) {
                // if no bottles of soda have been produced for any flavor, no work is necessary for this production run
                break;
            }

            // get a pointer to the next vending machine needing to be restocked
            VendingMachine *machine = vendingMachines[machineForRestock];

            printer.print(Printer::Truck, (char)Begin, machine->getId(), bottlesLeft());

            // get the number of stocked bottles for each flavour from the vending machine
            unsigned int *inventory = machine->inventory();

            unsigned int notReplenished = 0;
            for (unsigned int j = 0; j < VendingMachine::NumberOfFlavours; j++) {
                // compute the number of bottles to stock for this flavour
                unsigned int toStock = maxStockPerFlavour - inventory[j];

                if (cargo[j] < toStock) {
                    // if the provided shipment is not sufficient, increment `notReplenished` by the difference
                    notReplenished += toStock - cargo[j];

                    // since we don't have enough bottles, stock as many as we can, i.e., `cargo[j]`
                    toStock = cargo[j];
                }

                // decrement the truck's cargo and increment the machine's inventory
                cargo[j] -= toStock;
                inventory[j] += toStock;
            }
            printer.print(Printer::Truck, (char)Unsuccessful, machine->getId(), notReplenished);
            printer.print(Printer::Truck, (char)Delivered, machine->getId(), bottlesLeft());

            // let the vending machine know that the restocking has completed
            machine->restocked();

            // select the next machine to restock in a round-robin fashion
            machineForRestock = (machineForRestock < numVendingMachines - 1) ? machineForRestock + 1 : 0;
        }
    }

    printer.print(Printer::Truck, (char)Finish);
}

unsigned int Truck::bottlesLeft() {
    // compute the total number of bottles (across all flavours) remaining on the truck
    unsigned int total = 0;
    for(unsigned int i = 0; i < VendingMachine::NumberOfFlavours; i++) {
        total += cargo[i];
    }

    return total;
}
