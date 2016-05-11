#pragma once

#include "define.h"
#include <string>


class dns_ping
{
public:
    result initialize();
    void uninitialize();
public:
    result ping(const std::wstring& str_dns_ip, unsigned int& t_ms);
};