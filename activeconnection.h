#ifndef ACTIVECONNECTION_H
#define ACTIVECONNECTION_H
#pragma once
#include <QObject>
#include "portaudio.h"
#include "mutex"
#include "cwizsslsocket.h"
#include "condition_variable"
class ActiveConnection : public QObject
{
    Q_OBJECT
public:
    //Constructor
    explicit ActiveConnection(QObject *parent = nullptr, CWizSSLSocket* hSocket = nullptr);
    //Getter/setter for some private fields...
    void setName(std::string conName){name=conName;}
    std::string getName(){return name;}
    //Basic read/write for text chat
    int readHandler(char* inBuf, int bufLen);
    int writeHandler(const char* outBuf, int bufLen);
    //Method to handle the transfer of files
    int writeFile(BYTE* bArray[], int bufLen);
    int readFile(BYTE* bArray[], int bufLen);
    //These last methods are related to voice chat
    int voiceChatHandler(CWizSSLSocket* socket, std::string clientName);
    static int paCallback(const void* inputBuffer,
                          void* outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void* userData);
    void audioReader(std::vector<char>& buffer,
                     std::mutex& bufferMutex,
                     std::condition_variable& bufferCv);
    int audioTransmitter();
private:
    CWizSSLSocket* socket;
    std::string name;
    std::vector<char> audioBuffer_;
    std::mutex bufferMutex_;
    std::condition_variable bufferCv_;
signals:
};

#endif // ACTIVECONNECTION_H
