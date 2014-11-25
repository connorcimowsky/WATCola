#include "printer.h"
#include <iostream>

using namespace std;

Printer::Printer(unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers) :
    numStudents(numStudents),
    numVendingMachines(numVendingMachines),
    numCouriers(numCouriers),
    startVendingMachineIndex((unsigned int)Student + numStudents),
    startCourierIndex(startVendingMachineIndex + numVendingMachines),
    totalSize(startCourierIndex + numCouriers),
    states(new char[totalSize]),
    firstValues(new int[totalSize]),
    secondValues(new int[totalSize]) {

    // initialize states
    for (unsigned int i = 0; i < totalSize; i++) {
        states[i] = ' ';
    }

    // print fixed column headers
    cout << "Parent\tWATOff\tNames\tTruck\tPlant\t";

    // print dynamically-numbered column headers
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

    if (state == 'F') {
        for (unsigned int i = 0; i < totalSize; i++) {
            if (states[i] != ' ') {
                flushBuffer(' ');
                break;
            }
        }

        states[index] = 'F';
        flushBuffer(state);
    } else {
        flushBufferAndSetState(index, state);
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
    for (unsigned int i = startIndex, j = 0; i < endIndex; i++, j++) {
        cout << prefix << j << '\t';
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

// set the state for a task and flush the buffer if needed
void Printer::flushBufferAndSetState(unsigned int id, char state) {
    if (states[id] != ' ') {
        // flush the buffer if we are overwriting a state
        flushBuffer(state);
    }

    states[id] = state;
}

// flush the buffer of states and values for all tasks; the `triggerState` determines whether a task finished
void Printer::flushBuffer(char triggerState) {
    for (unsigned int i = 0; i < totalSize; i++) {
        if (states[i] != ' ') {              // check if there is a state to output
            flushBufferIndex(i);
        } else if (triggerState == 'F') {    // output dots if a task finished
            cout << "...";
        }

        cout << '\t';

        states[i] = ' ';                     // reset the state to empty
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

    if (hasSecondValue(index)) {
        cout << ',' << secondValues[index];
    }
}

void Printer::throwExcIfNoAdditionalOutput(unsigned int index) {
    if (states[index] == 'F') {
        throw "";
    }

    // all types except for students and vending machines have a blank start
    if (states[index] == 'S' && (index < Student || index >= startCourierIndex)) {
        throw "";
    }

    // watcard request work call is complete
    if (index == WATCardOffice && states[index] == 'W') {
        throw "";
    }

    // bottling plant shipment has been picked up
    if (index == BottlingPlant && states[index] == 'P') {
        throw "";
    }

    // students who lost their watcard
    if (index >= Student && index < startVendingMachineIndex && states[index] == 'L') {
        throw "";
    }

    // vending machine is reloading
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
