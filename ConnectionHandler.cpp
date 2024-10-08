// ConnectionHandler.cpp
#include "ConnectionHandler.h"

#include "thread"
#include "vector"

constexpr size_t MAX_READ_BUFFER_SIZE = 500;
#define DEFAULT_NAME "DefaultUser"



ConnectionHandler::ConnectionHandler() {
    username = DEFAULT_NAME;
}

void ConnectionHandler::setUsername(std::string name){
    username = name;
}

//Helper function to delimit the given string on a given character. returns a vector with the elements
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

//Connects to the given IP and port number. connection type is: 0 = text chat, 1 = voice chat, 2 = file transfer
int ConnectionHandler::connectToPeer(std::string sIPAddress, int iPortNum, int connectionType){
    if(username == DEFAULT_NAME){
        emit updateUI("You must set a username first");
        return 1;
    }

    CWizReadWriteSocket* conSock = new CWizReadWriteSocket();
    CWizSSLSocket* sslSock = new CWizSSLSocket(false);
    //Need to convert the IP address to LPCTSTR, because rawsocket is oldschool
    std::wstring wstr = std::wstring(sIPAddress.begin(), sIPAddress.end());
    try{
        if(!conSock->Connect(wstr.c_str(), iPortNum)){
            emit updateUI("Failed to connect to " + QString::number(iPortNum) + " at " + QString::fromStdString(sIPAddress));
            return 1;
        }

        sslSock->SetSocket(conSock->H());
        sslSock->SSL_Connect();
    }
    catch(...){
        emit updateUI("Some error occured when attempting to connect");
    }
    startComs(sslSock, connectionType);
    return 0;
}
//This methods reads info about the newly connected client, and writes info about us to the client
int ConnectionHandler::startComs(CWizSSLSocket* conSock, int connectionType){
    char inBuf[50]{};
    int iRead = 0;
    std::string clientName = "";
    std::vector<std::string> peerInfoVec;
    //First we write our name to the newly connected peer
    std::string infoString = username + "," + std::to_string(connectionType);
    conSock->Write(infoString.c_str(), infoString.length());
    //And then we read the peer info

    while(iRead == 0){
        iRead = conSock->Read(inBuf, 50);
    }
    if(iRead < 0){
        emit updateUI("Failed to connect to this host");
        delete conSock;
        return -1;
    }
    peerInfoVec = delimitString(inBuf,iRead, ',');
    if(peerInfoVec.size() < 1){
        emit updateUI("Error in protocol. No valid client data received.");
    }
    //The first element is the name
    clientName = peerInfoVec.at(0);
    //0 = text chat, 1 = voice chat, 2 = file transfer
    return dispatchConnectionThreads(conSock, clientName, connectionType);
}

//Listens for incomming connections on PRIMARY_PORT or SECONDARY_PORT
//It writes and reads client info to/from any newly connected client
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

        CWizSSLSocket* socket = new CWizSSLSocket(true);

        if (WSAGetLastError() != 0) {
            emit updateUI("Socket error occurred");
            continue;
        }

        socket->SetSocket(sock);
        socket->SSL_Accept();
        emit updateUI(QString::fromStdString("Connection established with client"));
        //first we send our username
        socket->Write(username.c_str(), username.length());
        //then we read the initial message, which should contain client info
        char inBuf[50]{};
        int iRead = 0;
        while(iRead == 0){
            // inBuf = [userName,connectionType]
            iRead = socket->Read(inBuf, 50);
        }
        clientInfoVec = delimitString(inBuf,iRead, ',');
        if(clientInfoVec.size() < 2){
            emit updateUI("Error in protocol. No valid client data received.");
            delete socket;
            delete serversocket;
            return -1;
        }
        dispatchConnectionThreads(socket, clientInfoVec.at(0) ,stoi(clientInfoVec.at(1)));
    }
}

int ConnectionHandler::dispatchConnectionThreads(CWizSSLSocket* socket, std::string clientName, int connectionType){

    if(connectionType == 0){
        emit updateUI(QString::fromStdString("Text chat started with: " + clientName));
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
    else if(connectionType == 2){
        //We don't have to update the ui to tell the user that a file transfer has been initiated
        //All we do is create the instance, update the fileConMap, and we let the caller of the
        //instance in the map deal with calling writeFile with an image in BYTE format
        ActiveConnection* actCon = nullptr;
        //If this client does not exist in the file connection map
        if(fileConMap.find(clientName) == fileConMap.end()){
            actCon = new ActiveConnection(this, socket);
            fileConMap[clientName] = actCon;
        }
        //If it already exist
        else{
            actCon = fileConMap[clientName];
        }
        std::thread fileThread(&ConnectionHandler::handleFileTransfer, this, actCon, socket, clientName);
        fileThread.detach();
        return 0;
    }
    return -1;
}

int ConnectionHandler::handleConnection(CWizSSLSocket* socket, std::string clientName){
    int iRet = 0;
    ActiveConnection* connection = nullptr;
    if(conMap.find(clientName) == conMap.end()){
        connection = new ActiveConnection(this, socket);
        conMap[clientName] = connection;
    }
    //If it already exist
    else{
        connection = conMap[clientName];
    }
    //now that this is done, we can stay here and read any message comming to the socket
    while(true){
        Sleep(100);
        int iRead = 0;
        char inBuf[MAX_READ_BUFFER_SIZE]{};
        iRead = connection->readHandler(inBuf, MAX_READ_BUFFER_SIZE);
        if(iRead > 0){
            //and update the UI with anything read.
            emit updateUI("[" + QString::fromStdString(clientName) + "]: " + QString::fromStdString(inBuf));
        }
        else if(iRead < 0){
            //Client has disconnected
            emit updateUI(QString::fromStdString("Client " + clientName+ " disconnected."));
            iRet = -1;
            emit removeFromClientList(QString::fromStdString(clientName));
            conMap.erase("clientName");
            delete socket;
            delete connection;
            break;
        }
    }
    return iRet;
}

int ConnectionHandler::handleFileTransfer(ActiveConnection* actCon, CWizSSLSocket* socket, std::string clientName){
    int iRead = 0;
    char inBuf[MAX_READ_BUFFER_SIZE];
    int incImageSize = 0;
    std::string imageName = "";

    //First we read the socket for incomming image metadata
    while(iRead == 0){
        actCon->readHandler(inBuf, MAX_READ_BUFFER_SIZE);
    }
    //Now we can delimit the incomming data, and read the size of the incomming image
    std::vector<std::string> imageDataVec = delimitString(inBuf, MAX_READ_BUFFER_SIZE, ',');
    incImageSize = stoi(imageDataVec.at(0));
    imageName = imageDataVec.at(1);
    //Allocate space for the image
    BYTE* imBuf[incImageSize];
    //Now we read the socket for any incomming image data
    actCon->readFile(imBuf, incImageSize);
    QByteArray imageData(reinterpret_cast<char*>(imBuf), incImageSize);
    emit showImage(imageData);
    return 0;
}
