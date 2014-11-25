#ifndef VENDINGMACHINE_H
#define VENDINGMACHINE_H

_Monitor Printer;
_Task NameServer;
class WATCard;

_Task VendingMachine {
public:
    enum Flavours {
        BluesBlackCherry   = 0,
        ClassicalCreamSoda = 1,
        RockRootBeer       = 2,
        JazzLime           = 3,
        NumberOfFlavours   = 4
    };

    _Event Funds {};    // inserted WATCard has insufficient funds
    _Event Stock {};    // machine is out of stock for the desired flavour

    VendingMachine(Printer &prt, NameServer &nameServer, unsigned int id, unsigned int sodaCost,
        unsigned int maxStockPerFlavour);
    void buy(Flavours flavour, WATCard &card);
    unsigned int* inventory();    // the current stock of each soda flavour
    void restocked();             // called by the delivery truck when it has finished restocking the machine

    // getters for `sodaCost` and `id`, respectively
    _Nomutex unsigned int cost();
    _Nomutex unsigned int getId();
private:
    enum States {
        Start       = 'S',
        ReloadStart = 'r',
        ReloadEnd   = 'R',
        Buy         = 'B',
        Finished    = 'F'
    };

    Printer &printer;
    NameServer &nameServer;

    unsigned int id;    // the ID of this vending machine

    // constants as defined in ConfigParms
    unsigned int sodaCost;
    unsigned int maxStockPerFlavour;

    unsigned int stock[NumberOfFlavours];    // the current stock of each soda flavour

    bool isRestocking;    // used for determining whether we should block users of the vending machine

    // used for determining when we should raise the `Funds` and `Stock` events, respectively
    bool raiseFunds;
    bool raiseStock;

    // used for synchronizing users of the vending machine
    uCondition processing;

    WATCard *currentCard;       // keep a pointer to the WATCard currently being used in the machine
    Flavours currentFlavour;    // keep track of the flavour currently being dispensed

    void main();
};

#endif
