#pragma once

#include "define.h"
#include <vector>

struct adapter_performance_counter
{
    unsigned int _adapter_interface_index;
    unsigned int _recv_bytes_total;
    unsigned int _send_bytes_total;

    adapter_performance_counter()
        : _adapter_interface_index(-1)
        , _recv_bytes_total(0)
        , _send_bytes_total(0)
    {}
};


class performance_counter
{
public:
    static result get_adapter_performance(std::vector<adapter_performance_counter>& vec_adapter_pfmc_counter);
};