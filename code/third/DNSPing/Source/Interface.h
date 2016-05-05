#pragma once

#include <string>

namespace DNSPingInterface
{
    bool Ping(const std::wstring& str_dns_ip, const std::wstring& str_url, unsigned int& t_ms);
};