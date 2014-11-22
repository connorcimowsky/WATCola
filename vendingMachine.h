#ifndef VENDINGMACHINE_H
#define VENDINGMACHINE_H

_Monitor Printer;
_Task NameServer;
class WATCard;

_Task VendingMachine {
public:
    enum Flavours {
        BluesBlackCherry = 0,
        ClassicalCreamSoda = 1,
        RockRootBeer = 2,
        JazzLime = 3
    };

private:
    enum States {
        Start = 'S',
        ReloadStart = 'r',
        ReloadEnd = 'R',
        Buy = 'B',
        Finished = 'F'
    };

    Printer &printer;
    NameServer &nameServer;

    unsigned int id;
    unsigned int sodaCost;
    unsigned int maxStockPerFlavour;
    unsigned int stock[4];

    bool isRestocking;
    bool raiseFunds;
    bool raiseStock;

    uCondition processing;

    WATCard *currentCard;
    Flavours currentFlavour;

    void main();

  public:
    _Event Funds {};                       // insufficient funds
    _Event Stock {};                       // out of stock for particular flavour
    VendingMachine(Printer &prt, NameServer &nameServer, unsigned int id, unsigned int sodaCost,
                    unsigned int maxStockPerFlavour);
    void buy(Flavours flavour, WATCard &card);
    unsigned int* inventory();
    void restocked();
    _Nomutex unsigned int cost();
    _Nomutex unsigned int getId();
};

#endif
