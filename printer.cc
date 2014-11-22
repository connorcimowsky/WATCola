#include "printer.h"
#include <iostream>

using namespace std;

Printer::Printer(unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers) :
    numStudents(numStudents), numVendingMachines(numVendingMachines), numCouriers(numCouriers) {
    startVendingMachineIndex = (unsigned int)Student + numStudents;
    startCourierIndex = startVendingMachineIndex + numVendingMachines;
    totalSize = startCourierIndex + numCouriers;

    states = new char[totalSize];
    firstValues = new int[totalSize];
    secondValues = new int[totalSize];

    for (unsigned int i = 0; i < totalSize; i++) {
        states[i] = ' ';
    }

    cout << "Parent\tWATOff\tNames\tTruck\tPlant\t";

    outputHeaders(Student, startVendingMachineIndex, "Stud");
    outputHeaders(startVendingMachineIndex, startCourierIndex, "Mach");
    outputHeaders(startCourierIndex, totalSize, "Cour");

    cout << endl;

    for (unsigned int i = 0; i < totalSize; i++) {
        cout << "*******\t";
    }

    cout << endl;
}

Printer::~Printer() {
    cout << "***********************" << endl;

    delete[] states;
    delete[] firstValues;
    delete[] secondValues;
}

void Printer::print(Kind kind, char state) {
    unsigned int kindIndex = (unsigned int)kind;

    print(kind, kindIndex, state);
}

void Printer::print(Kind kind, char state, int value1) {
    unsigned int kindIndex = (unsigned int)kind;

    print(kind, kindIndex, state, value1);
}

void Printer::print(Kind kind, char state, int value1, int value2) {
    unsigned int kindIndex = (unsigned int)kind;

    print(kind, kindIndex, state, value1, value2);
}

void Printer::print(Kind kind, unsigned int lid, char state) {
    unsigned int index = getIndexFromKind(kind, lid);

    flushBufferAndSetState(index, state);

    if (state == 'F') {
        flushBuffer(states[index]);
    }
}

void Printer::print(Kind kind, unsigned int lid, char state, int value1) {
    unsigned int index = getIndexFromKind(kind, lid);

    flushBufferAndSetState(index, state);

    firstValues[index] = value1;
}

void Printer::print(Kind kind, unsigned int lid, char state, int value1, int value2) {
    unsigned int index = getIndexFromKind(kind, lid);

    flushBufferAndSetState(index, state);

    firstValues[index] = value1;
    secondValues[index] = value2;
}

void Printer::outputHeaders(unsigned int startIndex, unsigned int endIndex, const char* prefix) {
    for (unsigned int i = startIndex; i < endIndex; i++) {
        cout << prefix << i << '\t';
    }
}

unsigned int Printer::getIndexFromKind(Kind kind, unsigned int lid) {
    unsigned int index = lid;

    switch(kind) {
        case Student:
            index += (unsigned int)Student;
            return index;
        case Vending:
            index += startVendingMachineIndex;
            return index;
        case Courier:
            index += startCourierIndex;
            return index;
        default:
            return index;
    }
}

/*
 * Sets the state for a task and flushes the buffer if needed.
 */
void Printer::flushBufferAndSetState(unsigned int id, char state) {
    if (states[id] != ' ') {                                // Flush the buffer if we are overwriting a state
        flushBuffer(states[id]);
    }

    states[id] = state;
}

/*
 * Flushes the buffer of states and values for all tasks. The triggerState determines whether a task finished.
 */
void Printer::flushBuffer(char triggerState) {
    for (unsigned int i = 0; i < totalSize; i++) {
        if (states[i] != ' ') {                                     // Check if there is a state to output
            flushBufferIndex(i);
        } else if (triggerState == 'F') {                           // Output dots if a task finished
            cout << "...";
        }

        cout << '\t';

        states[i] = ' ';                                            // Reset the state to empty
    }

    cout << endl;
}

void Printer::flushBufferIndex(unsigned int index) {
    cout << states[index];

    try {
        throwExcIfNoAdditionalOutput(index);
    } catch (...) {
        return;
    }

    cout << ' ' << firstValues[index];

    if(hasSecondValue(index)) {
        cout << ',' << secondValues[index];
    }
}

void Printer::throwExcIfNoAdditionalOutput(unsigned int index) {
    if (states[index] == 'F') {
        throw "";
    }

    // All types but students and vending machines have a blank start
    if(states[index] == 'S' && (index < Student || index >= startCourierIndex)) {
        throw "";
    }

    // Watcard request work call complete
    if (index == WATCardOffice && states[index] == 'W') {
        throw "";
    }

    // Bottling Plant shipment picked up
    if (index == BottlingPlant && states[index] == 'P') {
        throw "";
    }

    // Students that lost watcard
    if (index >= Student && index < startVendingMachineIndex && states[index] == 'L') {
        throw "";
    }

    // Vending machine reloading
    if (index >= startVendingMachineIndex &&
        index < startCourierIndex &&
        (states[index] == 'r' || states[index] == 'R')) {
        throw "";
    }
}

bool Printer::hasSecondValue(unsigned int index) {
    char state = states[index];
    if (state == 'D' || state == 'T' || state == 'C' || state == 'N' || state == 'd' || state == 'U' || state == 't') {
        return true;
    }

    if (state == 'S' && index >= Student && index < startVendingMachineIndex) {
        return true;
    }

    if (state == 'B' && index >= startVendingMachineIndex && index < startCourierIndex) {
        return true;
    }

    return false;
}