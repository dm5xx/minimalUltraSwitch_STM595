#include <Arduino.h>
#include "RelayBoard.h"

//#define LDEBUG

RelayBoard::RelayBoard() : _status(0), _pinStatus{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, _RelayPins { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 }
{
}


void RelayBoard::Init(byte boardnumber)
{
#ifdef LDEBUG
    Serial.print("Init boardnumber: ");
    Serial.println(boardnumber);  // just for test  
#endif

    _boardNumber = boardnumber;    
    Serial.print("After Init boardnumber: ");
    Serial.println(_boardNumber);  // just for test  
}

unsigned int RelayBoard::GetStatus() {

#ifdef LDEBUG
    Serial.print("GetStatus: ");
    Serial.print(_status);  // just for test  
    Serial.print(" GetOnBoard: ");
    Serial.println(_boardNumber);  // just for test  
#endif

    return _status;
}

byte * RelayBoard::GetPinStatusArray() {
    return _pinStatus;
}

void RelayBoard::SetStatus(unsigned int value)
{

#ifdef LDEBUG
    Serial.print("SetOnBoard: ");
    Serial.println(_boardNumber);  // just for test  
    Serial.print(" SetStatus befor: ");
    Serial.println(_status);
#endif

    _status = value;

#ifdef LDEBUG
    Serial.print(" Status after: ");
    Serial.println(_status);
#endif

    GetOrderedArraybyValue(_status, _pinStatus);
}

void RelayBoard::GetOrderedArraybyValue(unsigned int value, byte * feld)
{
    int i;

    for (i = 0; i < 16; i++)
    {
        feld[i] = value % 2;
        value /= 2;
    }
}

unsigned int RelayBoard::GetValueByOrderedArray(byte * arr)
{
    unsigned int result = 0;
        
    for(uint8_t a = 0; a < 16; a++)
    {
            result = result + (arr[a] * 1<<a);
    }

    return result;
}