#ifndef PRINTER_H
#define PRINTER_H

_Monitor Printer {
public:                                             // So the private methods can see that this exists
    enum Kind {
        Parent = 0,
        WATCardOffice = 1,
        NameServer = 2,
        Truck = 3,
        BottlingPlant = 4,
        Student = 5,
        Vending,
        Courier
    };

private:
    unsigned int numStudents;
    unsigned int numVendingMachines;
    unsigned int numCouriers;
    unsigned int startVendingMachineIndex;
    unsigned int startCourierIndex;
    unsigned int totalSize;

    char* states;
    int* firstValues;
    int* secondValues;

    void outputHeaders(unsigned int startIndex, unsigned int endIndex, const char* prefix);
    unsigned int getIndexFromKind(Kind kind, unsigned int lid);
    void flushBufferAndSetState(unsigned int id, char state);
    void flushBuffer(char state);
    void flushBufferIndex(unsigned int index);
    void throwExcIfNoAdditionalOutput(unsigned int index);
    bool hasSecondValue(unsigned index);

public:
    Printer(unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers);
    ~Printer();
    void print(Kind kind, char state);
    void print(Kind kind, char state, int value1);
    void print(Kind kind, char state, int value1, int value2);
    void print(Kind kind, unsigned int lid, char state);
    void print(Kind kind, unsigned int lid, char state, int value1);
    void print(Kind kind, unsigned int lid, char state, int value1, int value2);
};

#endif