#ifndef ACTIVECONNECTION_H
#define ACTIVECONNECTION_H
#pragma once
#include <QObject>
#include "Rawsocket.h"
class ActiveConnection : public QObject
{
    Q_OBJECT
public:
    explicit ActiveConnection(QObject *parent = nullptr, CWizReadWriteSocket* hSocket = nullptr);
    int readHandler(char* inBuf, int bufLen);
    int writeHandler(const char* outBuf, int bufLen);
    void setName(std::string conName){name=conName;}
    std::string getName(){return name;}
private:
    CWizReadWriteSocket* socket;
    std::string name;
signals:
};

#endif // ACTIVECONNECTION_H
