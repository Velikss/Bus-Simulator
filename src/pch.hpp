#pragma once
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <stack>

typedef std::string string;
typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned char byte;

namespace str
{
    template <class Container>
    void split(const std::string& str, Container& cont,
               const std::string& delims)
    {
        std::size_t current, previous = 0;
        current = str.find_first_of(delims);
        while (current != std::string::npos) {
            cont.push_back(str.substr(previous, current - previous));
            previous = current + 1;
            current = str.find_first_of(delims, previous);
        }
        cont.push_back(str.substr(previous, current - previous));
    }
}