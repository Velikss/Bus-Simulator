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

struct tUser
{
    SQLINTEGER iId;
    SQLCHAR sName[256];
};

struct tColumnDefintion
{
    SQLSMALLINT iIndex;
    SQLSMALLINT uiSize;
};

typedef std::map<string, std::map<string, tColumnDefintion>> tTableDefintions;

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
    bool QueryExec(string sQuery, std::vector<tUser>* aUsers);
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
            SQLSMALLINT sLen = 0;

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

            aColumns.insert({sName, {i, sLen}});
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmtColumn);
    }
    return true;
}

bool cODBCInstance::QueryExec(string sQuery, std::vector<tUser>* aUsers)
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

        SQLSMALLINT sColumns = 0;
        if (SQL_SUCCESS != SQLNumResultCols(stmt, &sColumns))
        {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        SQLULEN columnLen = 0;
        if(SQL_SUCCESS != SQLDescribeCol(stmt, 1, nullptr, 0, nullptr, nullptr, &columnLen, nullptr, nullptr))
        {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        while (SQL_SUCCESS == SQLFetch(stmt))
        {
            tUser tUsr;
            SQLLEN tLen = 0;
            if (SQL_SUCCESS != SQLGetData(stmt, 1, SQL_C_LONG, &tUsr.iId, sizeof(SQL_C_LONG), &tLen))
            {
                SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                return false;
            }
            aUsers->push_back(tUsr);
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
            std::cout << "\t[" << tColumnSpecs.iIndex << "]" << sName << ": " << tColumnSpecs.uiSize << std::endl;
    }
}
