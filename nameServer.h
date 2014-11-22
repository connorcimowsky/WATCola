#ifndef NAMESERVER_H
#define NAMESERVER_H

_Monitor Printer;
_Task VendingMachine;

_Task NameServer {
    enum States {
        Starting = 'S',
        Register = 'R',
        New = 'N',
        Finished = 'F'
    };

    Printer &printer;
    unsigned int numVendingMachines;
    unsigned int numStudents;
    unsigned int numRegistered;
    VendingMachine **machineList;
    unsigned int *machineAssignment;

    void main();
  public:
    NameServer(Printer &prt, unsigned int numVendingMachines, unsigned int numStudents);
    ~NameServer();
    void VMregister(VendingMachine *vendingmachine);
    VendingMachine *getMachine(unsigned int id);
    VendingMachine **getMachineList();
};

#endif
