#include "ui.h"
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <locale.h>
#include <memory>
#include "resource.h"
#include "version.h"

DUI_BEGIN_MESSAGE_MAP(ui, DuiLib::CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

DuiLib::CDuiString ui::GetSkinFolder()
{
    return _T("");
}

DuiLib::CDuiString ui::GetSkinFile()
{
    return _T("main_frame.xml");
}

LPCTSTR ui::GetWindowClassName(void) const
{
    return _T("dns_selector_wnd_class");
}

LPCTSTR ui::GetResourceID() const
{
#ifdef _DEBUG
    return WindowImplBase::GetResourceID();
#else
    return MAKEINTRESOURCE(IDR_ZIP_SKIN);
#endif // _DEBUG
}

DuiLib::UILIB_RESOURCETYPE ui::GetResourceType() const
{
#ifdef _DEBUG
    return WindowImplBase::GetResourceType();
#else
    return UILIB_ZIPRESOURCE;
#endif // _DEBUG
}

void ui::InitWindow()
{
    m_p_exit_btn = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T("exit_btn")));
    Assert(m_p_exit_btn != nullptr);
    m_p_min_btn = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T("min_btn")));
    Assert(m_p_min_btn != nullptr);
    m_p_version_label = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T("version_label")));
    Assert(m_p_version_label != nullptr);
    m_p_auto_done_btn = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T("auto_done_btn")));
    Assert(m_p_auto_done_btn != nullptr);
    m_p_professional_view = static_cast<DuiLib::CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("professional_view")));
    Assert(m_p_professional_view != nullptr);
    m_p_adapter_info_list = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T("adapter_info_list")));
    Assert(m_p_adapter_info_list != nullptr);
    m_p_dns_info_list = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T("dns_info_list")));
    Assert(m_p_dns_info_list != nullptr);
    m_p_freshen_adapter_btn = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T("freshen_adapter_btn")));
    Assert(m_p_freshen_adapter_btn != nullptr);
    m_p_freshen_dns_btn = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T("freshen_dns_btn")));
    Assert(m_p_freshen_dns_btn != nullptr);
    m_p_switch_adapter_dns_btn = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl(_T("switch_adapter_dns_btn")));
    Assert(m_p_switch_adapter_dns_btn != nullptr);
    m_p_info_tablayout = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(_T("info_tablayout")));
    Assert(m_p_info_tablayout != nullptr);
    m_p_log_list = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T("log_list")));
    Assert(m_p_log_list != nullptr);
    m_p_ad_brower = static_cast<DuiLib::CWebBrowserUI*>(m_PaintManager.FindControl(_T("ad_brower")));
    Assert(m_p_ad_brower != nullptr);
    
    m_p_info_tablayout->SelectItem(m_p_ad_brower);

    m_p_ad_brower->SetDelayCreate(false);
    m_p_ad_brower->Navigate2(L"about:blank");

    m_p_version_label->SetText(get_version().c_str());
    m_p_log_list->GetHeader()->SetFixedHeight(1);
    m_p_dns_info_list->GetHeader()->SetFixedHeight(1);
    m_p_adapter_info_list->GetHeader()->SetFixedHeight(1);

    EnableUserOperation(false);
    std::thread(
        [this]()
    {
        result res = m_p_notifyer->on_initialize(); 
        if (res == result_success)
        {
            post_call(
                [this]()
            {
                EnableUserOperation(true);
            }
                );
        }
        else
        {
            showerror("初始化失败-->[%s]", result_string(res));
        }
    }
    ).detach();

}

LRESULT ui::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if (WM_TASK_MSG == uMsg)
    {
        std::function<void()>* p_func = (std::function<void()>*)wParam;
        (*p_func)();
        delete p_func;
    }
    else
    {

    }
    return FALSE;
}

LRESULT ui::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    SendMessageW(WM_DESTROY, 0, 0);
    bHandled = TRUE;
    return 0;
}

