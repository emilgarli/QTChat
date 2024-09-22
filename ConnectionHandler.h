// ConnectionHandler.h
#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H
#pragma once
#include <QObject>
#include <winsock2.h>
#include "activeconnection.h"
#include "map"


class ConnectionHandler : public QObject
{
    Q_OBJECT  // This is required to enable signals and slots in a QObject subclass

public:
    ConnectionHandler();

    // Start listening for connections
    int listenThread();
    int connectToPeer(std::string sIPAddress, int iPortNum, int connectionType);
    int handleConnection(CWizReadWriteSocket* socket, std::string clientName);
    int voiceChatHandler(CWizReadWriteSocket* socket, std::string clientName);
    std::map<std::string, ActiveConnection*> getConMap(){return conMap;}
    void setUsername(std::string name);
    int startComs(CWizReadWriteSocket* conSock, int connectionType);

private:
    std::map<std::string, ActiveConnection*> conMap;
    std::string messageToSend;
    std::string username;
signals:
    void updateUI(const QString &message);
    void updateClientList(const QString &clientName);
};

#endif // CONNECTIONHANDLER_H
