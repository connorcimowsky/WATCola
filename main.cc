#include "MPRNG.h"
#include "config.h"
#include "printer.h"
#include "bank.h"
#include "parent.h"
#include "watcardoffice.h"
#include "nameServer.h"
#include "vendingMachine.h"
#include "bottlingPlant.h"
#include "student.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

MPRNG generator;

bool convert(int &val, char *buffer) {        // convert C string to integer
    stringstream ss(buffer);         // connect stream and buffer
    ss >> dec >> val;                   // convert integer from buffer
    return !ss.fail() &&               // conversion successful ?
    // characters after conversion all blank ?
    string(buffer).find_first_not_of(" ", ss.tellg()) == string::npos;
} // convert

void uMain::main() {
    int seed = getpid();
    ConfigParms config;

    processConfigFile("soda.config", config);

    switch (argc) {
        case 3:
            if(!convert(seed, argv[2])) {
                cerr << "Usage: ./soda_64 [ config-file [ random-seed (> 0) ] ]" << endl;
                exit(EXIT_FAILURE);
            }
        case 2:
            processConfigFile(argv[1], config);
        case 1:
            break;
        default:
            cerr << "Usage: ./soda_64 [ config-file [ random-seed (> 0) ] ]" << endl;
            exit(EXIT_FAILURE);
    }

    Printer printer(config.numStudents, config.numVendingMachines, config.numCouriers);
    Bank bank(config.numStudents);
    Parent *parent = new Parent(printer, bank, config.numStudents, config.parentalDelay);
    WATCardOffice *office = new WATCardOffice(printer, bank, config.numCouriers);
    NameServer *nameServer = new NameServer(printer, config.numVendingMachines, config.numStudents);

    VendingMachine **vendingMachines = new VendingMachine*[config.numVendingMachines];
    for (unsigned int i = 0; i < config.numVendingMachines; i++) {
        vendingMachines[i] = new VendingMachine(printer, *nameServer, i, config.sodaCost, config.maxStockPerFlavour);
    }

    BottlingPlant *plant = new BottlingPlant(printer, *nameServer, config.numVendingMachines, config.maxShippedPerFlavour,
                                             config.maxStockPerFlavour, config.timeBetweenShipments);

    Student **students = new Student*[config.numStudents];
    for (unsigned int i = 0; i < config.numStudents; i++) {
        students[i] = new Student(printer, *nameServer, *office, i, config.maxPurchases);
    }

    for (unsigned int i = 0; i < config.numStudents; i++) {
        delete students[i];
    }

    delete[] students;

    delete plant;

    for (unsigned int i = 0; i < config.numVendingMachines; i++) {
        delete vendingMachines[i];
    }

    delete[] vendingMachines;

    delete nameServer;
    delete office;
    delete parent;
}