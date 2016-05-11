#include "dns_selector.h"
#include "ui.h"
#include <map>
#include <tuple>

result dns_selector::initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    result res = result_success;
    do
    {
        res = ui::instance()->initialize();
        if (res != result_success)
        {
            break;
        }
        res = ui::instance()->startup(this, hInstance, hPrevInstance, lpCmdLine, nCmdShow);
        if (res != result_success)
        {
            break;
        }
        res = result_success;
    } while (false);
    return res;
}

void dns_selector::uninitialize()
{
    ui::instance()->uninitialize();
}

result dns_selector::fetch_dns_info(std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::wstring>>& map_dns_info)
{
    return m_ip_fetcher.fetch(map_dns_info);
}

result dns_selector::ping_time(const std::wstring& str_ip, unsigned int& t_ms)
{
    return m_dns_ping.ping(str_ip, t_ms);
}

result dns_selector::get_adapter_list(std::vector<adapter_info>& vec)
{
    return m_dns_switcher.get_adapter_list(vec);
}

result dns_selector::switch_adapter_dns(unsigned int adapter_id, const std::wstring& str_dns_ip_1, const std::wstring& str_dns_ip_2)
{
    return m_dns_switcher.switch_adapter_dns(adapter_id, str_dns_ip_1, str_dns_ip_2);
}

