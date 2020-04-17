#pragma once
#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <queue>
#include <map>
#include <initializer_list>
#include <set>
#include <tuple>
#include <iterator>
#include <filesystem>
namespace fs = std::filesystem;
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <algorithm>
#include <functional>
#include <inttypes.h>
#include <limits>
#include <memory>
#include <sstream>
#ifdef __linux__
#define LINUX
#include <unistd.h>
#endif
#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

void sleep(int sleepMs)
{
#if defined(LINUX)
    usleep(sleepMs * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#elif defined(WINDOWS)
    Sleep(sleepMs);
#else
#error Unsupported Platform.
#endif
}

typedef std::string string;
typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned char byte;

void toLower(string& str)
{
    std::for_each(str.begin(), str.end(), [](char& c) {
        c = ::tolower(c);
    });
}

void toUpper(string& str)
{
    std::for_each(str.begin(), str.end(), [](char& c) {
        c = ::toupper(c);
    });
}

string toLowerR(string& str)
{
    string s = str;
    toLower(s);
    return s;
}

int toInteger(string& str, long long& value)
{
    try {
        char* end;
        value = strtoull(str.c_str(), &end, 10);
        return 0;
    }
    catch (std::exception & ex)
    {
        return -1;
    }
}

std::vector<string> split(string str, string delim) noexcept
{
    std::vector<string>result;
    while (str.size()) {
        int index = str.find(delim);
        if (index != string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + delim.size());
            if (str.size() == 0)result.push_back(str);
        }
        else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

string concat(const std::vector<string>& strings, const string& delim = "") noexcept
{
    string s;
    for (unsigned int i = 0; i < strings.size(); i++)
    {
        if (i == strings.size() - 1)
            s += strings[i];
        else
            s += strings[i] + delim;
    }
    return s;
}