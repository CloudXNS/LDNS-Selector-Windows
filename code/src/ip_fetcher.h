#pragma once

#include "define.h"
#include <unordered_map>
#include <string>
#include <cpprest/json.h>

using namespace web;

class ip_fetcher
{
public:
    result initialize();
    void uninitialize();
public:
    result fetch(std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::wstring>>& map_dns_info);
private:
    result parse_respond(json::value& resp, std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::wstring>>& map_dns_info);
};