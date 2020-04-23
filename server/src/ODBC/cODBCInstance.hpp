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
public:
    SQLSMALLINT sIndex;
    SQLSMALLINT sType;
    SQLULEN uiSize;
};

typedef std::map<string, std::map<string, tColumnDefintion>> tTableDefintions;

class cODBCValue
{
    void* pvValue = nullptr;
    SQLSMALLINT psType = SQL_UNKNOWN_TYPE;
public:
    cODBCValue()
        = default;

    cODBCValue(void* vValue, SQLSMALLINT sType)
    {
        this->pvValue = vValue;
        this->psType = sType;
    }

    bool GetValueInteger(SQLINTEGER* piValue)
    {
        if(psType != SQL_INTEGER) return false;
        *piValue = *(SQLINTEGER*)pvValue;
        return true;
    }

    bool GetValueChar(SQLCHAR** ppValue)
    {
        if (psType != SQL_CHAR) return false;
        *ppValue = (SQLCHAR*)pvValue;
        return true;
    }

    bool GetValueVarChar(SQLVARCHAR** ppValue)
    {
        if (psType != SQL_VARCHAR) return false;
        *ppValue = (SQLVARCHAR*)pvValue;
        return true;
    }

    ~cODBCValue()
    {
        switch (psType)
        {
        case SQL_INTEGER:
            delete (SQLINTEGER*)pvValue;
            break;
        case SQL_CHAR || SQL_VARCHAR:
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

    tTableDefintions aTables;

    bool DiscoverTables(tTableDefintions* aTables);
    bool DiscoverColumns(tTableDefintions* aTables);
public:
    bool Connect(string sConnectionString);
    bool Disconnect();
    bool Discover();
    void PrintTables();
    bool QueryExec(string sQuery, std::vector<SQLROW>* aRows);
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

bool cODBCInstance::Discover()
{
    if (!DiscoverTables(&aTables)) return false;
    if (!DiscoverColumns(&aTables)) return false;
    return true;
}

bool cODBCInstance::DiscoverTables(tTableDefintions*aTables)
{
    HSTMT stmtTable = nullptr;
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &stmtTable);

    string sShowTablesQuery = "Show tables;";
    if (SQL_SUCCESS != SQLExecDirect(stmtTable, (SQLCHAR*)sShowTablesQuery.c_str(), SQL_NTS))
    {
        SQLFreeHandle(SQL_HANDLE_STMT, stmtTable);
        return false;
    }

    while (SQL_SUCCESS == SQLFetch(stmtTable))
    {
        SQLLEN tLen = 0;
        SQLCHAR caNameBuffer[256];

        if (SQL_SUCCESS != SQLGetData(stmtTable, 1, SQL_C_CHAR, &caNameBuffer, sizeof(caNameBuffer), &tLen))
        {
            SQLFreeHandle(SQL_HANDLE_STMT, stmtTable);
            return false;
        }
        aTables->insert({string((const char *)caNameBuffer, tLen), {}});
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmtTable);

    return true;
}

bool cODBCInstance::DiscoverColumns(tTableDefintions*aTables)
{
    for (auto& [sName, aColumns] : *aTables)
    {
        string sColumnQuery = "SELECT * FROM " + sName + " LIMIT 1;";
        HSTMT stmtColumn = nullptr;
        SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &stmtColumn);

        if (SQL_SUCCESS != SQLExecDirect(stmtColumn, (SQLCHAR *) sColumnQuery.c_str(), SQL_NTS))
        {
            SQLFreeHandle(SQL_HANDLE_STMT, stmtColumn);
            return false;
        }

        SQLSMALLINT sColumns = 0;
        if (SQL_SUCCESS != SQLNumResultCols(stmtColumn, &sColumns))
        {
            SQLFreeHandle(SQL_HANDLE_STMT, stmtColumn);
            return false;
        }

        for (SQLSMALLINT i = 1; i <= sColumns; i++)
        {
            SQLCHAR caName[256];
            SQLSMALLINT iNameLength = 0;
            SQLULEN columnLength = 0;
            SQLSMALLINT columnType = SQL_UNKNOWN_TYPE;
            if (SQL_SUCCESS !=
                SQLDescribeCol(stmtColumn, i, (SQLCHAR *)&caName, sizeof(caName), &iNameLength, &columnType,
                               &columnLength, nullptr, nullptr))
            {
                SQLFreeHandle(SQL_HANDLE_STMT, stmtColumn);
                return false;
            }

            string sName = string((const char*)caName, iNameLength);
            SQLULEN sLen = 0;

            switch (columnType)
            {
                case SQL_CHAR:
                    sLen = columnLength;
                    break;
                case SQL_VARCHAR:
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

        SQLFreeHandle(SQL_HANDLE_STMT, stmtColumn);
    }
    return true;
}

bool cODBCInstance::QueryExec(string sQuery, std::vector<SQLROW>* aRows)
{
    try
    {
        HSTMT stmt = nullptr;
        if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &stmt))
        {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        if (SQL_SUCCESS != SQLExecDirect(stmt, (SQLCHAR*)sQuery.c_str(), SQL_NTS))
        {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        SQLSMALLINT sColumnCount = 0;
        if (SQL_SUCCESS != SQLNumResultCols(stmt, &sColumnCount))
        {
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
                    sLen = 1024;
                    break;
                case SQL_VARCHAR:
                    sLen = 1024;
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
                SQLLEN sLen = 0;
                auto result = SQLGetData(stmt, tColumnDef.sIndex, tColumnDef.sType, vValueBuffer, tColumnDef.uiSize, &sLen);
                if (result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO)
                {
                    extract_error("Varchar field: ", stmt, SQL_HANDLE_STMT);
                    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                    return false;
                }
                tRow.insert({ sName, new cODBCValue {vValueBuffer, tColumnDef.sType} });
            }
            aRows->push_back(tRow);
        }
        return true;
    }
    catch (std::exception& ex)
    {
        return false;
    }
}

void cODBCInstance::PrintTables()
{
    for (auto& [sName, aColumns] : aTables)
    {
        std::cout << sName << ": " << std::endl;
        for (auto& [sName, tColumnSpecs] : aColumns)
            std::cout << "\t[" << tColumnSpecs.sIndex << "]" << sName << ": " << tColumnSpecs.uiSize << std::endl;
    }
}
