#include "Interface.h"
#include "Main.h"

bool DNSPingInterface::Ping(const std::wstring& str_dns_ip, const std::wstring& str_url, unsigned int& t_ms)
{
    wchar_t argv_1[4096];
    GetModuleFileName(NULL, argv_1, 4096);
    wchar_t argv_2[64] = L"-qt";
    wchar_t argv_3[64] = L"A";
    wchar_t argv_4[64] = L"-n";
#ifdef _DEBUG
    wchar_t argv_5[64] = L"1";
#else
    wchar_t argv_5[64] = L"3";
#endif // _DEBUG
    wchar_t argv_6[64] = L"-w";
    wchar_t argv_7[64] = L"500";
    wchar_t argv_8[64];
    wcscpy_s(argv_8, str_url.c_str());
    wchar_t argv_9[64];
    wcscpy_s(argv_9, str_dns_ip.c_str());

    wchar_t* argv[9] = { argv_1, argv_2, argv_3, argv_4, argv_5, argv_6, argv_7, argv_8, argv_9 };

    dnsping_main(9, argv);

    if (ConfigurationParameter.Statistics_RealSend - ConfigurationParameter.Statistics_RecvNum > 0)
    {
        return false;
    }

    t_ms = ConfigurationParameter.Statistics_TotalTime / (long double)ConfigurationParameter.Statistics_RecvNum;

    return true;
}
