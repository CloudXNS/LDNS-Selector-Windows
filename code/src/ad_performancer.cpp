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
    result res = check_ad_resource(AD_IMG_URL);
    if (res != result_success)
    {
        return res;
    }
    m_str_img_url = AD_IMG_URL;
    res = check_ad_resource(AD_LINK_URL);
    if (res == result_success)
    {
        m_str_link_url = AD_LINK_URL;
    }
    else
    {
        m_str_link_url = DEFAULT_AD_LINK_URL;
    }
    res = prepare_ad_html();
    if (res != result_success)
    {
        return res;
    }
    ui::instance()->navigate(m_str_html_file);
    return result_success;
}

void ad_performancer::uninitialize()
{
    if (!m_str_html_file.empty())
    {
        DeleteFile(util_string_a2w(m_str_html_file).c_str());
    }
}


result ad_performancer::prepare_ad_html()
{
    HANDLE hHtmlFile = nullptr;
    result res = result_temp_ad_html_file_create_file;
    do
    {
        WCHAR szTempDir[1024];
        if (0 == GetTempPathW(1024, szTempDir))
        {
            break;
        }
        WCHAR szHtmlFile[1024];
        if (0 == GetTempFileNameW(szTempDir, L"xns", 0, szHtmlFile))
        {
            break;
        }
        HANDLE hHtmlFile = CreateFileW(szHtmlFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hHtmlFile == nullptr || hHtmlFile == INVALID_HANDLE_VALUE)
        {
            break;
        }
        CHAR szHtmlContent[4096];
        sprintf_s(szHtmlContent, sizeof(szHtmlContent), AD_HTML_FMT, m_str_link_url.c_str(), m_str_img_url.c_str());
        DWORD dwWriteBytes = 0;
        if (!WriteFile(hHtmlFile, szHtmlContent, strlen(szHtmlContent), &dwWriteBytes, NULL))
        {
            break;
        }
        CloseHandle(hHtmlFile);
        m_str_html_file = util_string_w2a(szHtmlFile);
        res = result_success;
    } while (false);
    if (res != result_success)
    {
        if (hHtmlFile != nullptr)
        {
            CloseHandle(hHtmlFile);
        }
    }
    return res;
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