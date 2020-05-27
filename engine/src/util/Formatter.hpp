#pragma once

#include <pch.hpp>
#include <stdexcept>
#include <sstream>

// https://stackoverflow.com/a/12262626

class cFormatter //-V690
{
public:
    cFormatter()
    {}

    ~cFormatter()
    {}

    template<typename Type>
    cFormatter& operator<<(const Type& value)
    {
        psStream << value;
        return *this;
    }

    string str() const
    {
        return psStream.str();
    }

    operator string() const
    {
        return psStream.str();
    }

    enum eConvertToString
    {
        to_str
    };

    std::string operator>>(eConvertToString)
    {
        return psStream.str();
    }

private:
    std::stringstream psStream;

    cFormatter(const cFormatter&);
};
