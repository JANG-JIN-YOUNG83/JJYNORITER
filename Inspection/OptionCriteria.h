#pragma once

#include "basedef.h"

class Database;

namespace Share
{
	class SpecData;
};

class __INSPECTOIN_CLASS__ SpecCriteria
{
public:
	SpecCriteria();
	~SpecCriteria();

	void Init();
	void Link(bool bSave, Database &db);
	void LinkOld(bool bSave, Database &db, LPCTSTR minusKey, LPCTSTR plusKey);
	bool In(double value) const;
	bool In(float nominalValue, double value) const;
	bool IsUsed() const;
	bool IsNotUsed() const;

	float m_minusTolerance;
	float m_plusTolerance;
};

class __INSPECTOIN_CLASS__ ObjectCriteria
{
public:
	ObjectCriteria();
	~ObjectCriteria();

	void Init();
	void Link(bool bSave, Database &db);
	void FromHost(const Share::SpecData& spec);
	void ToHost(Share::SpecData& o_spec);

	SpecCriteria m_ai;
	SpecCriteria m_marking;
};
