#pragma once
#include "basedef.h"
#include "InspectionCriteria.h"

class Database;

namespace Inspection
{
	class __INSPECTOIN_CLASS__ InspectionCriteriaList
	{
	public:
		InspectionCriteriaList();
		~InspectionCriteriaList();

		void LinkDataBase(bool bSave, Database& db);
		InspectionCriteria* Find(DefectType defectType);

		InspectionCriteria m_tabPitch;
		InspectionCriteria m_tabWidth;
		InspectionCriteria m_tabHeight;
		InspectionCriteria m_topTabHeight2;
		InspectionCriteria m_bottomTabHeight2;
		InspectionCriteria m_tabSide;
		InspectionCriteria m_mismatch;
		InspectionCriteria m_cellLength;
		InspectionCriteria m_cellTopLength2;
		InspectionCriteria m_cellBottomLength2;
		InspectionCriteria m_shoulderLineTop;
		InspectionCriteria m_shoulderLineBot;
		InspectionCriteria m_vhomeHeight;
		InspectionCriteria m_vhomeTiltTop;
		InspectionCriteria m_vhomeTiltBot;
		InspectionCriteria m_misalign;
		InspectionCriteria m_frontCuttingHeightTop;
		InspectionCriteria m_frontCuttingHeightBot;
		InspectionCriteria m_backCuttingHeightTop;
		InspectionCriteria m_backCuttingHeightBot;
		InspectionCriteria m_frontOverlay;
		InspectionCriteria m_backOverlay;
		InspectionCriteria m_frontInsulation;
		InspectionCriteria m_backInsulation;
		InspectionCriteria m_frontBotOverlay;
		InspectionCriteria m_backBotOverlay;
		//InspectionCriteria m_frontBotInsulation;
		//InspectionCriteria m_backBotInsulation;
	};
}
