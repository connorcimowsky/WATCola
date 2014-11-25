#include "parent.h"
#include "bank.h"
#include "printer.h"
#include "MPRNG.h"

static const unsigned int AMOUNT_LOWER_BOUND = 1;
static const unsigned int AMOUNT_UPPER_BOUND = 3;

Parent::Parent(Printer &prt, Bank &bank, unsigned int numStudents, unsigned int parentalDelay) :
    printer(prt),
    bank(bank),
    numStudents(numStudents),
    parentalDelay(parentalDelay) {
}

void Parent::main() {
    printer.print(Printer::Parent, Start);

    while (true) {
        _Accept(~Parent) {
            break;
        } _Else {
            yield(parentalDelay);

            int id = generator(numStudents - 1);
            int amount = generator(AMOUNT_LOWER_BOUND, AMOUNT_UPPER_BOUND);

            printer.print(Printer::Parent, Deposit, id, amount);
            bank.deposit(id, amount);
        }
    }

    printer.print(Printer::Parent, Finished);
}
