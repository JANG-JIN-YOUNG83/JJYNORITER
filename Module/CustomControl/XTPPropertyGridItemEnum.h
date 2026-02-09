#pragma once

#ifdef _XTP_INCLUDE_CONTROLS

template<typename T> class XTPPropertyGridItemEnum : public CXTPPropertyGridItemEnum
{
public:
	XTPPropertyGridItemEnum(LPCTSTR strCaption, const T originalValue, T &editingValue)
		: CXTPPropertyGridItemEnum(strCaption)
		, m_originalValue(originalValue)
		, m_editingValue(editingValue)
	{
		m_strDefaultValue = m_pConstraints->GetAt(static_cast<int>(originalValue));
		SetEnum(int(m_editingValue));
	}

	virtual ~XTPPropertyGridItemEnum()
	{
	}

protected:
	virtual void SetEnum(int nValue) override
	{
		m_editingValue = T(nValue);

		int nIndex = m_pConstraints->FindConstraint(nValue);

		CXTPPropertyGridItem::SetValue(m_pConstraints->GetAt(nIndex));
	}

	virtual void SetEnum(CXTPPropertyGridItemConstraint* pConstraint) override
	{
		m_editingValue = T(pConstraint->m_dwData);

		CXTPPropertyGridItem::SetValue(pConstraint->m_strConstraint);
	}

	virtual void OnBeforeInsert() override
	{
		m_strValue = m_pConstraints->GetAt(m_pConstraints->FindConstraint(static_cast<int>(m_editingValue)));
	}

	virtual BOOL IsValueChanged() const override
	{
		return m_editingValue != m_originalValue;
	}

protected:
	const T m_originalValue;
	T &m_editingValue;	 // Binded value.  This is a pointer to the variable bound to this item.
};

#endif
