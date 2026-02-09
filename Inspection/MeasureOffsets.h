#pragma once

#include "basedef.h"

class Database;

namespace Inspection
{
	class __INSPECTOIN_CLASS__ MeasureOffsets
	{
	public:
		MeasureOffsets();
		~MeasureOffsets();

		double GetValue_mm(bool isUp, MeasureOffsetType type);
		double GetValue_mm(DefectType defectType);

		double& Get(bool isUp, MeasureOffsetType type);
		double* Get(DefectType defectType);

		void Reset();
		void LinkDataBase(bool bSave, Database& db);
		void SetValue_mm(DefectType defectType, double value);

		double m_shoulderLineOffsetTop_mm;
		double m_shoulderLineOffsetBot_mm;

		double m_cellLeftLength_mm;
		double m_cellRightLength_mm;//20220625 [jjh]
		double m_cellTopLength2_mm;
		double m_cellBottomLength2_mm;
		double m_misalign_mm;
		double m_mismatch_mm;
		double m_tabHeight_mm;
		double m_topTabHeight2_mm;
		double m_bottomTabHeight2_mm;
		double m_tabPitch_mm;
		double m_tabSide_mm;
		double m_tabWidth_mm;
		double m_vhomeHeight_mm;
		double m_vhomeHeightBot_mm;
		double m_vhomeTiltTop_mm;
		double m_vhomeTiltBot_mm;

		double m_frontCuttingHeightTop_mm;
		double m_frontCuttingHeightBot_mm;
		double m_backCuttingHeightTop_mm;
		double m_backCuttingHeightBot_mm;

		double m_frontoverlay_mm;
		double m_backoverlay_mm;
		double m_frontinsulation_mm;
		double m_backinsulation_mm;
		double m_frontbotoverlay_mm;
		double m_backbotoverlay_mm;
	};
}
