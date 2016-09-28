#pragma once

#include "define.h"
#include <string>
class ad_performancer
{
public:
    result initialize();
    void uninitialize();
private:
    result check_ad_resource(const std::string& str_resource_url);
public:
    ad_performancer()
    {}
};