#pragma once
#include <pch.hpp>
#include <iostream>
//#define UNICODE
#if defined(WINDOWS)
#include <windows.h>
#endif
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#define SQL_RESULT_LEN 1024

namespace dt
{
    time_t TimeOfTimeStamp(SQL_TIMESTAMP_STRUCT & tTimeStamp)
    {
        time_t iLocalTime(NULL);
        tm* tLocalTime = std::localtime(&iLocalTime);

        tm tTime;
        tTime.tm_year = tTimeStamp.year - 1900;
        tTime.tm_mon = tTimeStamp.month - 1;
        tTime.tm_mday = tTimeStamp.day;
        tTime.tm_hour = tTimeStamp.hour;
        tTime.tm_min = tTimeStamp.minute;
        tTime.tm_sec = tTimeStamp.second;
        tTime.tm_isdst = tLocalTime->tm_isdst;

        return mktime(&tTime);
    }

    SQL_TIMESTAMP_STRUCT TimeToTimeStamp(std::time_t & iTime)
    {
        tm* t = gmtime(&iTime);
        SQL_TIMESTAMP_STRUCT tTimeStamp{};
        tTimeStamp.year = t->tm_year + 1900;
        tTimeStamp.month = t->tm_mon + 1;
        tTimeStamp.day = t->tm_mday;
        tTimeStamp.hour = t->tm_hour;
        tTimeStamp.minute = t->tm_min;
        tTimeStamp.second = t->tm_sec;
        return tTimeStamp;
    }

    SQL_TIMESTAMP_STRUCT Now()
    {
        time_t t = time(NULL);
        tm* now = gmtime(&t);

        SQL_TIMESTAMP_STRUCT tNow{};
        tNow.year = 1900 + now->tm_year;
        tNow.month = 1 + now->tm_mon;
        tNow.day = now->tm_mday;
        tNow.hour = now->tm_hour;
        tNow.minute = now->tm_min;
        tNow.second = now->tm_sec;
        while(tNow.month > 12)
        {
            tNow.year++;
            tNow.month-=12;
        }
        return tNow;
    }

    string to_string(const SQL_TIMESTAMP_STRUCT& tTimeStamp)
    {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << tTimeStamp.day << "-";
        ss << std::setw(2) << std::setfill('0') << tTimeStamp.month << "-";
        ss << std::setw(4) << std::setfill('0') << tTimeStamp.year << " "; //-V112
        ss << std::setw(2) << std::setfill('0') << tTimeStamp.hour << ":";
        ss << std::setw(2) << std::setfill('0') << tTimeStamp.minute << ":";
        ss << std::setw(2) << std::setfill('0') << tTimeStamp.second;
        return ss.str();
    }
}

template<class I, class E, class S>
class codecvt : public std::codecvt<I, E, S>
{
public:
    ~codecvt()
    { }
};

struct tColumnDefintion
{
    SQLSMALLINT sIndex;
    SQLSMALLINT sType;
    SQLULEN uiSize;
};

class cODBCValue
{
public:
    void* pvValue = nullptr;
    SQLSMALLINT psType = SQL_UNKNOWN_TYPE;
    SQLULEN puiLen = 0;

    cODBCValue()
        = default;

    cODBCValue(void* vValue, SQLSMALLINT sType, SQLULEN uiLen)
    {
        this->pvValue = vValue;
        this->psType = sType;
        this->puiLen = uiLen;
    }

    bool GetValueInteger(SQLINTEGER* piValue)
    {
        if(psType != SQL_INTEGER) return false;
        *piValue = *(SQLINTEGER*)pvValue;
        return true;
    }

    bool GetValueStrRef(std::string_view & sValue)
    {
        if (psType != SQL_CHAR) return false;
        sValue = std::string_view((const char*)pvValue, puiLen);
        return true;
    }

    bool GetValueStr(std::string & sValue)
    {
        if (psType != SQL_CHAR &&
            psType != SQL_TYPE_TIMESTAMP &&
            psType != SQL_INTEGER) return false;
        if (psType == SQL_TYPE_TIMESTAMP)
        {
            SQL_TIMESTAMP_STRUCT* ptTimeStamp = (SQL_TIMESTAMP_STRUCT*)pvValue;
            sValue = dt::to_string(*ptTimeStamp);
        }
        else if (psType == SQL_INTEGER)
            sValue = std::to_string(*(long*)pvValue);
        else
            sValue = string((const char*)pvValue, puiLen);
        return true;
    }

