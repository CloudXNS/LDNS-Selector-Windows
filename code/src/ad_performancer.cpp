#include "ad_performancer.h"
#include "ui.h"
#include "define.h"

#include <cpprest/json.h>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

result ad_performancer::initialize()
{
    result res = check_ad_resource(AD_LINK_URL);
    if (res != result_success)
    {
        return res;
    }
    ui::instance()->navigate(AD_LINK_URL);
    return result_success;
}

void ad_performancer::uninitialize()
{
}

result ad_performancer::check_ad_resource(const std::string& str_resource_url)
{
    result res = result_success;
    http_client client(util_string_a2w(str_resource_url));
    auto query = uri_builder().to_string();

    try
    {
        client
            .request(methods::GET, query)
            .then([&](http_response response)
        {
            if (response.status_code() != status_codes::OK)
            {
                res = result_temp_ad_uri_invalid;
            }
        }).wait();
    }
    catch (http_exception const&)
    {
        res = result_temp_ad_uri_invalid;
    }
    return res;
}