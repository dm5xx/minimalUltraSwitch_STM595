#pragma once
#include <Arduino.h>
#include "RelayBoard.h"

class Boards {
    private:
        RelayBoard * _boards;
        uint8_t _numberOfBoards;
    public:
       Boards();
        ~Boards();
        Boards(const Boards&) = delete; // prevent copy constructor to be generated - prevent object for being copied
        Boards& operator=(const Boards&) = delete; // prevent assignment constructor to be generated - prevent object for being assigned
        
       void Init(byte numberOfBoards);
       RelayBoard * GetBoardByNumber(byte boardNumber);
       RelayBoard * GetBoards();
        void WriteRegisters();
};
