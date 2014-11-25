#ifndef NAMESERVER_H
#define NAMESERVER_H

_Monitor Printer;
_Task VendingMachine;

_Task NameServer {
public:
    NameServer(Printer &prt, unsigned int numVendingMachines, unsigned int numStudents);
    ~NameServer();
    void VMregister(VendingMachine *vendingmachine);
    VendingMachine *getMachine(unsigned int id);
    VendingMachine **getMachineList();
private:
    enum States {
        Starting = 'S',
        Register = 'R',
        New      = 'N',
        Finished = 'F'
    };

    Printer &printer;

    // as defined in ConfigParms
    unsigned int numVendingMachines;
    unsigned int numStudents;

    // the number of vending machines (out of `numVendingMachines`) that have been registered
    unsigned int numRegistered;

    // an array of pointers to each registered vending machine
    VendingMachine **machineList;

    // an array of vending machine IDs associated with each student
    unsigned int *machineAssignment;

    void main();
};

#endif
