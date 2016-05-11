#include <windows.h>
#include "dns_selector.h"


int APIENTRY WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
    )
{
    result res = dns_selector::instance()->initialize(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    if (res != result_success)
    {
        wchar_t sz_error[1024];
        wsprintf(sz_error, L"DNS SelectorÆô¶¯Ê§°Ü[%ws]", result_string(res));
        MessageBoxW(NULL, L"DNS SelectorÆô¶¯Ê§°Ü...", L"´íÎó", MB_ICONERROR);
    }
    dns_selector::instance()->uninitialize();
    return 0;
}
