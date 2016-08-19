#pragma once
#include "util.h"
#include "interface.h"
#include "../third/duilib/src/UIlib.h"
#include <unordered_map>
#include <thread>

using namespace DuiLib;

class ui : public DuiLib::WindowImplBase, public singleton<ui>
{
public:
    virtual DuiLib::CDuiString GetSkinFolder();
    virtual DuiLib::CDuiString GetSkinFile();
    virtual LPCTSTR GetWindowClassName(void) const;
    virtual LPCTSTR GetResourceID() const;
    virtual UILIB_RESOURCETYPE GetResourceType() const;

    virtual void InitWindow();
    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
public:
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
    DUI_DECLARE_MESSAGE_MAP();
    virtual void OnClick(TNotifyUI& msg);
    virtual void OnItemClick(TNotifyUI& msg);
public:
    result initialize();
    void uninitialize();

    result startup(ui_notifyer* p_notifyer, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
    void showlog(const char* log, ...);
    void showwarning(const char* warning, ...);
    void showerror(const char* error, ...);
    void show_ad(bool show);
    void navigate(const std::string& str_url);
private:
    void post_call(std::function<void()> func);
    void send_call(std::function<void()> func);
    void ui_fresh();
private:
    void EnableUserOperation(bool bEnable);
private:
    void OnAutoDone();
    void OnFreshenAdapterInfo();
    void OnFreshenDNSInfo();
    void OnSwitchAdapter();
private:
    DuiLib::CButtonUI* m_p_min_btn;
    DuiLib::CButtonUI* m_p_exit_btn;
    DuiLib::CLabelUI* m_p_version_label;
    DuiLib::CButtonUI* m_p_auto_done_btn;
    DuiLib::CVerticalLayoutUI* m_p_professional_view;
    DuiLib::CButtonUI* m_p_freshen_dns_btn;
    DuiLib::CButtonUI* m_p_freshen_adapter_btn;
    DuiLib::CButtonUI* m_p_switch_adapter_dns_btn;
    DuiLib::CListUI* m_p_dns_info_list;
    DuiLib::CListUI* m_p_adapter_info_list;
    DuiLib::CListUI* m_p_log_list;
    DuiLib::CWebBrowserUI* m_p_ad_brower;
    DuiLib::CTabLayoutUI* m_p_info_tablayout;
private:
    DWORD m_orgin_dns_item_color;
    DWORD m_orgin_adapter_item_color;
    DuiLib::CListContainerElementUI* m_p_last_selected_adapter_item;
    DuiLib::CListContainerElementUI* m_p_last_selected_dns_item;
private:
    ui_notifyer* m_p_notifyer;
    std::unordered_map<DuiLib::CListContainerElementUI*, std::wstring> m_map_dns_item_ip;
    std::unordered_map<DuiLib::CListContainerElementUI*, unsigned int> m_map_adapter_item_index;
private:
    ui()
        : m_p_notifyer(nullptr)
        , m_p_min_btn(nullptr)
        , m_p_exit_btn(nullptr)
        , m_p_version_label(nullptr)
        , m_p_auto_done_btn(nullptr)
        , m_p_professional_view(nullptr)
        , m_p_freshen_dns_btn(nullptr)
        , m_p_freshen_adapter_btn(nullptr)
        , m_p_switch_adapter_dns_btn(nullptr)
        , m_p_dns_info_list(nullptr)
        , m_p_adapter_info_list(nullptr)
        , m_p_log_list(nullptr)
        , m_p_ad_brower(nullptr)
        , m_p_info_tablayout(nullptr)
        , m_orgin_dns_item_color(0xFFFFFFFF)
        , m_orgin_adapter_item_color(0xFFFFFFFF)
        , m_p_last_selected_adapter_item(nullptr)
        , m_p_last_selected_dns_item(nullptr)
    {}
private:
    SINLETON_CLASS_DECLARE(ui);
};