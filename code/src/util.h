#pragma once
#include <stdio.h>
#include <windows.h>
#include <string>
#include <locale>
#include <codecvt>

template<class T>
class singleton
{
public:
    static T* instance()
    {
        static T* s_instance = nullptr;
        if (s_instance == nullptr)
        {
            if (s_instance == nullptr)
            {
                s_instance = new T();
            }
        }
        return s_instance;
    }
public:
    virtual ~singleton(){}
protected:
    singleton(){}
private:
    singleton(const singleton& s);
    singleton& operator =(const singleton& s);
};


#define SINLETON_CLASS_DECLARE(class_name)	\
	friend class singleton<##class_name>;


__inline std::wstring util_string_a2w(const std::string& str)
{
    if (str.size() == 0)
    {
        return L"";
    }
    int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), str.size(), 0, 0);
    if (nSize <= 0) return NULL;

    WCHAR *pwszDst = new WCHAR[nSize + 1];
    if (NULL == pwszDst) return NULL;

    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), str.size(), pwszDst, nSize);
    pwszDst[nSize] = 0;

    if (pwszDst[0] == 0xFEFF) // skip Oxfeff
        for (int i = 0; i < nSize; i++)
            pwszDst[i] = pwszDst[i + 1];

    std::wstring wcharString(pwszDst);
    delete pwszDst;

    return wcharString;
}

__inline std::string util_string_w2a(const std::wstring& wstr)
{
    if (wstr.size() == 0)
    {
        return "";
    }
    int nLen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

    if (nLen <= 0) return std::string("");

    char* pszDst = new char[nLen];
    if (NULL == pszDst) return std::string("");

    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pszDst, nLen, NULL, NULL);
    pszDst[nLen - 1] = 0;

    std::string strTemp(pszDst);
    delete[] pszDst;

    return strTemp;
}

__inline std::wstring util_get_image_dir()
{
    WCHAR szImageDir[4096];
    GetModuleFileNameW(NULL, szImageDir, 4096);
    unsigned int i = 0;
    for (unsigned int i = wcslen(szImageDir); i >= 0; i--)
    {
        if (szImageDir[i] == L'\\')
        {
            szImageDir[i + 1] = L'\0';
            break;
        }
    }
    return std::wstring(szImageDir);
}

__inline std::wstring util_get_image_path()
{
    WCHAR szImageDir[4096];
    GetModuleFileNameW(NULL, szImageDir, 4096);
    return std::wstring(szImageDir);
}


__inline void _DbgPoint()
{
#ifdef _DEBUG
#ifdef _WIN32
	__asm int 3;
#endif
#endif // _DEBUG
}



#ifdef _DEBUG
#define AssertMsg(exps, comment) if (!(exps)){char msg[1024];sprintf_s(msg, "%s:%d", __FILE__, __LINE__);MessageBoxA(NULL, msg, comment, MB_ICONERROR);_DbgPoint();}
#else
#define AssertMsg(exps, comment) if (!(exps)){}
#endif // _DEBUG

#define Assert(exps) AssertMsg(exps, "Assert Happened")