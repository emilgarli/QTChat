#ifndef CWIZSSLSOCKET_H
#define CWIZSSLSOCKET_H
#pragma once
#include "Rawsocket.h"
#include <openssl/configuration.h>
#include <openssl/opensslconf.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

#if !defined(OPENSSL_SYS_WINDOWS)
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#else
#include <winsock.h>
#endif

class CWizSSLSocket : public CWizReadWriteSocket
{
public:
    CWizSSLSocket(bool bIsServer);
    int SSL_Accept();
    int SSL_Connect();
    int Read(void* inBuf, int bufLen);
    int Write(const char* outBuf, int bufLen);
    bool isOdd(int num){return num & 1;}
    void create_context();
    void configure_server_context(SSL_CTX *ctx);
    void configure_client_context(SSL_CTX *ctx);
    void SetSocket(int socket);
private:
    bool isServer = false;
    SSL_CTX* ssl_ctx = nullptr;
    SSL* ssl = nullptr;

};

#endif // CWIZSSLSOCKET_H
