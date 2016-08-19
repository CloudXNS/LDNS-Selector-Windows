#pragma once

#define NETWORK_INTERFACE                           "http://tools.cloudxns.net/Api/getLdns"
#define AD_IMG_URL                                  "https://www.cloudxns.net/Public/Sun/images/common/AdBanner.png"
#define AD_LINK_URL                                 "https://www.cloudxns.net/Index/index.html?channel_code=dnsselector"
#define DEFAULT_AD_LINK_URL                         "https://www.cloudxns.net"

#define DNS_PING_URL                                L"www.qq.com"

#define WM_TASK_MSG									(WM_USER + 16427)

#define LIST_ITEM_SELECTED_COLOR                    (0xFF1A86D2)

#define PING_TIMEOUT                                (0xFFFFFFFF)

#define UI_MSG_TEXT_COLOR                           (0xFF1A86D2)
#define UI_WARNING_TEXT_COLOR                       (0xFFFFB700)
#define UI_ERROR_TEXT_COLOR                         (0xFFFF0000)

#define DEFAULT_DNS_1                               L"180.76.76.76"
#define DEFAULT_DNS_2                               L"114.114.114.114"

#define NET_WORK_INTERFACE_REQUEST_MAX_TIME         (3)

#define UI_MAIN_FRAME_WIDTH                          (600)
#define UI_MAIN_FRAME_MIN_HEIGHT                     (500)
#define UI_MAIN_FRAME_MAX_HEIGHT                     (800)

enum result
{
    result_success,
    result_com_initialize_fail,
    result_main_frame_wnd_create_fail,
    result_dnsping_fail,
    result_wmi_initialize_fail,
    result_enum_adapter_fail,
    result_dns_ip_invalid,
    result_switch_dns_ip_fail,
    result_network_interface_http_request_fail,
    result_network_interface_respond_parse_fail,
    result_network_interface_respond_status_not_exist,
    result_network_interface_respond_status_not_success,
    result_network_interface_respond_data_not_exist,
    result_getiftalbe_malloc_fail,
    result_getiftalbe_fail,
    result_no_valid_dns_for_switching,
    result_no_valid_adapter_for_switching,
    result_temp_ad_uri_invalid,
    result_temp_ad_html_file_create_file
};

static const char* s_result_string[] = 
{
    "成功",
    "COM组件初始化失败",
    "主窗口创建失败",
    "ping DNS失败",
    "WMI初始化失败",
    "枚举网卡失败",
    "DNS Ip不合法",
    "切换网卡DNS失败",
    "请求DNS列表网络错误",
    "DNS列表返回数据异常",
    "DNS返回数据格式不正确",
    "服务器拒绝请求DNS数据",
    "未能从服务器获取有效DNS数据",
    "获取网卡数据统计分配内存失败",
    "获取网卡数据统计失败",
    "没有可切换的DNS",
    "没有可切换的网卡",
    "页面资源不可用",
    "临时HTML文件创建失败"
};

__inline const char* result_string(result res)
{
    return s_result_string[res];
}


#define AD_HTML_FMT "<!DOCTYPE html><html lang=\"en\"><head>	<meta charset=\"UTF-8\">	<title>test</title>	<style type=\"text/css\">	*{margin: 0;padding: 0}	html,body{		height: 100%%;		width: 100%%;		overflow: hidden;	}	img{		height: 100%%;		width: 100%%;	}	</style></head><body>	<a href=\"%s\" target=\"_blank\">		<img src=\"%s\">	</a></body></html>"