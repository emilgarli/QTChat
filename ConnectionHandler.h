// ConnectionHandler.h
#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H
#pragma once
#include <QObject>
#include <winsock2.h>
#include "activeconnection.h"
#include "map"
#include "cwizsslsocket.h"
#define PRIMARY_PORT 17590
#define SECONDARY_PORT 17591

class ConnectionHandler : public QObject
{
    Q_OBJECT  // This is required to enable signals and slots in a QObject subclass

public:
    ConnectionHandler();

    int listenThread();
    int connectToPeer(std::string sIPAddress, int iPortNum, int connectionType);
    int handleConnection(CWizSSLSocket* socket, std::string clientName);
    int voiceChatHandler(CWizSSLSocket* socket, std::string clientName);
    std::map<std::string, ActiveConnection*> getConMap(){return conMap;}
    std::map<std::string, ActiveConnection*> getFileConMap(){return fileConMap;}
    void setUsername(std::string name);
    int startComs(CWizSSLSocket* conSock, int connectionType);
    int dispatchConnectionThreads(CWizSSLSocket* socket, std::string clientName, int connectionType, bool isListener);
    int handleFileTransfer(ActiveConnection* actCon, CWizSSLSocket* socket, std::string clientName);
private:
    //Map all active text connections
    std::map<std::string, ActiveConnection*> conMap;
    //Map all active voice connections
    std::map<std::string, ActiveConnection*> voiceConMap;
    //Map all active file connections
    std::map<std::string, ActiveConnection*> fileConMap;
    std::string messageToSend;
    std::string username;
    int portNumber = PRIMARY_PORT;
signals:
    void updateUI(const QString &message);
    void showImage(const QByteArray &imageData);
    void updateClientList(const QString &clientName);
    void removeFromClientList(const QString &clientName);
};

#endif // CONNECTIONHANDLER_H
