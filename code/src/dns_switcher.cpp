#include "dns_switcher.h"
#include "ui.h"
#include "wmi.h"
#include "performance_counter.h"
#include <algorithm>

result dns_switcher::initialize()
{
    return result_success;
}

void dns_switcher::uninitialize()
{
    
}

result dns_switcher::get_adapter_list(std::vector<adapter_info>& vec)
{
    std::vector<wmi_adapter_info> vec_wmi_adapter_info;
    result res = wmi_interface::get_adapter_list(vec_wmi_adapter_info);
    if (res != result_success)
    {
        return res;
    }
    std::vector<adapter_performance_counter> vec_adapter_pfmc_counter;
    res = performance_counter::get_adapter_performance(vec_adapter_pfmc_counter);
    if (res != result_success)
    {
        return res;
    }
    for (auto wmi_info : vec_wmi_adapter_info)
    {
        adapter_info info;
        info._id = MAKELONG(wmi_info._index, wmi_info._interface_index);
        info._str_description = wmi_info._str_description;
        info._str_mac_address = wmi_info._str_mac_address;
        info._dhcp = wmi_info._dhcp;
        info._str_dns_1 = wmi_info._str_dns_1;
        info._str_dns_2 = wmi_info._str_dns_2;

        auto it = std::find_if(vec_adapter_pfmc_counter.begin(), vec_adapter_pfmc_counter.end(),
            [&wmi_info](const adapter_performance_counter& r)->bool
        {
            return wmi_info._interface_index == r._adapter_interface_index; 
        }
        );
        if (it != vec_adapter_pfmc_counter.end())
        {
            adapter_performance_counter& pfmc_counter = *it;
            info._recv_bytes_total = pfmc_counter._recv_bytes_total;
            info._send_bytes_total = pfmc_counter._send_bytes_total;
        }
        else
        {
            info._recv_bytes_total = 0;
            info._send_bytes_total = 0;
        }
        vec.push_back(info);
    }
    std::sort(vec.begin(), vec.end(),
        [](const adapter_info& l, const adapter_info& r)->bool
    {
        return l._recv_bytes_total > r._recv_bytes_total;
    }
    );
    return result_success;
}

result dns_switcher::switch_adapter_dns(unsigned int adapter_id, const std::wstring& str_dns_1, const std::wstring& str_dns_2)
{
    unsigned int adapter_index = LOWORD(adapter_id);
    return wmi_interface::switch_adapter_dns(adapter_index, str_dns_1, str_dns_2);
}