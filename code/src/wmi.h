#pragma once

#include <Wbemidl.h>
#include "define.h"
#include <string>
#include <vector>

struct wmi_adapter_info
{
    unsigned int _index;
    unsigned int _interface_index;
    std::wstring _str_description;
    std::wstring _str_mac_address;
    bool _dhcp;
    std::wstring _str_dns_1;
    std::wstring _str_dns_2;

    wmi_adapter_info()
        : _index(-1)
        , _interface_index(-1)
        , _dhcp(false)
    {}
};


class wmi_interface
{
public:
    static result get_adapter_list(std::vector<wmi_adapter_info>& vec);
    static result switch_adapter_dns(unsigned int index, const std::wstring& str_dns_1, const std::wstring& str_dns_2);
private:
    static result initliaze_wmi(IWbemServices* &pSvc, IWbemLocator* &pLoc);
    static void uninitialize_wmi(IWbemServices* pSvc, IWbemLocator* pLoc);
};