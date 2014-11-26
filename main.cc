#include "config.h"
#include "printer.h"
#include "bank.h"
#include "parent.h"
#include "watCardOffice.h"
#include "nameServer.h"
#include "vendingMachine.h"
#include "bottlingPlant.h"
#include "student.h"
#include "MPRNG.h"

#include <iostream>
#include <sstream>

MPRNG generator;

using namespace std;

bool convert (int &val, char *buffer) {
    std::stringstream ss(buffer);
    ss >> dec >> val;
    return ! ss.fail() && string(buffer).find_first_not_of(" ", ss.tellg()) == string::npos;
}

void usage(char *argv[]) {
    cerr << "Usage: " << argv[0]
         << " [ config-file [ random-seed (> 0) ] ]"
         << endl;
    exit(EXIT_FAILURE);
}

void uMain::main() {
    int seed = getpid();
    const char *configFile = "soda.config";
    ConfigParms config;

    switch (argc) {
        case 3:
            if (!convert(seed, argv[2]) || seed <= 0) {
                usage(argv);
            }
        case 2:
            configFile = argv[1];
        case 1:
            break;
        default:
            usage(argv);
    }

    generator.seed(seed);
    processConfigFile(configFile, config);

    Printer printer(config.numStudents, config.numVendingMachines, config.numCouriers);
    Bank bank(config.numStudents);
    Parent parent(printer, bank, config.numStudents, config.parentalDelay);
    WATCardOffice office(printer, bank, config.numCouriers);
    NameServer nameServer(printer, config.numVendingMachines, config.numStudents);

    VendingMachine *vendingMachines[config.numVendingMachines];
    for (unsigned int i = 0; i < config.numVendingMachines; i++) {
        vendingMachines[i] = new VendingMachine(printer, nameServer, i, config.sodaCost, config.maxStockPerFlavour);
    }

    BottlingPlant *plant = new BottlingPlant(printer, nameServer, config.numVendingMachines,
                                   config.maxShippedPerFlavour, config.maxStockPerFlavour, config.timeBetweenShipments);

    Student *students[config.numStudents];
    for (unsigned int i = 0; i < config.numStudents; i++) {
        students[i] = new Student(printer, nameServer, office, i, config.maxPurchases);
    }

    for (unsigned int i = 0; i < config.numStudents; i++) {
        delete students[i];
    }

    delete plant;

    for (unsigned int i = 0; i < config.numVendingMachines; i++) {
        delete vendingMachines[i];
    }
}
