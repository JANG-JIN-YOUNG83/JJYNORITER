#pragma once

#ifdef _XTP_INCLUDE_CONTROLS

template<typename T> class XTPPropertyGridItemCustomNumber : public CXTPPropertyGridItem
{
public:
	XTPPropertyGridItemCustomNumber(LPCTSTR strCaption, const T originalValue, LPCTSTR strFormat, T &editingValue)
		: CXTPPropertyGridItem(strCaption)
		, m_originalValue(originalValue)
		, m_strFormat(strFormat)
		, m_editingValue(editingValue)
	{
		m_strDefaultValue = ToString(editingValue);
		SetValue(m_strDefaultValue);
	}

	virtual ~XTPPropertyGridItemCustomNumber()
	{
	}

protected:
	virtual void SetValue(CString strValue) override
	{
		m_editingValue = T(::_ttoi(strValue));
		CXTPPropertyGridItem::SetValue(ToString(m_editingValue));
	}

	virtual void OnBeforeInsert() override
	{
		m_strValue = ToString(m_editingValue);
	}

	virtual BOOL IsValueChanged() const override
	{
		return m_editingValue != m_originalValue;
	}

protected:
	CString ToString(T value)
	{
		CString strValue;
		strValue.Format(m_strFormat, value);

		return strValue;
	}

protected:
	const T m_originalValue;
	const CString m_strFormat;
	T &m_editingValue;
};

#endif
