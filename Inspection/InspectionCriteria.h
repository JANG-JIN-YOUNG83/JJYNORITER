#pragma once
#include "basedef.h"

class Database;

namespace Inspection
{
	class __INSPECTOIN_CLASS__ InspectionCriteria
	{
	public:
		InspectionCriteria(bool useStandardValue);
		~InspectionCriteria();

		void FromHost(const Share::SpecData& data);
		void Reset();
		void LinkDataBase(bool bSave, Database& db);
		void LoadDataBaseOld(Database& db, LPCTSTR useMin, LPCTSTR useMax, LPCTSTR valueMin, LPCTSTR valueMax);
		void LoadDataBaseOld(Database& db, LPCTSTR valueMin, LPCTSTR valueMax);
		void LoadDataBaseOld(Database& db, LPCTSTR standard, LPCTSTR valueMin, LPCTSTR valueMax);

		bool In(double value_mm) const;
		void ToHost(Share::SpecData& o_data) const;

		double m_standardValue_mm;
		double m_specMin_mm;		// Real Min = StandardValue + spec min
		double m_specMax_mm;		// Real Max = StandardValue + spec max

		bool m_useStandardValue;
	};
}
