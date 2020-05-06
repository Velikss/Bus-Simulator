#pragma once
#include <pch.hpp>
#include "server/src/cNetworkConnection.hpp"

typedef int SSO_STATUS;
#define cSSO_OK          0
#define cSSO_ERR         -1
#define cSSO_FAIL_INIT   -2
#define cSSO_FAIL_UPDATE -3
#define cSSO_FAIL_FINISH -4

namespace SSO
{
    // Function to create a blake2 hash of a byte sequence.
    SSO_STATUS Blake2Hash(const unsigned char *message, size_t message_len, unsigned char **digest, unsigned int *digest_len)
    {
        EVP_MD_CTX *mdctx;

        if((mdctx = EVP_MD_CTX_new()) == NULL)
            return cSSO_FAIL_INIT;

#if defined(x86_64)
        if(1 != EVP_DigestInit_ex(mdctx, EVP_blake2b512(), NULL))
#else
        if(1 != EVP_DigestInit_ex(mdctx, EVP_blake2s256(), NULL))
#endif
            return cSSO_FAIL_INIT;

        if(1 != EVP_DigestUpdate(mdctx, message, message_len))
            return cSSO_FAIL_UPDATE;

        if((*digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()))) == NULL)
            return cSSO_FAIL_UPDATE;

        if(1 != EVP_DigestFinal_ex(mdctx, *digest, digest_len))
            return cSSO_FAIL_FINISH;

        EVP_MD_CTX_free(mdctx);
        return cSSO_OK;
    }
}