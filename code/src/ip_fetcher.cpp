#include "ip_fetcher.h"
#include "ui.h"

#include <cpprest/json.h>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

result ip_fetcher::initialize()
{
    return result_success;
}

void ip_fetcher::uninitialize()
{

}

result ip_fetcher::fetch(std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::wstring>>& map_dns_info)
{
    result res = result_success;
    bool request_return = false;

    http_client client(U(NETWORK_INTERFACE));
    auto query = uri_builder().to_string();
   
    ui::instance()->showlog("请求DNS列表...");
    client
        .request(methods::GET, query)
        .then([&](http_response response) -> pplx::task<json::value>
    {
        request_return = true;
        if (response.status_code() == status_codes::OK)
        {
            return response.extract_json(true);
        }
        else
        {
            ui::instance()->showerror("请求DNS列表 响应结果-->[%d]", response.status_code());
            return pplx::task_from_result(json::value());
        }
    })
        .then([&](pplx::task<json::value> previousTask)
    {
        try
        {
            json::value& resp = previousTask.get();
            if (resp.is_null())
            {
                res = result_network_interface_http_request_fail;
            }
            else
            {
                ui::instance()->showlog("开始解析DNS列表数据...");
                res = parse_respond(resp, map_dns_info);
            }
        }
        catch (http_exception const & e)
        {
            if (!request_return)
            {
                res = result_network_interface_http_request_fail;
                ui::instance()->showerror("请求DNS列表发生异常-->[%s]", e.what());
            }
            else
            {
                res = result_network_interface_respond_parse_fail;
                ui::instance()->showerror("解析DNS列表数据发生异常-->[%s]", e.what());
            }
        }
        catch (web::json::json_exception const & e)
        {
            res = result_network_interface_respond_parse_fail;
            ui::instance()->showerror("解析DNS列表数据发生异常-->[%s]", e.what());
        }
    })
        .wait();
    return res;
}


result ip_fetcher::parse_respond(json::value& resp, std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::wstring>>& map_dns_info)
{
    result res = result_success;
    do
    {
        if (resp[U("status")].is_null())
        {
            res = result_network_interface_respond_status_not_exist;
            break;
        }
        std::wstring str_status = resp[U("status")].as_string();
        if (str_status != L"success")
        {
            ui::instance()->showerror("DNS列表数据 状态码-->[%ws]", str_status.c_str());
            res = result_network_interface_respond_status_not_success;
            break;
        }
        if (resp[U("data")].is_null())
        {
            res = result_network_interface_respond_data_not_exist;
            break;
        }
        for (auto view_item : resp[U("data")].as_array())
        {
            std::wstring str_view_name;
            if (!view_item[U("view_name")].is_null())
            {
                str_view_name = view_item[U("view_name")].as_string();
            }
            else
            {
                static unsigned int s_suffix = 0;
                str_view_name = L"unknow" +std::to_wstring(++s_suffix);
                ui::instance()->showwarning("DNS列表数据 [view_name]为空");
            }
            if (view_item[U("info")].is_null())
            {
                ui::instance()->showwarning("DNS列表数据 [info]为空");
                continue;
            }
            std::unordered_map<std::wstring, std::wstring> map_vip_uptime;
            for (auto ip_item : view_item[U("info")].as_array())
            {
                if (ip_item[U("vip")].is_null())
                {
                    ui::instance()->showwarning("DNS列表数据 [vip]为空");
                    continue;
                }
                std::wstring str_vip;
                str_vip = ip_item[U("vip")].as_string();

                std::wstring str_uptime;
                if (!ip_item[U("uptime")].is_null())
                {
                    str_uptime = ip_item[U("uptime")].as_string();
                }
                else
                {
                    str_uptime = L"unknow";
                    ui::instance()->showwarning("DNS列表数据 [uptime]为空");
                }
                map_vip_uptime.insert(std::make_pair(str_vip, str_uptime));
            }
            map_dns_info.insert(std::make_pair(str_view_name, map_vip_uptime));
        }
        res = result_success;
    } while (false);

    return res;
}