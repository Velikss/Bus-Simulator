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
        if (psType != SQL_CHAR) return false;
        sValue = string((const char*)pvValue, puiLen);
        return true;
    }

    ~cODBCValue()
    {
        switch (psType)
        {
        case SQL_INTEGER:
            delete (SQLINTEGER*)pvValue;
            break;
        case SQL_CHAR:
            delete[] (SQLCHAR*)pvValue;
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
            char* fn,
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
                printf("%s:%ld:%ld:%s\n", state, i, native, text);
        } while (ret == SQL_SUCCESS);
    }
public:
    bool Connect(string sConnectionString);
    bool Disconnect();
    bool Fetch(string sQuery, std::vector<SQLROW>* aRows);
    bool Exec(string sQuery);
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
    SQLRETURN result = SQLDriverConnectA( hdbc,    /* Connection handle */
                      0,                     /* Window handle */
                      (SQLCHAR*) sConnectionString.c_str(), /* Connection string */
                      SQL_NTS,               /* This is a null-terminated string */
                      (SQLCHAR *)NULL,       /* Output (result) connection string */
                      SQL_NTS,               /* This is a null-terminated string */
                      0,                     /* Length of output connect string */
                      SQL_DRIVER_NOPROMPT ); /* Don’t display a prompt window */

    return (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO);
}

bool cODBCInstance::Disconnect()
{
    /* Disconnect from the database. */
    SQLRETURN result = SQLDisconnect(hdbc);

    /* Free the connection handle. */
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);

    /* Free the environment handle. */
    SQLFreeHandle(SQL_HANDLE_ENV, henv);

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
                    sLen = columnLength;
                    break;
                case SQL_VARCHAR:
                    columnType = SQL_CHAR;
                    sLen = columnLength;
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
                case SQL_TIMESTAMP:
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
                memset(vValueBuffer, ' ', tColumnDef.uiSize);
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

bool cODBCInstance::Exec(string sQuery)
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
        return true;
    }
    catch (std::exception& ex)
    {
        extract_error("Execute: ", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }
}
