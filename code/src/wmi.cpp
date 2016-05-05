#include "wmi.h"
#include "ui.h"

#define _WIN32_DCOM
#include <comdef.h>
#include "comutil.h"
#include "atlbase.h"
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "comsuppw.lib")

result wmi_interface::get_adapter_list(std::vector<wmi_adapter_info>& vec)
{
    IWbemServices* pSvc = nullptr;
    IWbemLocator* pLoc = nullptr;

    result res = result_success;
    do
    {
        res = initliaze_wmi(pSvc, pLoc);
        if (res != result_success)
        {
            break;
        }

        res = result_enum_adapter_fail;
        // Step 6: --------------------------------------------------
        // Use the IWbemServices pointer to make requests of WMI ----

        // For example, get the name of the operating system

        IEnumWbemClassObject* pEnumerator = NULL;
        HRESULT hres = pSvc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM Win32_NetworkAdapterConfiguration"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator);

        if (FAILED(hres))
        {
            ui::instance()->showerror("Query for operating system name failed Error code = 0x%8x", hres);
            break;
        }

        if (pEnumerator == nullptr)
        {
            ui::instance()->showerror("Enumerator is null");
            break;
        }
        // Step 7: -------------------------------------------------
        // Get the data from the query in step 6 -------------------

        while (true)
        {
            bool cotinue = true;
            IWbemClassObject *pclsObj = nullptr;
            wmi_adapter_info info;
            do
            {
                ULONG uReturn = 0;
                hres = pEnumerator->Next(WBEM_INFINITE, 1,
                    &pclsObj, &uReturn);

                if (FAILED(hres))
                {
                    cotinue = false;
                    ui::instance()->showerror("enum adapter fail with Error code = 0x%8x", hres);
                    break;
                }
                if (0 == uReturn)
                {
                    cotinue = false;
                    break;
                }

                VARIANT vtProp;
                hres = pclsObj->Get(L"IPEnabled", 0, &vtProp, 0, 0);
                if (FAILED(hres))
                {
                    ui::instance()->showerror("get adapter \"IPEnable\" fail with Error code = 0x%8x", hres);
                    break;
                }
                else
                {
                    if (vtProp.vt != VT_NULL && vtProp.boolVal == 0)
                    {
                        VariantClear(&vtProp);
                        break;
                    }
                    else
                    {
                        VariantClear(&vtProp);
                    }
                }

                hres = pclsObj->Get(L"Index", 0, &vtProp, 0, 0);
                if (FAILED(hres))
                {
                    ui::instance()->showerror("get adapter \"Index\" fail with Error code = 0x%8x", hres);
                    break;
                }
                else
                {
                    if (vtProp.vt != VT_NULL && vtProp.intVal != -1)
                    {
                        info._index = vtProp.intVal;
                        VariantClear(&vtProp);
                    }
                    else
                    {
                        VariantClear(&vtProp);
                        continue;
                    }
                }

                hres = pclsObj->Get(L"InterfaceIndex", 0, &vtProp, 0, 0);
                if (FAILED(hres))
                {
                    ui::instance()->showerror("get adapter \"InterfaceIndex\" fail with Error code = 0x%8x", hres);
                    break;
                }
                else
                {
                    if (vtProp.vt != VT_NULL && vtProp.intVal != -1)
                    {
                        info._interface_index = vtProp.intVal;
                        VariantClear(&vtProp);
                    }
                    else
                    {
                        VariantClear(&vtProp);
                        continue;
                    }
                }

                hres = pclsObj->Get(L"Description", 0, &vtProp, 0, 0);
                if (FAILED(hres))
                {
                    ui::instance()->showerror("get adapter \"Description\" fail with Error code = 0x%8x", hres);
                    break;
                }
                else
                {
                    if (vtProp.vt != VT_NULL && vtProp.bstrVal != nullptr)
                    {
                        info._str_description = vtProp.bstrVal;
                    }
                    else
                    {
                        info._str_description = L"δ֪";
                    }
                    VariantClear(&vtProp);
                }

                hres = pclsObj->Get(L"MacAddress", 0, &vtProp, 0, 0);
                if (FAILED(hres))
                {
                    ui::instance()->showerror("get adapter \"MacAddress\" fail with Error code = 0x%8x", hres);
                    break;
                }
                else
                {
                    if (vtProp.vt != VT_NULL && vtProp.bstrVal != nullptr)
                    {
                        info._str_mac_address = vtProp.bstrVal;
                    }
                    else
                    {
                        info._str_mac_address = L"δ֪";
                    }
                    VariantClear(&vtProp);
                }

                hres = pclsObj->Get(L"DHCPEnabled", 0, &vtProp, 0, 0);
                if (FAILED(hres))
                {
                    ui::instance()->showerror("get adapter \"DHCPEnabled\" fail with Error code = 0x%8x", hres);
                    break;
                }
                else
                {
                    if (vtProp.vt != VT_NULL && vtProp.boolVal != -1)
                    {
                        info._dhcp = (vtProp.boolVal == VARIANT_TRUE);
                    }
                    else
                    {
                        info._dhcp = true;
                    }
                    VariantClear(&vtProp);
                }

                hres = pclsObj->Get(L"DNSServerSearchOrder", 0, &vtProp, 0, 0);
                if (FAILED(hres))
                {
                    ui::instance()->showerror("get adapter \"DNSServerSearchOrder\" fail with Error code = 0x%8x", hres);
                    break;
                }
                else
                {
                    info._str_dns_1 = L"";
                    info._str_dns_2 = L"";
                    if (vtProp.vt != VT_NULL && vtProp.parray != nullptr)
                    {
                        unsigned int valid_dns_ip = 0;
                        for (auto n = 0; n != vtProp.parray->cbElements; n++)
                        {
                            LONG i = n;
                            BSTR str_dns_ip;
                            if (FAILED(SafeArrayGetElement(vtProp.parray, &i, &str_dns_ip)))
                            {
                                continue;
                            }
                            if (++valid_dns_ip == 1)
                            {
                                info._str_dns_1 = str_dns_ip;
                                continue;
                            }
                            if (valid_dns_ip == 2)
                            {
                                info._str_dns_2 = str_dns_ip;
                                break;
                            }
                        }
                    }
                    VariantClear(&vtProp);
                }

                vec.push_back(info);
            } while (false);
            if (pclsObj != nullptr)
            {
                pclsObj->Release();
            }
            if (!cotinue)
            {
                break;
            }
        }
        pEnumerator->Release();
        res = result_success;
    } while (false);
    uninitialize_wmi(pSvc, pLoc);
    return res;
}