LRESULT ui::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    PostQuitMessage(0);
    bHandled = TRUE;
    return 0;
}

LRESULT ui::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (GetAsyncKeyState(VK_F5) && GetAsyncKeyState(VK_CONTROL))
    {
        bool pro_mode = m_p_professional_view->IsVisible();
        if (pro_mode)
        {
            show_ad(false);
            ResizeClient(UI_MAIN_FRAME_WIDTH, UI_MAIN_FRAME_MIN_HEIGHT);
            m_p_professional_view->SetVisible(false);
        }
        else
        {
            ResizeClient(UI_MAIN_FRAME_WIDTH, UI_MAIN_FRAME_MAX_HEIGHT);
            m_p_professional_view->SetVisible(true);
        }
    }
    bHandled = FALSE;
    return 0;
}

LRESULT ui::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (wParam == SIZE_MAXIMIZED)
    {
        bHandled = TRUE;
        return 0;
    }
    else if (wParam == SIZE_MAXSHOW)
    {
        bHandled = TRUE;
        return 0;
    }
    else
    {
        return WindowImplBase::OnSize(uMsg, wParam, lParam, bHandled);
    }
}

void ui::OnClick(TNotifyUI& msg)
{
    DuiLib::CControlUI* pSender = msg.pSender;
    if (pSender == m_p_min_btn)
    {
        ::ShowWindow(m_hWnd, SW_SHOWMINIMIZED);
    }
    else if (pSender == m_p_exit_btn)
    {
        Close();
    }
    else if (pSender == m_p_auto_done_btn)
    {
        show_ad(false);
        OnAutoDone();
    }
    else if (pSender == m_p_freshen_adapter_btn)
    {
        OnFreshenAdapterInfo();
    }
    else if (pSender == m_p_freshen_dns_btn)
    {
        OnFreshenDNSInfo();
    }
    else if (pSender == m_p_switch_adapter_dns_btn)
    {
        OnSwitchAdapter();
    }
    else
    {

    }
}

void ui::OnItemClick(TNotifyUI& msg)
{
    DuiLib::CControlUI* pSender = msg.pSender;
    {
        auto it = m_map_dns_item_ip.find(static_cast<DuiLib::CListContainerElementUI*>(pSender));
        if (it != m_map_dns_item_ip.end())
        {
            DuiLib::CListContainerElementUI* p_item = it->first;
            if (m_p_last_selected_dns_item != nullptr)
            {
                m_p_last_selected_dns_item->SetBkColor(m_orgin_dns_item_color);
            }
            p_item->SetBkColor(LIST_ITEM_SELECTED_COLOR);
            m_p_last_selected_dns_item = p_item;
            m_p_dns_info_list->SelectItem(m_p_dns_info_list->GetItemIndex(p_item));
        }
    }
    {
        auto it = m_map_adapter_item_index.find(static_cast<DuiLib::CListContainerElementUI*>(pSender));
        if (it != m_map_adapter_item_index.end())
        {
            DuiLib::CListContainerElementUI* p_item = it->first;
            if (m_p_last_selected_adapter_item != nullptr)
            {
                m_p_last_selected_adapter_item->SetBkColor(m_orgin_adapter_item_color);
            }
            p_item->SetBkColor(LIST_ITEM_SELECTED_COLOR);
            m_p_last_selected_adapter_item = p_item;
            m_p_adapter_info_list->SelectItem(m_p_adapter_info_list->GetItemIndex(p_item));
        }
    }
}


result ui::initialize()
{
    return result_success;
}

void ui::uninitialize()
{

}

