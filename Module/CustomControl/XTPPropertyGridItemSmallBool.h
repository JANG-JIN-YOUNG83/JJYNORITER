#pragma once

#ifdef _XTP_INCLUDE_CONTROLS

class XTPPropertyGridItemSmallBool : public CXTPPropertyGridItemBool
{
public:
	XTPPropertyGridItemSmallBool(LPCTSTR strCaption, bool bValue, bool* pBindBool)
		: CXTPPropertyGridItemBool(strCaption, bValue ? TRUE : FALSE, nullptr)
		, m_bindCutsomBool(pBindBool)
	{
	}

	XTPPropertyGridItemSmallBool(UINT nID, bool bValue, bool* pBindBool)
		: CXTPPropertyGridItemBool(nID, bValue ? TRUE : FALSE, nullptr)
		, m_bindCutsomBool(pBindBool)
	{
	}

	virtual ~XTPPropertyGridItemSmallBool()
	{
	}

	virtual void SetBool(BOOL bValue)
	{
		if (m_bindCutsomBool)
		{
			*m_bindCutsomBool = bValue ? true : false;
		}

		CXTPPropertyGridItemBool::SetBool(bValue);
	}

	virtual void BindToBool(BOOL* pBindBool)
	{
		m_bindCutsomBool = nullptr;
		CXTPPropertyGridItemBool::BindToBool(pBindBool);
	}

protected:
	virtual void OnBeforeInsert()
	{
		if (m_bindCutsomBool && *m_bindCutsomBool != (GetBool() ? true : false))
		{
			SetBool(*m_bindCutsomBool ? TRUE : FALSE);
		}
	}

private:
	bool* m_bindCutsomBool;
};

#endif