result wmi_interface::switch_adapter_dns(unsigned int index, const std::wstring& str_dns_1, const std::wstring& str_dns_2)
{
    Assert(!str_dns_1.empty());
    Assert(!str_dns_2.empty());
    if (str_dns_1.empty() || str_dns_2.empty())
    {
        ui::instance()->showlog("DNS_1[%ws] DNS_2[%ws]", str_dns_1.c_str(), str_dns_2.c_str());
        return result_dns_ip_invalid;
    }
    IWbemServices* pSvc = nullptr;
    IWbemLocator* pLoc = nullptr;

    result res = result_success;
    do
    {
        res = initliaze_wmi(pSvc, pLoc);
        if (res != result_success)
        {
            break;
        }

        res = result_switch_dns_ip_fail;
        wchar_t instance_path[1024];
        wsprintfW(instance_path, L"Win32_NetworkAdapterConfiguration.index=%d", index);

        BSTR InstancePath = SysAllocString(instance_path);
        BSTR ClassPath = SysAllocString(L"Win32_NetworkAdapterConfiguration");
        BSTR MethodName1 = SysAllocString(L"SetDNSServerSearchOrder");
        LPCWSTR MethodName1ArgName = L"DNSServerSearchOrder";
        CComBSTR dns1 = str_dns_1.c_str();
        CComBSTR dns2 = str_dns_2.c_str();

        long DnsIndex1[] = { 0 };
        long DnsIndex2[] = { 1 };

        SAFEARRAY *ip_list = SafeArrayCreateVector(VT_BSTR, 0, 2);
        SafeArrayPutElement(ip_list, DnsIndex1, dns1);
        SafeArrayPutElement(ip_list, DnsIndex2, dns2);

        VARIANT dns;
        dns.vt = VT_ARRAY | VT_BSTR;
        dns.parray = ip_list;

        IWbemClassObject* pClsObj = nullptr;
        IWbemClassObject* pInParamsDefinition = nullptr;
        IWbemClassObject* pClassInstance = nullptr;
        IWbemClassObject* pOutParams = nullptr;
        do
        {
            HRESULT hres = pSvc->GetObject(ClassPath, 0, NULL, &pClsObj, NULL);
            if (FAILED(hres))
            {
                ui::instance()->showerror("GetObject \"Win32_NetworkAdapterConfiguration\" fail with Error code = 0x%8x", hres);
                break;
            }

            hres = pClsObj->GetMethod(MethodName1, 0, &pInParamsDefinition, NULL);
            if (FAILED(hres))
            {
                ui::instance()->showerror("GetMethod \"SetDNSServerSearchOrder\" fail with Error code = 0x%8x", hres);
                break;
            }

            hres = pInParamsDefinition->SpawnInstance(0, &pClassInstance);
            if (FAILED(hres))
            {
                ui::instance()->showerror("SpawnInstance fail with Error code = 0x%8x", hres);
                break;
            }

            hres = pClassInstance->Put(MethodName1ArgName, 0, &dns, 0);
            if (FAILED(hres))
            {
                ui::instance()->showerror("Put Arg \"DNSServerSearchOrder\" fail with Error code = 0x%8x", hres);
                break;
            }

            hres = pSvc->ExecMethod(InstancePath, MethodName1, 0, NULL,
                pClassInstance, &pOutParams, NULL);
            if (FAILED(hres))
            {
                ui::instance()->showerror("ExecMethod [DNSServerSearchOrder] InstancePath[%ws] fail with Error code = 0x%8x", instance_path, hres);
                break;
            }
            res = result_success;
        } while (false);
        SysFreeString(InstancePath);
        SysFreeString(ClassPath);
        SysFreeString(MethodName1);
        SafeArrayDestroy(ip_list);
        if (pClsObj != nullptr)
        {
            pClsObj->Release();
        }
        if (pInParamsDefinition != nullptr)
        {
            pInParamsDefinition->Release();
        }
        if (pClassInstance != nullptr)
        {
            pClassInstance->Release();
        }
        if (pOutParams != nullptr)
        {
            pOutParams->Release();
        }
    } while (false);
    uninitialize_wmi(pSvc, pLoc);
    return res;
}