result dns_selector::auto_select_dns()
{
    std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::wstring>> map_dns_info;

    ui::instance()->showlog("开始自动优化DNS...");

    result res = result_success;
    for (auto n = 0; n != NET_WORK_INTERFACE_REQUEST_MAX_TIME; n++)
    {
        map_dns_info.clear();
        res = fetch_dns_info(map_dns_info);
        if (res == result_success)
        {
            break;
        }
        if (res != result_network_interface_http_request_fail)
        {
            return res;
        }
        ui::instance()->showwarning("请求DNS列表失败-->[网络异常]，重试[%d/%d次]后将自动切换成公共DNS再次拉取...", n + 1, NET_WORK_INTERFACE_REQUEST_MAX_TIME);
        Sleep(1000);
    }
    if (res != result_success)
    {
        do 
        {
            if (res != result_network_interface_http_request_fail)
            {
                break;
            }
            ui::instance()->showlog("正在切换公共DNS...");

            ui::instance()->showlog("检测可以切换的网卡...");
            std::vector<adapter_info> vec;
            res = get_adapter_list(vec);
            if (res != result_success)
            {
                break;
            }
            if (vec.empty())
            {
                res = result_no_valid_adapter_for_switching;
                break;
            }

            ui::instance()->showlog("切换网卡[%ws]到公共DNS...", vec[0]._str_description.c_str());
            res = switch_adapter_dns(vec[0]._id, DEFAULT_DNS_1, DEFAULT_DNS_2);
            if (res != result_success)
            {
                break;
            }

            ui::instance()->showlog("切换公共DNS成功，重新请求DNS列表...");
            res = fetch_dns_info(map_dns_info);
            if (res != result_success)
            {
                break;
            }
            res = result_success;
        } while (false);
        if (res != result_success)
        {
            return res;
        }
    }

    unsigned int dns_count = 0;
    for (auto it : map_dns_info)
    {
        for (auto itx : it.second)
        {
            dns_count++;
        }
    }
    ui::instance()->showlog("获取到[%d个]DNS，开始测速...", dns_count);

    std::vector<std::tuple<unsigned int, std::wstring>> vec_tp_ping_t_ms_vip;
    for (auto it : map_dns_info)
    {
        std::wstring str_view_name = it.first;
        unsigned int time_out = 9999999;
        for (auto it_item : it.second)
        {
            std::wstring str_vip = it_item.first;
            std::wstring str_uptime = it_item.second;

            unsigned int time_ms;
            ui::instance()->showlog("正在测速 [%ws]...", str_vip.c_str());
            if (result_success == ping_time(str_vip, time_ms))
            {
                ui::instance()->showlog("ping [%ws]-->[%ws]", str_vip.c_str(), (std::to_wstring(time_ms) + L"ms").c_str());
            }
            else
            {
                time_ms = time_out;
                ui::instance()->showwarning("ping [%ws] 超时", str_vip.c_str());
            }
            vec_tp_ping_t_ms_vip.push_back(std::make_tuple(time_ms, str_vip));
        }
    }

    ui::instance()->showlog("正在选择最优DNS...");

    std::sort(vec_tp_ping_t_ms_vip.begin(), vec_tp_ping_t_ms_vip.end(),
        [](const std::tuple<unsigned int, std::wstring>& l, const std::tuple<unsigned int, std::wstring>& r)->bool
    {
        return std::get<0>(l) < std::get<0>(r);
    }
    );

    std::wstring str_pre_switch_dns_1;
    std::wstring str_pre_switch_dns_2;

    res = result_no_valid_dns_for_switching;
    do
    {
        if (vec_tp_ping_t_ms_vip.size() == 0)
        {
            break;
        }
        unsigned int ping_t_ms = std::get<0>(vec_tp_ping_t_ms_vip[0]);
        if (ping_t_ms == PING_TIMEOUT)
        {
            break;
        }
        str_pre_switch_dns_1 = std::get<1>(vec_tp_ping_t_ms_vip[0]);
        ui::instance()->showlog("最优DNS为[%ws]...", str_pre_switch_dns_1.c_str());
        if (vec_tp_ping_t_ms_vip.size() > 1)
        {
            unsigned int ping_t_ms = std::get<0>(vec_tp_ping_t_ms_vip[1]);
            if (ping_t_ms != PING_TIMEOUT)
            {
                str_pre_switch_dns_2 = std::get<1>(vec_tp_ping_t_ms_vip[1]);
                ui::instance()->showlog("最优备用DNS为[%ws]...", str_pre_switch_dns_2.c_str());
            }
        }
        if (str_pre_switch_dns_2.empty())
        {
            ui::instance()->showwarning("未能找到最优备用DNS，将使用原始DNS代替...");
        }
        res = result_success;
    } while (false);
    if (res != result_success)
    {
        return res;
    }

    ui::instance()->showlog("正在检测可切换的网卡...");

    std::vector<adapter_info> vec_adapter_info;
    res = get_adapter_list(vec_adapter_info);
    if (res != result_success)
    {
        return res;
    }

    if (vec_adapter_info.empty())
    {
        return result_no_valid_adapter_for_switching;
    }
    std::wstring str_pre_switch_adapter_description = vec_adapter_info[0]._str_description;
    std::wstring str_pre_switch_adapter_dns_1 = vec_adapter_info[0]._str_dns_1;
    std::wstring str_pre_switch_adapter_dns_2 = vec_adapter_info[0]._str_dns_2;

    ui::instance()->showlog("检测到可切换的网卡[%ws]-->DNS[%ws] 备用DNS[%ws]...", 
        str_pre_switch_adapter_description.c_str(), str_pre_switch_adapter_dns_1.c_str(), str_pre_switch_adapter_dns_2.c_str());

    unsigned int pre_switch_adapter_id = vec_adapter_info[0]._id;

    if (str_pre_switch_dns_2.empty())
    {
        adapter_info& info = vec_adapter_info[0];
        do
        {
            if (info._dhcp)
            {
                if (str_pre_switch_dns_1 != DEFAULT_DNS_1)
                {
                    str_pre_switch_dns_2 = DEFAULT_DNS_1;
                    ui::instance()->showlog("未能找到最优备用DNS，使用默认DNS[%ws]作为备用DNS...", str_pre_switch_dns_2.c_str());
                    break;
                }
                if (str_pre_switch_dns_1 != DEFAULT_DNS_2)
                {
                    str_pre_switch_dns_2 = DEFAULT_DNS_2;
                    ui::instance()->showlog("未能找到最优备用DNS，使用默认备用DNS[%ws]作为备用DNS...", str_pre_switch_dns_2.c_str());
                    break;
                }
                Assert(false);
            }
            if (info._str_dns_1 != str_pre_switch_dns_1)
            {
                str_pre_switch_dns_2 = info._str_dns_1;
                ui::instance()->showlog("未能找到最优备用DNS，使用原始DNS[%ws]作为备用DNS...", str_pre_switch_dns_2.c_str());
                break;
            }
            if (info._str_dns_2 != str_pre_switch_dns_1)
            {
                str_pre_switch_dns_2 = info._str_dns_2;
                ui::instance()->showlog("未能找到最优备用DNS，继续使用原始备用DNS[%ws]作为备用DNS...", str_pre_switch_dns_2.c_str());
                break;
            }
        } while (false);
    }

    
    ui::instance()->showlog("开始切换DNS_1[%ws] DNS_2[%ws] --> 网卡[%ws]...",
        str_pre_switch_dns_1.c_str(), str_pre_switch_dns_2.c_str(), str_pre_switch_adapter_description.c_str());

    res = switch_adapter_dns(pre_switch_adapter_id, str_pre_switch_dns_1, str_pre_switch_dns_2);
    if (res != result_success)
    {
        return res;
    }

    return result_success;
}



result dns_selector::on_initialize()
{
    result res = result_success;
    do
    {
        res = m_ip_fetcher.initialize();
        if (res != result_success)
        {
            break;
        }
        res = m_dns_ping.initialize();
        if (res != result_success)
        {
            break;
        }
        res = m_dns_switcher.initialize();
        if (res != result_success)
        {
            break;
        }
        res = result_success;
    } while (false);
    return res;
}

