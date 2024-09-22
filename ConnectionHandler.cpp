// ConnectionHandler.cpp
#include "ConnectionHandler.h"
#include "Rawsocket.h"
#include "thread"

#define MAX_READ_BUFFER_SIZE 500
#define DEFAULT_NAME "DefaultUser"

ConnectionHandler::ConnectionHandler() {
    username = DEFAULT_NAME;
}
int ConnectionHandler::connectToPeer(std::string sIPAddress, int iPortNum){

    if(username == DEFAULT_NAME){
        emit updateUI("You must set a username first");
        return 1;
    }
    CWizReadWriteSocket* conSock = new CWizReadWriteSocket();

    //Need to convert the IP address to LPCTSTR, because rawsocket is oldschool
    std::wstring wstr = std::wstring(sIPAddress.begin(), sIPAddress.end());
    try{
        if(!conSock->Connect(wstr.c_str(), iPortNum)){
            emit updateUI("Failed to connect to " + QString::number(iPortNum) + " at " + QString::fromStdString(sIPAddress));
            return 1;
        }
    }
    catch(...){
        emit updateUI("Some error occured when attempting to connect");
    }
    //First we write our name to the newly connected peer
    conSock->Write(username.c_str(), username.length());
    //And then we read their name
    char inBuf[50]{};
    int iRead = 0;
    while(iRead == 0){
        //iRead = recv(sock, inBuf, 50, 0);
        iRead = conSock->Read(inBuf, 50, 0);
    }
    std::string clientName = (std::string)inBuf;
    emit updateUI(QString::fromStdString("Connected to client: " + clientName));
    std::thread conThread(&ConnectionHandler::handleConnection, this, conSock, clientName);
    conThread.detach();
    emit updateClientList(QString::fromStdString(clientName));

    return 0;
}
int ConnectionHandler::listenThread() {
    WSADATA wsaData;
    int iResult;
    char readBuf[10] = {};
    int iRead = 0;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        emit updateUI("Failed to initialize Winsock");
        return 1;
    }

    // Create listening socket
    CWizSyncSocket* serversocket = new CWizSyncSocket(17590, SOCK_STREAM);
    emit updateUI("Listening for incoming connections on port 17590...");

    while (true) {
        SOCKET sock = serversocket->Accept();
        if (sock == INVALID_SOCKET) {
            emit updateUI("Error accepting connection");
            continue;
        }

        CWizReadWriteSocket* socket = new CWizReadWriteSocket();
        if (WSAGetLastError() != 0) {
            emit updateUI("Socket error occurred");
            continue;
        }

        socket->SetSocket(sock);
        emit updateUI("Connection established with client");
        //first we send our username
        socket->Write(username.c_str(), username.length());
        //then we read the initial message, which should contain client info
        char inBuf[50]{};
        int iRead = 0;
        while(iRead == 0){
            //iRead = recv(sock, inBuf, 50, 0);
            iRead = socket->Read(inBuf, 50, 0);
        }
        std::string clientName = (std::string)inBuf;
        emit updateUI(QString::fromStdString("Connected to client: " + clientName));
        std::thread conThread(&ConnectionHandler::handleConnection, this, socket, clientName);
        conThread.detach();
        emit updateClientList(QString::fromStdString(clientName));
        //ActiveConnection* connection = new ActiveConnection();
    }
}

void ConnectionHandler::setUsername(std::string name){
    username = name;
}

int ConnectionHandler::handleConnection(CWizReadWriteSocket* socket, std::string clientName){
    ActiveConnection* connection = new ActiveConnection(this, socket);
    //Add this connection to the map
    conMap[clientName] = connection;
    connection->setName(clientName);
    //now that this is done, we can stay here and read any message comming to the socket
    while(true){
        int iRead = 0;
        char inBuf[MAX_READ_BUFFER_SIZE]{};
        iRead = connection->readHandler(inBuf, MAX_READ_BUFFER_SIZE);
        if(iRead > 0){
            //and update the UI with anything read.
            emit updateUI("[" + QString::fromStdString(clientName) + "]: " + QString::fromStdString(inBuf));
        }

        else if(!WSAGetLastError()){
            emit updateUI("Connection dropped. Error: " + QString::fromStdString(GetLastSocketErrorText().c_str()));
        }
    }
}
