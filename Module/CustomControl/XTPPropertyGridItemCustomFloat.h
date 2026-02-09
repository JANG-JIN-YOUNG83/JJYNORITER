#pragma once

#ifdef _XTP_INCLUDE_CONTROLS

template<typename T> class XTPPropertyGridItemCustomFloat : public CXTPPropertyGridItem
{
public:
	XTPPropertyGridItemCustomFloat(LPCTSTR strCaption, const T originalValue, LPCTSTR strFormat, T &editingValue)
		: CXTPPropertyGridItem(strCaption)
		, m_originalValue(originalValue)
		, m_strFormat(strFormat)
		, m_editingValue(editingValue)
	{
		m_strDefaultValue = ToString(editingValue);
		SetValue(m_strDefaultValue);
	}

	virtual ~XTPPropertyGridItemCustomFloat()
	{
	}

	void SetValue(LPCTSTR strFormat, T value)
	{
		m_strFormat = strFormat;
		CString str;
		str.Format(_T("%lf"), value);

		SetValue(str);
	}

	void SetValue(T value)
	{
		CString str;
		str.Format(_T("%lf"), value);

		SetValue(str);
	}

protected:
	virtual void SetValue(CString strValue) override
	{
		m_editingValue = T(::_ttof(strValue));

		CXTPPropertyGridItem::SetValue(ToString(m_editingValue));
	}

	virtual void OnBeforeInsert() override
	{
		m_strValue = ToString(m_editingValue);
	}

protected:
	CString ToString(T value)
	{
		CString strFormat(m_strFormat);
		if (strFormat.IsEmpty())
			strFormat = _T("%0.2f");

		CString strValue;
		strValue.Format(strFormat, value);

		return strValue;
	}

protected:
	const T m_originalValue;
	CString m_strFormat;
	T &m_editingValue;	 // Binded value.  This is a pointer to the variable bound to this item.
};

#endif
