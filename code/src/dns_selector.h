#pragma once

#include "util.h"
#include "ip_fetcher.h"
#include "dns_switcher.h"
#include "dns_ping.h"
#include "interface.h"

class dns_selector : public singleton<dns_selector>, public ui_notifyer
{
public:
    virtual result on_initialize();
    virtual result on_auto_done(std::wstring& _str_auto_done_take_time);
    virtual result on_freshen_dns_info(freshen_dns_info_result_data& res_data);
    virtual result on_freshen_adapter_info(freshen_adapter_info_result_data& res_data);
    virtual result on_switch_dns(unsigned int adapter_id, const std::wstring& str_ip);
public:
    result initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nCmdShow);
    void uninitialize();
private:
    result fetch_dns_info(std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::wstring>>& map_dns_info);
    result ping_time(const std::wstring& str_dns_ip, unsigned int& t_ms);
    result get_adapter_list(std::vector<adapter_info>& vec);
    result switch_adapter_dns(unsigned int adapter_id, const std::wstring& str_dns_ip_1, const std::wstring& str_dns_ip_2);
private:
    result auto_select_dns();
private:
    ip_fetcher m_ip_fetcher;
    dns_switcher m_dns_switcher;
    dns_ping m_dns_ping;
private:
    dns_selector()
    {}
private:
    SINLETON_CLASS_DECLARE(dns_selector);
};