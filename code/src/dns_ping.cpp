#include "dns_ping.h"
#include "..\third\DNSPing\Source\Interface.h"

result dns_ping::initialize()
{
    return result_success;
}

void dns_ping::uninitialize()
{

}

result dns_ping::ping(const std::wstring& str_dns_ip, unsigned int& t_ms)
{
    unsigned int time;
    if (!DNSPingInterface::Ping(str_dns_ip, DNS_PING_URL, time))
    {
        return result_dnsping_fail;
    }
    t_ms = time;
    return result_success;
}
