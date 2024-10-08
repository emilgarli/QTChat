#include "cwizsslsocket.h"
#include <openssl/applink.c>
#include <openssl/err.h>  // Required for OpenSSL error handling
CWizSSLSocket::CWizSSLSocket(bool bIsServer) {
    isServer = bIsServer;
    create_context();
}

void CWizSSLSocket::create_context()
{
    const SSL_METHOD* method;

    if (isServer)
        method = TLS_server_method();
    else
        method = TLS_client_method();

    ssl_ctx = SSL_CTX_new(method);
    if (ssl_ctx == NULL) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    ssl = SSL_new(ssl_ctx);
}

void CWizSSLSocket::configure_server_context(SSL_CTX* ctx)
{
    /* Set the key and cert */
    if (SSL_use_certificate_chain_file(ssl, "chain.pem") <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_use_PrivateKey_file(ssl, "server-key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

void CWizSSLSocket::configure_client_context(SSL_CTX* ctx)
{
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    if (!SSL_CTX_load_verify_locations(ctx, "ca-cert.pem", NULL)) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

void CWizSSLSocket::SetSocket(int socket) {
    m_hSocket = socket;
    if (!SSL_set_fd(ssl, socket)) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

int CWizSSLSocket::SSL_Accept() {
    configure_server_context(ssl_ctx);

    int ret = SSL_accept(ssl);
    if (ret <= 0) {
        int ssl_err = SSL_get_error(ssl, ret);  // Get the SSL error code
        unsigned long err_code;
        printf("SSL_accept failed with SSL error code: %d\n", ssl_err);

        // Get all OpenSSL errors and print them
        while ((err_code = ERR_get_error()) != 0) {
            const char* err_str = ERR_reason_error_string(err_code);
            printf("OpenSSL error: %s\n", err_str);
        }

        // Check certificate verification result
        long verify_result = SSL_get_verify_result(ssl);
        if (verify_result != X509_V_OK) {
            printf("Certificate verification failed: %s\n", X509_verify_cert_error_string(verify_result));
        }

        return -1;
    }

    return 0;
}

int CWizSSLSocket::SSL_Connect() {
    configure_client_context(ssl_ctx);

    int ret = SSL_connect(ssl);
    if (ret <= 0) {
        int ssl_err = SSL_get_error(ssl, ret);  // Get the SSL error code
        unsigned long err_code;
        printf("SSL_connect failed with SSL error code: %d\n", ssl_err);

        // Get all OpenSSL errors and print them
        while ((err_code = ERR_get_error()) != 0) {
            const char* err_str = ERR_reason_error_string(err_code);
            printf("OpenSSL error: %s\n", err_str);
        }

        // Check certificate verification result
        long verify_result = SSL_get_verify_result(ssl);
        if (verify_result != X509_V_OK) {
            printf("Certificate verification failed: %s\n", X509_verify_cert_error_string(verify_result));
        }

        return -1;
    }

    return 0;
}




int CWizSSLSocket::Read(void* inBuf, int bufLen) {
    int iRet = SSL_read(ssl, inBuf, bufLen);
    return iRet;
}

int CWizSSLSocket::Write(const char* outBuf, int bufLen) {
    return SSL_write(ssl, outBuf, bufLen);
}

