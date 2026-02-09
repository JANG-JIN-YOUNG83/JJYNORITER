#pragma once

#include "../VisionModule/Base/ParaBool.h"
#include "../VisionModule/Base/ParaByte.h"
#include "../VisionModule/Base/ParaLong.h"
#include "../VisionModule/Base/ParaDouble.h"
#include "../VisionModule/Base/ParaFloat.h"
#include "../VisionModule/Base/ParaRect.h"
#include "../VisionModule/Base/ParaEdgeDetection.h"
#include "Types/Rect.h"

#define MAX_CAMERA	2
class Database;

class CInspectionCellAlignPara
{
public:
	CInspectionCellAlignPara();
	~CInspectionCellAlignPara();

public:
	void LinkDataBase(bool bSave, Database &db);
	void SetInitialDetectROI(int nIndex, IPVM::Rect& rtDetect);
public:
	//현식
	Base::ParaLong m_nLineCountMeasure;
	Base::ParaLong m_nLineCountBlob;

	Base::ParaBool m_bUseCenterROISplitMeasure;
    Base::ParaBool m_bUseCenterROISplitBlob;

	std::vector<Base::ParaRect> m_vecrtDetectLineMeasure;
	std::vector<Base::ParaEdgeDetection> m_vecDetectLineParaMeasure;

	std::vector<Base::ParaRect> m_vecrtDetectLineBlob;
	std::vector<Base::ParaEdgeDetection> m_vecDetectLineParaBlob;

	/*std::vector<Base::ParaRect> m_vecrtDetectLineMSA;
	std::vector<Base::ParaEdgeDetection> m_vecDetectLineParaMSA;*/
	
	Base::ParaBool m_bCellDesignShowOverlay;

	Base::ParaLong m_imageUseIndex;
	Base::ParaBool m_bUseInspection;
    Base::ParaLong m_nSelectImageType;
    Base::ParaLong m_nInspectionType;
};
