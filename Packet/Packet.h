#pragma once
#include <stdlib.h>
#include <Winsock.h>
#include <iostream>

class Packet
{
public:
    __int16 TransactionID;
    __int16 ProtocolID;
    __int16 Length;
    char SlaveAddress;
    char FC;
    char Data[252] = { 0 };
    Packet()
    {
        TransactionID = 0;
        ProtocolID = 0;
        Length = 0;
        FC = 0;
        SlaveAddress = 0;   
    }
    Packet(char fc, char* data)
    {
        TransactionID = 1;
        ProtocolID = 1;
        FC = fc;
        SlaveAddress = 1;
        strcpy(Data, data);
        Length = 3 * sizeof(__int16) + 2 * sizeof(char) + sizeof(Data);
    }
    char* Serialize()
    {
        char* result = new char[Length+1];
        memcpy(result, &TransactionID, sizeof(TransactionID));
        memcpy(&result[2], &ProtocolID, sizeof(ProtocolID));
        memcpy(&result[4], &Length, sizeof(Length));
        memcpy(&result[6], &SlaveAddress, sizeof(SlaveAddress));
        memcpy(&result[7], &FC, sizeof(FC));
        memcpy(&result[8], Data, sizeof(Data));
        return result;
    }
    void Deserialize(char* src)
    {
        memcpy(&TransactionID, src, sizeof(TransactionID));
        memcpy(&ProtocolID, &src[2], sizeof(ProtocolID));
        memcpy(&Length,&src[4], sizeof(Length));
        memcpy(&SlaveAddress,&src[6], sizeof(SlaveAddress));
        memcpy(&FC,&src[7], sizeof(FC));
        memcpy(&Data,&src[8], sizeof(Data));
    }
    void Print()
    {
        std::cout << "Transaction ID: " << TransactionID << std::endl << "Protocol ID: " << ProtocolID << std::endl << "Packet Length: " << Length << std::endl << "Slave Address: " << (int)SlaveAddress << std::endl << "Function Code: " << (int)FC << std::endl << "Data: " << Data << std::endl;
    }
};