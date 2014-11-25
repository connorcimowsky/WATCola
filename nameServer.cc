#include "nameServer.h"
#include "printer.h"
#include "vendingMachine.h"

NameServer::NameServer(Printer &prt, unsigned int numVendingMachines, unsigned int numStudents) :
    printer(prt),
    numVendingMachines(numVendingMachines),
    numStudents(numStudents),
    numRegistered(0),
    machineList(new VendingMachine*[numVendingMachines]),
    machineAssignment(new unsigned int[numStudents]) {
}

NameServer::~NameServer() {
    delete[] machineList;
    delete[] machineAssignment;
}

void NameServer::VMregister(VendingMachine *vendingmachine) {
    // add `vendingMachine` to the end of the list of registered machines
    machineList[numRegistered] = vendingmachine;

    printer.print(Printer::NameServer, (char)Register, vendingmachine->getId());
}

VendingMachine* NameServer::getMachine(unsigned int id) {
    // cache a pointer to the vending machine associated with student `id`
    VendingMachine *ret = machineList[machineAssignment[id]];

    printer.print(Printer::NameServer, (char)New, id, ret->getId());

    // after caching the return value, perform a circular increment to the next vending machine
    machineAssignment[id] = (machineAssignment[id] < numVendingMachines - 1) ? machineAssignment[id] + 1 : 0;

    return ret;
}

VendingMachine** NameServer::getMachineList() {
    return machineList;
}

void NameServer::main() {
    printer.print(Printer::NameServer, Starting);

    outer: while (true) {
        // block tasks from using vending machines until `numVendingMachines` machines have been registered
        while (numRegistered < numVendingMachines) {
            _Accept(~NameServer) {
                // allow the destructor to terminate the loop
                break outer;
            } or _Accept(VMregister) {
                // each time we register a vending machine, increment `numRegistered`
                numRegistered++;
            }
        }

        // now that machines have been registered, assign each student to a vending machine
        for (unsigned int i = 0; i < numStudents; i++) {
            unsigned int machine = i % numVendingMachines;
            machineAssignment[i] = machine;
        }

        while (true) {
            _Accept(~NameServer) {
                // allow the destructor to terminate the loop
                break outer;
            } or _Accept(getMachine, getMachineList);    // otherwise, finally allow normal calls to our public methods
        }
    }

    printer.print(Printer::NameServer, (char)Finished);
}
