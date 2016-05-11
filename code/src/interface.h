#pragma once

#include <string>
#include <vector>
#include "define.h"

struct ui_dns_info
{
    std::wstring _str_vip;
    std::wstring _str_uptime;
    std::wstring _str_ping_t_ms;
};

struct freshen_dns_info_result_data
{
    std::wstring _str_freshen_take_time;
    std::vector<ui_dns_info> _vec_ip_dns_info;
};

struct ui_adapter_info
{
    unsigned _id;
    std::wstring _str_description;
    std::wstring _str_dns_1;
    std::wstring _str_dns_2;
};

struct freshen_adapter_info_result_data
{
    std::wstring _str_freshen_take_time;
    std::vector<ui_adapter_info> _vec_adapter_info;
};

class ui_notifyer
{
public:
    virtual ~ui_notifyer(){}
    virtual result on_initialize() = 0;
    virtual result on_auto_done(std::wstring& _str_auto_done_take_time) = 0;
    virtual result on_freshen_dns_info(freshen_dns_info_result_data& res_data) = 0;
    virtual result on_freshen_adapter_info(freshen_adapter_info_result_data& res_data) = 0;
    virtual result on_switch_dns(unsigned int adapter_index, const std::wstring& str_ip) = 0;
};