result ui::startup(ui_notifyer* p_notifyer, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
    DuiLib::CPaintManagerUI::SetInstance(hInstance);
    DuiLib::CPaintManagerUI::SetResourcePath(util_get_image_dir().c_str());
    DuiLib::CPaintManagerUI::SetResourceZip(L"res.zip");
#endif //_DEBUG

    ::CoInitialize(NULL);
    ::OleInitialize(NULL);

    HRESULT Hr = ::CoInitialize(NULL);

    if (FAILED(Hr))
    {
        return result_com_initialize_fail;
    }

    m_p_notifyer = p_notifyer;

    HWND h_main_frame = Create(NULL, _T("dns_selector"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW);
    if (h_main_frame == NULL)
    {
        return result_main_frame_wnd_create_fail;
    }
    CenterWindow();
    ShowWindow(true);

    DuiLib::CPaintManagerUI::MessageLoop();
    DuiLib::CPaintManagerUI::Term();

    ::OleUninitialize();
    ::CoUninitialize();

    return result_success;
}
void ui::OnAutoDone()
{
    EnableUserOperation(false);
    std::thread(
        [this]()
    {
        std::wstring take_time;
        result res = m_p_notifyer->on_auto_done(take_time);
        if (res == result_success)
        {
            showlog("自动优化DNS成功，耗时[%ws]", take_time.c_str());
        }
        else
        {
            showerror("自动优化DNS失败-->[%s]", result_string(res));
        }
        EnableUserOperation(true);
    }
    ).detach();
}

void ui::OnFreshenAdapterInfo()
{
    m_p_last_selected_adapter_item = nullptr;
    m_orgin_adapter_item_color = 0xFFFFFFFF;
    m_p_adapter_info_list->RemoveAll();
    m_map_adapter_item_index.clear();
    EnableUserOperation(false);
    std::thread(
        [this]()
    {
        std::shared_ptr<freshen_adapter_info_result_data> sp_res_data = std::make_shared<freshen_adapter_info_result_data>();
        result res = m_p_notifyer->on_freshen_adapter_info(*sp_res_data.get());
        if (res != result_success)
        {
            showerror("刷新网卡失败-->[%s]", result_string(res));
            EnableUserOperation(true);
            return;
        }
        post_call(
            [this, sp_res_data]()
        {
            showlog("刷新网卡耗时[%ws]", sp_res_data->_str_freshen_take_time.c_str());
            for (auto it : sp_res_data->_vec_adapter_info)
            {
                ui_adapter_info& info = it;
                DuiLib::CDialogBuilder builder;
                DuiLib::CListContainerElementUI* p_item = static_cast<DuiLib::CListContainerElementUI*>(builder.Create(_T("adapter_info_item.xml"), (UINT)0, NULL, &m_PaintManager));
                Assert(p_item != nullptr);

                if (m_orgin_adapter_item_color == 0xFFFFFFFF)
                {
                    m_orgin_adapter_item_color = p_item->GetBkColor();
                }

                DuiLib::CLabelUI* p_adapter_description_label = static_cast<DuiLib::CLabelUI*>(p_item->FindSubControl(_T("adapter_description_label")));
                Assert(p_adapter_description_label != nullptr);
                DuiLib::CLabelUI* p_adapter_dns_1_label = static_cast<DuiLib::CLabelUI*>(p_item->FindSubControl(_T("adapter_dns_1_label")));
                Assert(p_adapter_dns_1_label != nullptr);
                DuiLib::CLabelUI* p_adapter_dns_2_label = static_cast<DuiLib::CLabelUI*>(p_item->FindSubControl(_T("adapter_dns_2_label")));
                Assert(p_adapter_dns_2_label != nullptr);

                p_adapter_description_label->SetText(info._str_description.c_str());
                p_adapter_dns_1_label->SetText(info._str_dns_1.c_str());
                p_adapter_dns_2_label->SetText(info._str_dns_2.c_str());

                m_p_adapter_info_list->Add(p_item);
                m_map_adapter_item_index.insert(std::make_pair(p_item, info._id));
            }

            EnableUserOperation(true);
        }
        );
    }
    ).detach();
}

void ui::OnFreshenDNSInfo()
{
    m_p_last_selected_dns_item = nullptr;
    m_orgin_dns_item_color = 0xFFFFFFFF;
    m_p_dns_info_list->RemoveAll();
    m_map_dns_item_ip.clear();

    EnableUserOperation(false);
    std::thread(
        [this]()
    {
        std::shared_ptr<freshen_dns_info_result_data> sp_res_data = std::make_shared<freshen_dns_info_result_data>();
        result res = m_p_notifyer->on_freshen_dns_info(*sp_res_data.get());
        if (res != result_success)
        {
            showerror("刷新DNS失败-->[%s]", result_string(res));
            EnableUserOperation(true);
            return;
        }
        post_call(
            [this, sp_res_data]()
        {
            showlog("刷新DNS列表耗时[%ws]", sp_res_data->_str_freshen_take_time.c_str());
            for (auto it : sp_res_data->_vec_ip_dns_info)
            {
                ui_dns_info& info = it;

                DuiLib::CDialogBuilder builder;
                DuiLib::CListContainerElementUI* p_item = static_cast<DuiLib::CListContainerElementUI*>(builder.Create(_T("dns_info_item.xml"), (UINT)0, NULL, &m_PaintManager));
                Assert(p_item != nullptr);

                if (m_orgin_dns_item_color == 0xFFFFFFFF)
                {
                    m_orgin_dns_item_color = p_item->GetBkColor();
                }

                DuiLib::CLabelUI* p_dns_vip_label = static_cast<DuiLib::CLabelUI*>(p_item->FindSubControl(_T("dns_vip_label")));
                Assert(p_dns_vip_label != nullptr);
                DuiLib::CLabelUI* p_dns_uptime_label = static_cast<DuiLib::CLabelUI*>(p_item->FindSubControl(_T("dns_uptime_label")));
                Assert(p_dns_uptime_label != nullptr);
                DuiLib::CLabelUI* p_dns_ping_t_ms_label = static_cast<DuiLib::CLabelUI*>(p_item->FindSubControl(_T("dns_ping_t_ms_label")));
                Assert(p_dns_ping_t_ms_label != nullptr);

                p_dns_vip_label->SetText(info._str_vip.c_str());
                p_dns_uptime_label->SetText(info._str_uptime.c_str());
                p_dns_ping_t_ms_label->SetText(info._str_ping_t_ms.c_str());

                m_p_dns_info_list->Add(p_item);
                m_map_dns_item_ip.insert(std::make_pair(p_item, info._str_vip));
            }

            EnableUserOperation(true);
        }
        );
    }
    ).detach();
}

void ui::OnSwitchAdapter()
{
    int adapter_cur_sel = m_p_adapter_info_list->GetCurSel();
    if (adapter_cur_sel == -1)
    {
        ui::instance()->showwarning("请选择需要切换的网卡");
        return;
    }
    int dns_cur_sel = m_p_dns_info_list->GetCurSel();
    if (dns_cur_sel == -1)
    {
        ui::instance()->showwarning("选择需要切换的DNS");
        return;
    }

    unsigned int index = 0;
    {
        auto it = m_map_adapter_item_index.find(static_cast<DuiLib::CListContainerElementUI*>(m_p_adapter_info_list->GetItemAt(adapter_cur_sel)));
        Assert(it != m_map_adapter_item_index.end());
        if (it == m_map_adapter_item_index.end())
        {
            ui::instance()->showerror("找不到所选网卡");
            return;
        }
        index = it->second;
    }

    std::wstring str_ip;
    {
        auto it = m_map_dns_item_ip.find(static_cast<DuiLib::CListContainerElementUI*>(m_p_dns_info_list->GetItemAt(dns_cur_sel)));
        Assert(it != m_map_dns_item_ip.end());
        if (it == m_map_dns_item_ip.end())
        {
            ui::instance()->showerror("找不到所选DNS");
            return;
        }
        str_ip = it->second;
    }

    EnableUserOperation(false);
    std::thread(
        [this, index, str_ip]()
    {
        result res = m_p_notifyer->on_switch_dns(index, str_ip);
        if (res == result_success)
        {
            OnFreshenAdapterInfo();
            ui::instance()->showlog("切换网卡DNS成功");
        }
        else
        {
            ui::instance()->showerror("切换网卡DNS失败-->[%s]", result_string(res));
        }
        EnableUserOperation(true);
    }
    ).detach();

}

void ui::showlog(const char* format, ...)
{
    char buffer[4096];
    char* p = buffer;
    va_list args;
    setlocale(LC_CTYPE, "chs");
    va_start(args, format);
    p += _vsnprintf_s(buffer, sizeof(buffer) - 1, format, args);
    *p = '\0';
    va_end(args);

    std::wstring str_log;
    str_log += util_string_a2w(buffer);
    post_call(
        [this, str_log]()
    {
        DuiLib::CLabelUI* p_item = new DuiLib::CLabelUI;
        p_item->SetTextColor(UI_MSG_TEXT_COLOR);
        p_item->SetText(str_log.c_str());
        m_p_log_list->Add(p_item);
        ui_fresh();
        m_p_log_list->EndDown();
    }
    );
}

void ui::showwarning(const char* format, ...)
{
    char buffer[4096];
    char* p = buffer;
    va_list args;
    setlocale(LC_CTYPE, "chs");
    va_start(args, format);
    p += _vsnprintf_s(buffer, sizeof(buffer) - 1, format, args);
    *p = '\0';
    va_end(args);

    std::wstring str_log;
    str_log += util_string_a2w(buffer);
    post_call(
        [this, str_log]()
    {
        DuiLib::CLabelUI* p_item = new DuiLib::CLabelUI;
        p_item->SetTextColor(UI_WARNING_TEXT_COLOR);
        p_item->SetText(str_log.c_str());
        m_p_log_list->Add(p_item);
        ui_fresh();
        m_p_log_list->EndDown();
    }
    );
}

void ui::showerror(const char* format, ...)
{
    char buffer[4096];
    char* p = buffer;
    va_list args;
    setlocale(LC_CTYPE, "chs");
    va_start(args, format);
    p += _vsnprintf_s(buffer, sizeof(buffer) - 1, format, args);
    *p = '\0';
    va_end(args);

    std::wstring str_log;
    str_log += util_string_a2w(buffer);
    post_call(
        [this, str_log]()
    {
        DuiLib::CLabelUI* p_item = new DuiLib::CLabelUI;
        p_item->SetTextColor(UI_ERROR_TEXT_COLOR);
        p_item->SetText(str_log.c_str());
        m_p_log_list->Add(p_item);
        ui_fresh();
        m_p_log_list->EndDown();
    }
    );
}

void ui::show_ad(bool show)
{
    post_call(
        [this, show]()
    {
        if (show)
        {
            m_p_info_tablayout->SelectItem(m_p_ad_brower);
        }
        else
        {
            m_p_info_tablayout->SelectItem(m_p_log_list);
        }
    }
    );
}

void ui::navigate(const std::string& str_url)
{
    post_call(
        [this, str_url]()
    {
        m_p_ad_brower->Navigate2(util_string_a2w(str_url).c_str());
    }
    );
}

void ui::post_call(std::function<void()> func)
{
    std::function<void()>* p_func = new std::function<void()>(func);
    PostMessage(WM_TASK_MSG, (WPARAM)p_func);
}

void ui::send_call(std::function<void()> func)
{
    std::function<void()>* p_func = new std::function<void()>(func);
    SendMessage(WM_TASK_MSG, (WPARAM)&p_func);
}

void ui::ui_fresh()
{
    SendMessage(WM_SIZE);
    SendMessage(WM_PAINT);
}

void ui::EnableUserOperation(bool bEnable)
{
    post_call(
        [bEnable,this]()
    {
        m_p_auto_done_btn->SetEnabled(bEnable);
        m_p_freshen_adapter_btn->SetEnabled(bEnable);
        m_p_freshen_dns_btn->SetEnabled(bEnable);
        m_p_switch_adapter_dns_btn->SetEnabled(bEnable);
    }
    );
}
