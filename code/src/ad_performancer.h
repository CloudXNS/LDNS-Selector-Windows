#pragma once

#include "define.h"
#include <string>
class ad_performancer
{
public:
    result initialize();
    void uninitialize();
private:
    result prepare_ad_html();
    result check_ad_resource(const std::string& str_resource_url);
private:
    std::string m_str_html_file;
    std::string m_str_img_url;
    std::string m_str_link_url;
public:
    ad_performancer()
    {}
};