#pragma once
#include <string>
#include <map>
#include <codecvt>
#ifdef __linux__
#define LINUX
#include <unistd.h>
#endif
#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

template<class I, class E, class S>
struct codecvt : std::codecvt<I, E, S>
{
    ~codecvt()
    { }
};

typedef std::string string;
typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned short ushort;
typedef char* cstring;
typedef unsigned char byte;