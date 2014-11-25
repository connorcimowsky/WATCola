#ifndef PRINTER_H
#define PRINTER_H

_Monitor Printer {
public:
    enum Kind {
        Parent = 0,
        WATCardOffice,
        NameServer,
        Truck,
        BottlingPlant,
        Student,
        Vending,
        Courier
    };

    Printer(unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers);
    ~Printer();
    void print(Kind kind, char state);
    void print(Kind kind, char state, int value1);
    void print(Kind kind, char state, int value1, int value2);
    void print(Kind kind, unsigned int lid, char state);
    void print(Kind kind, unsigned int lid, char state, int value1);
    void print(Kind kind, unsigned int lid, char state, int value1, int value2);
private:
    // constants as defined in ConfigParms
    unsigned int numStudents;
    unsigned int numVendingMachines;
    unsigned int numCouriers;

    unsigned int startVendingMachineIndex;    // initial vending machine index for printing column names
    unsigned int startCourierIndex;           // initial courier index for printing column names
    unsigned int totalSize;                   // total number of columns (and thus states to keep track of)

    char *states;                             // array of states for each column
    int *firstValues;                         // array of integer values associated with each state, as appropriate
    int *secondValues;                        // array of integer values associated with each state, as appropriate

    // output the headers for columns with multiple actors (e.g., Stud, Mach, Cour)
    void outputHeaders(unsigned int startIndex, unsigned int endIndex, const char* prefix);

    // get the start index for a column type (e.g., Stud, Mach, Cour)
    unsigned int getIndexFromKind(Kind kind, unsigned int lid);

    // flush the buffer to `cout` for different combinations of column and state values
    void flushBufferAndSetState(unsigned int id, char state);
    void flushBuffer(char state);
    void flushBufferIndex(unsigned int index);

    // throw an exception if necessary to indicate that no more columns should be output
    void throwExcIfNoAdditionalOutput(unsigned int index);

    // used for determining whether a delimiter should be printed after a state
    bool hasSecondValue(unsigned index);
};

#endif
