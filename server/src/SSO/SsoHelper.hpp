#pragma once
#include <pch.hpp>
#include "server/src/NetworkConnection.hpp"

typedef int SSO_STATUS;
#define C_SSO_LOGIN_OK    3
#define C_SSO_NOHANDLE    2
#define C_SSO_DISCONNECT  1
#define C_SSO_OK          0
#define C_SSO_ERR         -1
#define C_SSO_FAIL_INIT   -2
#define C_SSO_FAIL_UPDATE -3
#define C_SSO_FAIL_FINISH -4

namespace SSO
{
    // Function to create a blake2 hash of a byte sequence.
    SSO_STATUS Blake2Hash(const unsigned char *message, size_t message_len, unsigned char **digest, unsigned int *digest_len)
    {
        EVP_MD_CTX *mdctx;

        if((mdctx = EVP_MD_CTX_new()) == NULL)
            return C_SSO_FAIL_INIT;

#if defined(x86_64)
        if(1 != EVP_DigestInit_ex(mdctx, EVP_blake2b512(), NULL))
#else
        if(1 != EVP_DigestInit_ex(mdctx, EVP_blake2s256(), NULL))
#endif
            return C_SSO_FAIL_INIT;

        if(1 != EVP_DigestUpdate(mdctx, message, message_len))
            return C_SSO_FAIL_UPDATE;

#if defined(x86_64)
        if ((*digest = (unsigned char*)OPENSSL_malloc(EVP_MD_size(EVP_sha512()))) == NULL) //-V106
#else
        if ((*digest = (unsigned char*)OPENSSL_malloc(EVP_MD_size(EVP_sha256()))) == NULL)
#endif
            return C_SSO_FAIL_UPDATE;

        if(1 != EVP_DigestFinal_ex(mdctx, *digest, digest_len))
            return C_SSO_FAIL_FINISH;

        EVP_MD_CTX_free(mdctx);
        return C_SSO_OK;
    }
}
