#include <pch.hpp>
#include <NetworkConnection.hpp>

namespace nGameConnectionHelper
{

    bool RecieveData(cNetworkConnection* pConnection, byte* buffer, int& iRecievedContent)
    {
        int iSize = 0;
        const size_t recievedSize = (size_t) pConnection->ReceiveBytes(((byte * ) & iSize), 4); //-V206 //-V112
        if (recievedSize != 4) std::cout << "didn't recieve header." << std::endl; //-V112
        buffer = new byte[iSize]; //-V121
        while (iRecievedContent != iSize)
        {
            iRecievedContent += pConnection->ReceiveBytes(buffer + iRecievedContent, iSize - iRecievedContent); //-V104
            if (iRecievedContent == -1) iRecievedContent += 1;
        }
        return true;
    }

    bool SendData(cNetworkConnection* pConnection, byte* buffer, int iSize)
    {
        if (!pConnection->SendBytes((byte * ) & iSize, 4)) return false; //-V206 //-V112
        if (!pConnection->SendBytes(buffer, iSize)) return false;
        return true;
    }
}