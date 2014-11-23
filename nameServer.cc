#include "nameServer.h"
#include "printer.h"
#include "vendingMachine.h"

NameServer::NameServer(Printer &prt, unsigned int numVendingMachines, unsigned int numStudents) :
    printer(prt),
    numVendingMachines(numVendingMachines),
    numStudents(numStudents),
    numRegistered(0) {
    machineList = new VendingMachine*[numVendingMachines];
    machineAssignment = new unsigned int[numStudents];
}

NameServer::~NameServer() {
    delete[] machineList;
    delete[] machineAssignment;
}

void NameServer::VMregister(VendingMachine *vendingmachine) {
    machineList[numRegistered] = vendingmachine;
    numRegistered++;
    printer.print(Printer::NameServer, (char)Register, vendingmachine->getId());
}

VendingMachine* NameServer::getMachine(unsigned int id) {
    VendingMachine *ret = machineList[machineAssignment[id]];
    printer.print(Printer::NameServer, (char)New, id, ret->getId());
    machineAssignment[id] = (machineAssignment[id] < numVendingMachines) ? machineAssignment[id] + 1 : 0;
    return ret;
}

VendingMachine** NameServer::getMachineList() {
    return machineList;
}

void NameServer::main() {
    printer.print(Printer::NameServer, (char)Start);
    outer: while(true) {
        while(numRegistered < numVendingMachines) {
            _Accept(~NameServer) {
                break outer;
            } or _Accept(VMregister) {
            }
        }

        for (unsigned int i = 0; i < numStudents; i++) {
            unsigned int machine = i % numVendingMachines;
            machineAssignment[i] = machine;
        }

        while(true) {
            _Accept(~NameServer) {
                break outer;
            } or _Accept(getMachine, getMachineList) {
            }
        }
    }

    printer.print(Printer::NameServer, (char)Finished);
}
