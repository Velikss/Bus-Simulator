#pragma once
#include <pch.hpp>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <thread>
#include <mutex>

class cSSLHelper
{
    static bool pbInit;
    static std::mutex * paMutex;
public:
    cSSLHelper()
    {
        Init();
    }
    ~cSSLHelper()
    {
        CleanUp();
    }

    static void Init();
    static void CleanUp();
    static unsigned long GetId()
    {
        return (unsigned long)std::hash<std::thread::id>()   (std::this_thread::get_id());
    }
    static void Lock(int mode, int n, const char* file, int line)
    {
        if(mode & CRYPTO_LOCK)
            paMutex[n].lock(); //-V108
        else
            paMutex[n].unlock(); //-V108
    }

    // Server
    static SSL_CTX* CreateServerCtx()
    {
        const SSL_METHOD* method = TLS_server_method();
        SSL_CTX* ctx = SSL_CTX_new(method);
        if ( ctx == NULL )
        {
            ERR_print_errors_fp(stderr);
            return nullptr;
        }
        return ctx;
    }

    static SSL_CTX* CreateClientCtx()
    {
        const SSL_METHOD* method = TLS_client_method();
        SSL_CTX* ctx = SSL_CTX_new(method);
        if (ctx == NULL)
        {
            ERR_print_errors_fp(stderr);
            return nullptr;
        }
        return ctx;
    }

    static bool LoadCertificate(SSL_CTX* ctx, const string& sCertFile, const string& sKeyFile)
    {
        if(!is_file_exist(sCertFile)) return false;
        if(!is_file_exist(sKeyFile)) return false;
        /* set the local certificate from CertFile */
        if ( SSL_CTX_use_certificate_file(ctx, sCertFile.c_str(), SSL_FILETYPE_PEM) <= 0 )
        {
            ERR_print_errors_fp(stderr);
            return false;
        }
        /* set the private key from KeyFile (may be the same as CertFile) */
        if ( SSL_CTX_use_PrivateKey_file(ctx, sKeyFile.c_str(), SSL_FILETYPE_PEM) <= 0 )
        {
            ERR_print_errors_fp(stderr);
            return false;
        }
        /* verify private key */
        if ( !SSL_CTX_check_private_key(ctx) )
        {
            fprintf(stderr, "Private key does not match the public certificate\n");
            return false;
        }
        return true;
    }

    static void PrintCertificates(SSL* ssl)
    {
        X509 *cert;
        char* line;

        cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
        if ( cert != NULL )
        {
            printf("Server certificate:\n");
            line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
            printf("Subject: %s\n", line);
            free(line);
            line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
            printf("Issuer: %s\n", line);
            free(line);
            X509_free(cert);
        }
        else
            printf("No certificates.\n");
    }
};

bool cSSLHelper::pbInit = false;
std::mutex* cSSLHelper::paMutex = nullptr;

void cSSLHelper::Init()
{
    if (pbInit) return;

    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();
    SSL_load_error_strings();   /* load all error messages */

    paMutex = new std::mutex[CRYPTO_num_locks()];

    CRYPTO_set_id_callback(cSSLHelper::GetId())
    CRYPTO_set_locking_callback(cSSLHelper::Lock())
}

void cSSLHelper::CleanUp()
{
    if(!paMutex) return;

    CRYPTO_set_id_callback(NULL);
    CRYPTO_set_locking_callback(NULL);
    delete[] paMutex;
    paMutex = nullptr;
}
