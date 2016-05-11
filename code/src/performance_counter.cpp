#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "IPHLPAPI.lib")
#include <iphlpapi.h>

#include "performance_counter.h"
#include "ui.h"

result performance_counter::get_adapter_performance(std::vector<adapter_performance_counter>& vec_adapter_pfmc_counter)
{
    result res = result_success;
    MIB_IFTABLE *pIfTable = nullptr;
    do
    {
        pIfTable = (MIB_IFTABLE*)malloc(sizeof(MIB_IFTABLE));
        if (pIfTable == nullptr)
        {
            res = result_getiftalbe_malloc_fail;
            break;
        }
        DWORD dwSize = 0;
        dwSize = sizeof(MIB_IFTABLE);
        if (GetIfTable(pIfTable, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER)
        {
            free(pIfTable);
            pIfTable = (MIB_IFTABLE*)malloc(dwSize);
            if (pIfTable == nullptr)
            {
                res = result_getiftalbe_malloc_fail;
                break;
            }
        }
        DWORD dwRetVal = GetIfTable(pIfTable, &dwSize, FALSE);
        if (dwRetVal != NO_ERROR)
        {
            ui::instance()->showerror("GetIfTable failed with error:[%d]", dwRetVal);
            res = result_getiftalbe_fail;
            break;
        }
        for (unsigned int i = 0; i != pIfTable->dwNumEntries; i++)
        {
            MIB_IFROW *pIfRow = nullptr;
            pIfRow = (MIB_IFROW*)& pIfTable->table[i];

            adapter_performance_counter pfmc_counter;

            pfmc_counter._adapter_interface_index = pIfRow->dwIndex;
            pfmc_counter._recv_bytes_total = pIfRow->dwInOctets;
            pfmc_counter._send_bytes_total = pIfRow->dwOutOctets;

            vec_adapter_pfmc_counter.push_back(pfmc_counter);
        }
        res = result_success;
    } while (false);
    if (res != result_success)
    {
        if (pIfTable != nullptr)
        {
            free(pIfTable);
        }
    }
    return res;
}