result wmi_interface::initliaze_wmi(IWbemServices* &pSvc, IWbemLocator* &pLoc)
{
    result res = result_wmi_initialize_fail;
    HRESULT hres = S_OK;
    do
    {
        // Step 1: --------------------------------------------------
        // Initialize COM. ------------------------------------------

        hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres))
        {
            ui::instance()->showerror("CoInitializeEx fail. Error code = 0x%8x", hres);
            break;
        }
        // Step 2: --------------------------------------------------
        // Set general COM security levels --------------------------
        // Note: If you are using Windows 2000, you need to specify -
        // the default authentication credentials for a user by using
        // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
        // parameter of CoInitializeSecurity ------------------------

        hres = CoInitializeSecurity(
            NULL,
            -1,                          // COM authentication
            NULL,                        // Authentication services
            NULL,                        // Reserved
            RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
            RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
            NULL,                        // Authentication info
            EOAC_NONE,                   // Additional capabilities 
            NULL                         // Reserved
            );
        if (FAILED(hres) && hres != RPC_E_TOO_LATE)
        {
            ui::instance()->showerror("Failed to initialize security. Error code = 0x%8x", hres);
            break;
        }

        // Step 3: ---------------------------------------------------
        // Obtain the initial locator to WMI -------------------------

        hres = CoCreateInstance(
            CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator, (LPVOID *)&pLoc);

        if (FAILED(hres))
        {
            ui::instance()->showerror("Failed to create IWbemLocator object Err code = 0x%8x", hres);
            break;
        }

        // Step 4: -----------------------------------------------------
        // Connect to WMI through the IWbemLocator::ConnectServer method


        // Connect to the root\cimv2 namespace with
        // the current user and obtain pointer pSvc
        // to make IWbemServices calls.
        hres = pLoc->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
            NULL,                    // User name. NULL = current user
            NULL,                    // User password. NULL = current
            0,                       // Locale. NULL indicates current
            NULL,                    // Security flags.
            0,                       // Authority (e.g. Kerberos)
            0,                       // Context object 
            &pSvc                    // pointer to IWbemServices proxy
            );

        if (FAILED(hres))
        {
            ui::instance()->showerror("Could not connect. Error code = 0x%8x", hres);
            break;
        }

        // Step 5: --------------------------------------------------
        // Set security levels on the proxy -------------------------

        hres = CoSetProxyBlanket(
            pSvc,                        // Indicates the proxy to set
            RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
            RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
            NULL,                        // Server principal name 
            RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
            RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
            NULL,                        // client identity
            EOAC_NONE                    // proxy capabilities 
            );

        if (FAILED(hres))
        {
            ui::instance()->showerror("Could not set proxy blanket. Error code = 0x%8x", hres);
            break;
        }
        res = result_success;
    } while (false);
    if (res != result_success)
    {
        if (pLoc != nullptr)
        {
            pLoc->Release();
        }
        if (pSvc != nullptr)
        {
            pSvc->Release();
        }
        CoUninitialize();
    }
    return res;
}

void wmi_interface::uninitialize_wmi(IWbemServices* pSvc, IWbemLocator* pLoc)
{
    if (pLoc != nullptr)
    {
        pLoc->Release();
    }
    if (pSvc != nullptr)
    {
        pSvc->Release();
    }
    CoUninitialize();
}