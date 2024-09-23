// ConnectionHandler.cpp
#include "ConnectionHandler.h"
#include "Rawsocket.h"
#include "thread"
#include "vector"

#define MAX_READ_BUFFER_SIZE 500
#define DEFAULT_NAME "DefaultUser"



ConnectionHandler::ConnectionHandler() {
    username = DEFAULT_NAME;
}

void ConnectionHandler::setUsername(std::string name){
    username = name;
}
static std::vector<std::string> delimitString(const char* buffer, int bufLen, char delimit) {
    std::vector<std::string> vRet;
    std::string tempChar = "";

    for (int i = 0; i < bufLen; i++) {
        if (buffer[i] != delimit) {
            tempChar += buffer[i]; // Append character to tempChar
        }
        else {
            if (!tempChar.empty()) {
                vRet.push_back(tempChar); // Store the current string in the vector
                tempChar = ""; // Reset tempChar for the next substring
            }
        }
    }

    // Push the last substring (if any) after the loop
    if (!tempChar.empty()) {
        vRet.push_back(tempChar);
    }

    return vRet; // Return the vector of substrings
}
int ConnectionHandler::connectToPeer(std::string sIPAddress, int iPortNum, int connectionType){
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
    startComs(conSock, connectionType);
    return 0;
}

int ConnectionHandler::startComs(CWizReadWriteSocket* conSock, int connectionType){
    char inBuf[50]{};
    int iRead = 0;
    std::string clientName = "";
    std::vector<std::string> peerInfoVec;
    //First we write our name to the newly connected peer
    std::string infoString = username + "," + std::to_string(connectionType);
    conSock->Write(infoString.c_str(), infoString.length());
    //And then we read the peer info

    while(iRead == 0){
        iRead = conSock->Read(inBuf, 50, 0);
    }
    peerInfoVec = delimitString(inBuf,iRead, ',');
    //The first element is the name
    clientName = peerInfoVec.at(0);
    //0 = text chat, 1 = voice chat, 2 = file transfer
    return dispatchConnectionThreads(conSock, clientName, connectionType);
}


int ConnectionHandler::listenThread() {
    WSADATA wsaData;
    int iResult;
    char readBuf[10] = {};
    int iRead = 0;
    CWizSyncSocket* serversocket = nullptr;
    std::vector<std::string> clientInfoVec;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        emit updateUI("Failed to initialize Winsock");
        return 1;
    }

    // Create listening socket for chat
    serversocket = new CWizSyncSocket(PRIMARY_PORT, SOCK_STREAM);
    if (WSAGetLastError() != 0) {
        emit updateUI("Unable to bind socket to port " + QString::number(PRIMARY_PORT));
        emit updateUI("Attempting backup listenport " + QString::number(SECONDARY_PORT));
        serversocket = new CWizSyncSocket(SECONDARY_PORT, SOCK_STREAM);
        portNumber = SECONDARY_PORT;
    }
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
            iRead = socket->Read(inBuf, 50, 0);
        }
        clientInfoVec = delimitString(inBuf,iRead, ',');
        dispatchConnectionThreads(socket, clientInfoVec.at(0), stoi(clientInfoVec.at(1)));
    }
}

int ConnectionHandler::dispatchConnectionThreads(CWizReadWriteSocket* socket, std::string clientName, int connectionType){

    if(connectionType == 0){
        emit updateUI(QString::fromStdString("Connected to client: " + clientName));
        std::thread conThread(&ConnectionHandler::handleConnection, this, socket, clientName);
        conThread.detach();
        emit updateClientList(QString::fromStdString(clientName));
        return 0;
    }
    else if(connectionType == 1){
        emit updateUI("Voice chat incomming from " + QString::fromStdString(clientName));
        ActiveConnection* actCon = new ActiveConnection();
        voiceConMap[clientName] = actCon;
        std::thread conThread(&ActiveConnection::voiceChatHandler, actCon, socket, clientName);
        conThread.detach();
        return 0;
    }
    return -1;
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