    bool GetValueTimeStamp(SQL_TIMESTAMP_STRUCT & tTimeStamp)
    {
        if (psType != SQL_TYPE_TIMESTAMP) return false;
        tTimeStamp = *(SQL_TIMESTAMP_STRUCT*)pvValue;
        return true;
    }

    ~cODBCValue()
    {
        switch (psType)
        {
            case SQL_INTEGER:
                delete (SQLINTEGER *) pvValue;
                break;
            case SQL_CHAR:
                delete[] (SQLCHAR *) pvValue;
                break;
            case SQL_TYPE_TIMESTAMP:
                delete (SQL_TIMESTAMP_STRUCT *) pvValue;
                break;
            case SQL_UNKNOWN_TYPE:
                break;
            default:
                assert(false); // Should have been a known type.
                break;
        }
    }
};

typedef std::map<string, cODBCValue*> SQLROW;

class cODBCInstance
{
    HENV henv = nullptr;
    HDBC hdbc = nullptr;

    void extract_error(
            const char* fn,
            SQLHANDLE handle,
            SQLSMALLINT type)
    {
        SQLINTEGER i = 0;
        SQLINTEGER native;
        SQLCHAR state[7];
        SQLCHAR text[256];
        SQLSMALLINT len;
        SQLRETURN ret;
        fprintf(stderr,
                "\n"
                "The driver reported the following diagnostics whilst running "
                "%s\n\n",
                fn);

        do
        {
            ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,
                                sizeof(text), &len);
            if (SQL_SUCCEEDED(ret))
                printf("%s:%d:%d:%s\n", state, i, native, text);
        } while (ret == SQL_SUCCESS);
    }
public:
    ~cODBCInstance()
    {
        Disconnect();
    }

    bool Connect(string sConnectionString);
    bool Disconnect();
    bool Fetch(string sQuery, std::vector<SQLROW>* aRows);
    bool Exec(string sQuery, SQLLEN* puiAffected = nullptr);
    bool ExecFile(string sFileName);
    static void Escape(string & sString);
};

bool cODBCInstance::Connect(string sConnectionString)
{
/*
    std::wstring_convert<codecvt<char16_t,char,std::mbstate_t>,char16_t> convert;
    std::u16string s16ConnectionString = convert.from_bytes(sConnectionString);
*/

    /* Initialize the ODBC environment handle. */
    SQLAllocHandle( SQL_HANDLE_ENV, nullptr, &henv );

    /* Set the ODBC version to version 3 (the highest version) */
    SQLSetEnvAttr( henv, SQL_ATTR_ODBC_VERSION,
                   (void *)SQL_OV_ODBC3, 0 );

    /* Allocate the connection handle. */
    SQLAllocHandle( SQL_HANDLE_DBC, henv, &hdbc );

    /* Connect to the database using the connection string. */
    SQLRETURN result = 0;

    try
    {
        result = SQLDriverConnect(hdbc,    /* Connection handle */
            0,                     /* Window handle */
            (SQLCHAR*)sConnectionString.c_str(), /* Connection string */
            SQL_NTS,               /* This is a null-terminated string */
            (SQLCHAR*)NULL,       /* Output (result) connection string */
            SQL_NTS,               /* This is a null-terminated string */
            0,                     /* Length of output connect string */
            SQL_DRIVER_NOPROMPT); /* Don’t display a prompt window */
    }
    catch (std::exception& ex)
    {
        Disconnect();
        return false;
    }

    return (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO);
}

bool cODBCInstance::Disconnect()
{
    SQLRETURN result = SQL_SUCCESS;
    if (hdbc)
    {
        try
        {
            /* Disconnect from the database. */
            result = SQLDisconnect(hdbc);
        }
        catch (std::exception& ex)
        {
            std::cout << "failed disconnect." << std::endl;
        }

        /* Free the connection handle. */
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);

        /* Free the environment handle. */
        SQLFreeHandle(SQL_HANDLE_ENV, henv);

        hdbc = nullptr;
        henv = nullptr;
    }

    return (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO);
}

