#pragma once

#include "define.h"
#include <vector>
#include <string>

struct adapter_info
{
    unsigned int _id;
    std::wstring _str_description;
    std::wstring _str_mac_address;
    bool _dhcp;
    std::wstring _str_dns_1;
    std::wstring _str_dns_2;
    unsigned int _recv_bytes_total;
    unsigned int _send_bytes_total;

    adapter_info()
        : _id(-1)
        , _dhcp(false)
        , _recv_bytes_total(0)
        , _send_bytes_total(0)
    {}
};

class dns_switcher
{
public:
    result initialize();
    void uninitialize();
public:
    result get_adapter_list(std::vector<adapter_info>& vec);
    result switch_adapter_dns(unsigned int adapter_id, const std::wstring& str_dns_1, const std::wstring& str_dns_2);
public:
    dns_switcher()
    {}
};