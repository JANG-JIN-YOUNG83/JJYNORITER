#pragma once

#include "basedef.h"
#include "OptionCriteria.h"

class Database;

class __INSPECTOIN_CLASS__ OptionCriteriaList
{
public:
	OptionCriteriaList();
	~OptionCriteriaList();

	ObjectCriteria* Find(DefectType defectType);

	void OldLinkDataBase(bool bSave, bool isUpCamera, Database& db);
	void LinkDataBase(bool bSave, Database& db);

private:
	ObjectCriteria m_tabWidth;
	ObjectCriteria m_tabHeight;
	ObjectCriteria m_topTabHeight2;
	ObjectCriteria m_bottomTabHeight2;
	ObjectCriteria m_cellLeftLength;
	ObjectCriteria m_cellRightLength;
	ObjectCriteria m_cellTopLength2;
	ObjectCriteria m_cellBottomLength2;
	ObjectCriteria m_cellPitch;
	ObjectCriteria m_VHomeTiltTop;
	ObjectCriteria m_VHomeTiltBot;
	ObjectCriteria m_mismatch;
	ObjectCriteria m_shoulderLineTop;
	ObjectCriteria m_shoulderLineBot;
	ObjectCriteria m_tabShape;
	ObjectCriteria m_tabSide;
	ObjectCriteria m_mislalign;
	ObjectCriteria m_frontCuttingHeightTop;	// um
	ObjectCriteria m_frontCuttingHeightBot;	// um
	ObjectCriteria m_backCuttingHeightTop;	// um
	ObjectCriteria m_backCuttingHeightBot;	// um
	ObjectCriteria m_topVHomeHeight;
	ObjectCriteria m_bottomVHomeHeight;
	ObjectCriteria m_frontoverlay;
	ObjectCriteria m_backoverlay;
	ObjectCriteria m_frontinsulation;
	ObjectCriteria m_backinsulation;
	ObjectCriteria m_frontbotoverlay;
	ObjectCriteria m_backbotoverlay;
};