bool cODBCInstance::Fetch(string sQuery, std::vector<SQLROW>* aRows)
{
    HSTMT stmt = nullptr;
    try
    {
        if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &stmt))
        {
            extract_error("Execute: ", stmt, SQL_HANDLE_STMT);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        if (SQL_SUCCESS != SQLExecDirect(stmt, (SQLCHAR*)sQuery.c_str(), SQL_NTS))
        {
            extract_error("Execute: ", stmt, SQL_HANDLE_STMT);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        SQLSMALLINT sColumnCount = 0;
        if (SQL_SUCCESS != SQLNumResultCols(stmt, &sColumnCount))
        {
            extract_error("Execute: ", stmt, SQL_HANDLE_STMT);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        std::map<string, tColumnDefintion> aColumns;

        for (SQLSMALLINT i = 1; i <= sColumnCount; i++)
        {
            SQLCHAR caName[256];
            SQLSMALLINT iNameLength = 0;
            SQLULEN columnLength = 0;
            SQLSMALLINT columnType = SQL_UNKNOWN_TYPE;

            if (SQL_SUCCESS !=
                SQLDescribeCol(stmt, i, (SQLCHAR *) &caName, sizeof(caName), &iNameLength, &columnType,
                               &columnLength, nullptr, nullptr))
            {
                SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                return false;
            }

            string sName = string((const char *) caName, iNameLength);
            SQLULEN sLen = 0;

            switch (columnType)
            {
                case SQL_CHAR:
                    sLen = columnLength + 1;
                    break;
                case SQL_VARCHAR:
                    columnType = SQL_CHAR;
                    sLen = columnLength + 1;
                    break;
                case SQL_NUMERIC:
                    sLen = sizeof(SQLNUMERIC);
                    break;
                case SQL_INTEGER:
                    sLen = sizeof(SQLINTEGER);
                    break;
                case SQL_SMALLINT:
                    sLen = sizeof(SQLSMALLINT);
                    break;
                case SQL_FLOAT:
                    sLen = sizeof(SQLFLOAT);
                    break;
                case SQL_REAL:
                    sLen = sizeof(SQLREAL);
                    break;
                case SQL_DOUBLE:
                    sLen = sizeof(SQLDOUBLE);
                    break;
                case SQL_TYPE_TIMESTAMP:
                    sLen = sizeof(SQL_TIMESTAMP_STRUCT);
                    break;
                default:
                    return false;
            }
            aColumns.insert({sName, {i, columnType, sLen}});
        }

        while (SQL_SUCCESS == SQLFetch(stmt))
        {
            SQLROW tRow;
            for (auto& [sName, tColumnDef] : aColumns)
            {
                void* vValueBuffer = malloc(tColumnDef.uiSize);
                if (tColumnDef.sType == SQL_CHAR)
                {
                    memset(vValueBuffer, ' ', tColumnDef.uiSize - 1);
                    ((char*)vValueBuffer)[tColumnDef.uiSize - 1] = '\0';
                }
                else
                    memset(vValueBuffer, ' ', tColumnDef.uiSize); //-V575
                SQLLEN sLen = 0;
                auto result = SQLGetData(stmt, tColumnDef.sIndex, tColumnDef.sType, vValueBuffer, tColumnDef.uiSize, &sLen);
                if (result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO)
                {
                    extract_error("Fetch: ", stmt, SQL_HANDLE_STMT);
                    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                    return false;
                }
                tRow.insert({ sName, new cODBCValue {vValueBuffer, tColumnDef.sType, static_cast<SQLULEN>(sLen)} });
            }
            aRows->push_back(tRow);
        }
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return true;
    }
    catch (std::exception& ex)
    {
        extract_error("Fetch: ", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }
}

bool cODBCInstance::Exec(string sQuery, SQLLEN* puiAffected)
{
    HSTMT stmt = nullptr;
    try
    {
        if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &stmt))
        {
            extract_error("Execute: ", stmt, SQL_HANDLE_STMT);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        if (SQL_SUCCESS != SQLExecDirect(stmt, (SQLCHAR*)sQuery.c_str(), SQL_NTS))
        {
            extract_error("Execute: ", stmt, SQL_HANDLE_STMT);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        if (puiAffected)
        {
            if (SQL_SUCCESS != SQLRowCount(stmt, puiAffected))
            {
                extract_error("Execute: ", stmt, SQL_HANDLE_STMT);
                SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                return false;
            }
        }
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return true;
    }
    catch (std::exception& ex)
    {
        extract_error("Execute: ", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }
}

bool cODBCInstance::ExecFile(string sFileName)
{
    std::ifstream oFileStream(sFileName);
    if (!oFileStream.is_open())
        return false;
    std::string sQuery((std::istreambuf_iterator<char>(oFileStream)),
                         std::istreambuf_iterator<char>());
    return Exec(sQuery);
}

void cODBCInstance::Escape(string & sString)
{
    size_t start_pos = 0;
    while((start_pos = sString.find('\'', start_pos)) != std::string::npos) {
        sString.insert(start_pos, "\\");
        start_pos += 2;
    }
}
