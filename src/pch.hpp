#pragma once
#include <assert.h>
#include <array>
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
// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define x86_64
#else
#define x86
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define x86_64
#else
#define x86
#endif
#endif

bool Is64Bit()
{
#if defined(x86_64)
    return true;
#else
    return false;
#endif
}

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
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

void fSleep(int sleepMs)
{
#if defined(LINUX)
    usleep(sleepMs * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#elif defined(WINDOWS)
    Sleep(sleepMs);
#else
#error Unsupported Platform.
#endif
}

#define GLFW_INCLUDE_VULKAN                 // We want to use GLFW with Vulkan
#define GLM_FORCE_RADIANS                   // Force GLM to use radians everywhere
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES  // Force GLM to use aligned types by default
#define GLM_FORCE_DEPTH_ZERO_TO_ONE         // Vulkan uses a depth range from 0 to 1, so we need GLM to do the same

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "../engine/vendor/stb_image.h"
#include "../engine/vendor/font/stb_font_arial_50_usascii.inl"
#include "../engine/vendor/tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef std::string string;
typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned char byte;
typedef std::string string;
typedef unsigned short ushort;
typedef char* cstring;

bool is_file_exist(string fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

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
        std::cout << ex.what() << std::endl;
        return -1;
    }
}

std::vector<string> split(string str, const string& delim) noexcept
{
    std::vector<string>result;
    while (str.size()) {
        size_t index = str.find(delim);
        if (index != string::npos) {
            result.emplace_back(str.substr(0, index)); //-V106
            str = str.substr(index + delim.size());
            if (str.size() == 0)result.push_back(str);
        }
        else {
            result.push_back(str);
            str.clear();
        }
    }
    return result;
}

string concat(const std::vector<string>& strings, const string& delim = "", size_t from = 0) noexcept
{
    string s;
    for (size_t i = from; i < strings.size(); i++)
    {
        if (i == strings.size() - 1)
            s += strings[i]; //-V108
        else
            s += strings[i] + delim; //-V108
    }
    return s;
}
/*
   base64.cpp and base64.h

   Copyright (C) 2004-2008 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";


static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4]; //-V112

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++) //-V112
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while((i++ < 3))
            ret += '=';

    }

    return ret;

}
std::string base64_decode(std::string const& encoded_string) {
    size_t in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3]; //-V112
    std::string ret;

    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) { //-V108
        char_array_4[i++] = encoded_string[in_]; in_++; //-V108
        if (i ==4) { //-V112
            for (i = 0; i < (unsigned char)4; i++) //-V112
                char_array_4[i] = (unsigned char)base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++) //-V112
            char_array_4[j] = 0;

        for (j = 0; j < (unsigned char)4; j++) //-V112
            char_array_4[j] = (unsigned char)base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

template<typename Base, typename T>
inline bool instanceof(const T* ptr)
{
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

#include <sstream>

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::setw(2) << std::setfill('0') << std::fixed << a_value;
    return out.str();
}
