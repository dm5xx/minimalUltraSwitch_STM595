#include <Arduino.h>
#include "Boards.h"

#define dataPin PA4
#define clockPin PA6
#define latchPin PA5

Boards::Boards()
{
}

Boards::~Boards()
{
    delete[] (_boards);
}

void Boards::Init(byte numberOfBanks)
{
    _boards = new RelayBoard[numberOfBanks];
    _numberOfBoards = numberOfBanks;

    for(byte a = 0; a < numberOfBanks; ++a)
    {
        _boards[a].Init(a);
    }

         // set pins low
    pinMode(dataPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);

    WriteRegisters();
}

RelayBoard * Boards::GetBoardByNumber(byte boardNumber)
{
    return &_boards[boardNumber];
}

RelayBoard * Boards::GetBoards()
{
    return _boards;
}

void Boards::WriteRegisters()
{
    digitalWrite(latchPin, LOW); //ground latchPin and hold low for as long as you are transmitting   

    for(int bor = _numberOfBoards; bor--;)
    {
        auto status = _boards[bor].GetPinStatusArray();

        for(int y = 16; y--; )
        {        
            digitalWrite(dataPin, status[y]); //return the latch pin high to signal chip that it
            digitalWrite(clockPin, HIGH); //return the latch pin high to signal chip that it
            digitalWrite(clockPin, LOW); //return the latch pin high to signal chip that it
        }
    }
    
    digitalWrite(latchPin, HIGH); //return the latch pin high to signal chip that it
}