result dns_selector::on_auto_done(std::wstring& _str_auto_done_take_time)
{
    DWORD tick_count = GetTickCount();
    result res = auto_select_dns();
    if (res != result_success)
    {
        return res;
    }
    tick_count = GetTickCount() - tick_count;

    _str_auto_done_take_time = std::to_wstring(tick_count) + L"ms";
    return result_success;
}

result dns_selector::on_freshen_dns_info(freshen_dns_info_result_data& res_data)
{
    result res = result_success;
    DWORD tick_count = GetTickCount();
    std::vector<std::tuple<unsigned int, ui_dns_info>> vec_tp_ping_t_ms_info;
    do
    {
        ui::instance()->showlog("正在刷新DNS列表...");
        std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::wstring>> map_dns_info;
        res = fetch_dns_info(map_dns_info);
        if (res != result_success)
        {
            break;
        }
        ui::instance()->showlog("获取到[%d个]DNS信息, 开始测速...", map_dns_info.size());

        for (auto it : map_dns_info)
        {
            std::wstring str_view_name = it.first;
            unsigned int time_out = 9999999;
            for (auto it_item : it.second)
            {
                std::wstring str_vip = it_item.first;
                std::wstring str_uptime = it_item.second;

                ui_dns_info info;
                info._str_vip = str_vip;
                info._str_uptime = str_uptime;

                unsigned int time_ms;
                ui::instance()->showlog("正在ping [%ws]...", str_vip.c_str());
                if (result_success == ping_time(str_vip, time_ms))
                {
                    info._str_ping_t_ms = std::to_wstring(time_ms) + L"ms";
                    ui::instance()->showlog("ping [%ws]-->[%ws]", str_vip.c_str(), info._str_ping_t_ms.c_str());
                }
                else
                {
                    time_ms = time_out++;
                    info._str_ping_t_ms = L"超时";
                    ui::instance()->showerror("ping [%ws] 超时", str_vip.c_str());
                }
                vec_tp_ping_t_ms_info.push_back(std::make_tuple(time_ms, info));
            }
        }
        res = result_success;
    } while (false);
    tick_count = GetTickCount() - tick_count;
    res_data._str_freshen_take_time = std::to_wstring(tick_count) + L"ms";

    std::sort(vec_tp_ping_t_ms_info.begin(), vec_tp_ping_t_ms_info.end(),
        [](const std::tuple<unsigned int, ui_dns_info>& l, const std::tuple<unsigned int, ui_dns_info>& r)->bool
    {
        return std::get<0>(l) < std::get<0>(r);
    }
    );

    for (auto tp : vec_tp_ping_t_ms_info)
    {
        res_data._vec_ip_dns_info.push_back(std::get<1>(tp));
    }
    return res;
}

result dns_selector::on_switch_dns(unsigned int adapter_id, const std::wstring& str_ip)
{
    std::wstring str_dns_2 = str_ip;
    std::vector<adapter_info> vec_adapter_info;
    if (result_success == get_adapter_list(vec_adapter_info))
    {
        do
        {
            if (vec_adapter_info.empty())
            {
                break;
            }
            auto it = std::find_if(vec_adapter_info.begin(), vec_adapter_info.end(),
                [&adapter_id](const adapter_info& r)->bool
            {
                return adapter_id == r._id;
            }
            );
            if (it == vec_adapter_info.end())
            {
                break;
            }
            adapter_info& info = *it;
            if (info._str_dns_1.empty())
            {
                break;
            }
            if (info._str_dns_1 != str_ip)
            {
                str_dns_2 = info._str_dns_1;
            }
            else
            {
                if (info._str_dns_2.empty())
                {
                    break;
                }
                if (info._str_dns_2 != str_ip)
                {
                    str_dns_2 = info._str_dns_2;
                }
            }
        } while (false);
    }
    return switch_adapter_dns(adapter_id, str_ip, str_dns_2);
}

result dns_selector::on_freshen_adapter_info(freshen_adapter_info_result_data& res_data)
{
    DWORD tick_count = GetTickCount();
    std::vector<adapter_info> vec;
    result res = get_adapter_list(vec);
    if (res != result_success)
    {
        return res;
    }
    for (auto inf : vec)
    {
        ui_adapter_info ui_info;
        ui_info._id = inf._id;
        ui_info._str_description = inf._str_description;
        ui_info._str_dns_1 = inf._dhcp ? L"自动获取" : inf._str_dns_1;
        ui_info._str_dns_2 = inf._dhcp ? L"自动获取" : inf._str_dns_2;
        res_data._vec_adapter_info.push_back(ui_info);
    }
    tick_count = GetTickCount() - tick_count;
    res_data._str_freshen_take_time = std::to_wstring(tick_count) + L"ms";

    return res;
}
