#pragma once

#ifdef __DATASEARCH_CLASS_EXPORT__
#define __DATASEARCH_CLASS__ __declspec(dllexport)
#else
#define __DATASEARCH_CLASS__ __declspec(dllimport)
#endif

class CDlgDataSearch;
class __DATASEARCH_CLASS__ AccessorDataSearch
{
public:
    AccessorDataSearch()
    {
        if (m_instance == nullptr)
        {
            m_instance = new AccessorDataSearch();
        }
    }

    bool ShowDlg();

    void GetSystemFilePath(CString strPath);
    void GetSystemTopBack(CString strTopBack);
    
    static AccessorDataSearch& Get()
    {
        return *m_instance;
    }
    CDlgDataSearch* m_pDlgDataSearch;

private:
    static AccessorDataSearch* m_instance;
};

CString m_strFilePath;
CString m_strTopBack;