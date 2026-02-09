#pragma once

#ifdef _XTP_INCLUDE_CONTROLS

class XTPPropertyGridItemFolder : public CXTPPropertyGridItem
{
public:
	XTPPropertyGridItemFolder(LPCTSTR caption, HWND parent, CString& value)
		: CXTPPropertyGridItem(caption, value, &value)
		, m_parent(parent)
		, m_value(value)
	{
	}

	virtual void OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton)
	{
		UNREFERENCED_PARAMETER(pButton);

		BROWSEINFO brif;
		memset(&brif, 0, sizeof(brif));
		brif.lpszTitle = _T("Select Folder");
		brif.hwndOwner = m_parent;
		brif.lParam = (LPARAM)&m_value;
		brif.lpfn = [](HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
		{
			UNREFERENCED_PARAMETER(lParam);

			auto& imagePath = (*((CString*)lpData));
			switch (uMsg) {
			case BFFM_INITIALIZED:
				::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)imagePath);
				break;
			}
			return 0;
		};

		LPITEMIDLIST pidl = SHBrowseForFolder(&brif);
		if (pidl == nullptr) return;

		TCHAR szPath[MAX_PATH];
		memset(szPath, 0, sizeof(TCHAR) * MAX_PATH);

		if (!SHGetPathFromIDList(pidl, szPath)) return;

		m_value = szPath;
		SetValue(szPath);
	}

private:
	HWND m_parent;
	CString& m_value;
};

#endif
