#include "activeconnection.h"

ActiveConnection::ActiveConnection(QObject *parent, CWizReadWriteSocket* hSocket)
    : QObject{parent}
{
    socket = hSocket;
}

int ActiveConnection::readHandler(char* inBuf, int bufLen){
    int iRead = 0;
    iRead = socket->Read(inBuf, bufLen, 0);
    return iRead;
}
int ActiveConnection::writeHandler(const char* outBuf, int bufLen){
    int iWrite = 0;
    iWrite = socket->Write(outBuf, bufLen);
    return iWrite;
}
