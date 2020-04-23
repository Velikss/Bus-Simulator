#pragma once
#include <pch.hpp>
#include <iostream>
#define UNICODE
#if defined(WINDOWS)
#include <windows.h>
#endif
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>

class cODBCInstance
{
    HENV henv = nullptr;
    HDBC hdbc = nullptr;
public:
    bool Connect(string sConnectionString);
    bool Disconnect();
};

bool cODBCInstance::Connect(string sConnectionString)
{
    std::wstring_convert<std::codecvt<char16_t,char,std::mbstate_t>,char16_t> convert;
    std::u16string s16ConnectionString = convert.from_bytes(sConnectionString);

    /* Initialize the ODBC environment handle. */
    SQLAllocHandle( SQL_HANDLE_ENV, nullptr, &henv );

    /* Set the ODBC version to version 3 (the highest version) */
    SQLSetEnvAttr( henv, SQL_ATTR_ODBC_VERSION,
                   (void *)SQL_OV_ODBC3, 0 );

    /* Allocate the connection handle. */
    SQLAllocHandle( SQL_HANDLE_DBC, henv, &hdbc );

    /* Connect to the database using the connection string. */
    SQLRETURN result = SQLDriverConnect( hdbc,    /* Connection handle */
                      0,                     /* Window handle */
                      (SQLWCHAR*) s16ConnectionString.c_str(), /* Connection string */
                      SQL_NTS,               /* This is a null-terminated string */
                      (SQLWCHAR *)NULL,       /* Output (result) connection string */
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
