#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <thread>
#include <WinSock2.h>
#include <inaddr.h>
#include <WS2tcpip.h>
#include "src/pugixml.hpp"

#pragma comment(lib, "ws2_32.lib")

enum class StatusConnection
{
    SUCCESS, // 0
    SOCKET_INIT,
    INVAL_SOCKET,
    CONNECT_ERROR,
    TRANSFER_ERROR,
};

// Пространство имен для процедур
namespace Procedures
{
    const std::vector<std::string> listProcedures
    {
        "call tst.read_currency('036', 'po_out');\n",
        "call tst.read_currency('23', 'po_out');\n"
    };

    const std::vector<std::string> listFiles
    {
        "currency_1.xml",
        "currency_2.xml"
    };
}

class CreateReport
{
    public:
        CreateReport();

        StatusConnection sendRequest();

    private:
        pugi::xml_document m_doc;
        pugi::xml_node m_root, m_currencyNode;
};

