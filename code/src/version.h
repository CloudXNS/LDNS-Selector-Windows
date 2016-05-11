#pragma once


#include "util.h"


__inline std::wstring get_version()
{
    std::wstring str_version;
    std::wstring str_image_file = util_get_image_path();
    DWORD size = GetFileVersionInfoSizeW(str_image_file.c_str(), NULL);
    WCHAR* buffer = new WCHAR[size + 1];
    do 
    {
        if (!GetFileVersionInfoW(str_image_file.c_str(), NULL, size, buffer))
        {
            break;
        }
        VS_FIXEDFILEINFO *FixDataInfo;
        UINT len;
        if (!VerQueryValueW(buffer, L"\\", (LPVOID *)&FixDataInfo, &len))
        {
            break;
        }
        str_version = L"Ver " + 
            std::to_wstring(HIWORD(FixDataInfo->dwProductVersionMS)) + L"." +
            std::to_wstring(LOWORD(FixDataInfo->dwProductVersionMS)) + L"." +
            std::to_wstring(HIWORD(FixDataInfo->dwProductVersionLS)) + L"." +
            std::to_wstring(LOWORD(FixDataInfo->dwProductVersionLS));
    } while (false);
    delete[] buffer;
    return str_version;
}