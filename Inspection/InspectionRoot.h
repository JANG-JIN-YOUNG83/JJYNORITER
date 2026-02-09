#pragma once

#include "basedef.h"
#include "InspectionBase.h"
#include "InspectionCell.h"
#include "MeasureOffsets.h"
#include "ProcessingItemDef.h"
#include "../SharedModule/CommonShare/DefectDef.h"
#include "../SharedModule/VisionHostShare//LoginInfo.h"

#include <deque>
#include <map>

class Database;

namespace Inspection
{
	class __INSPECTOIN_CLASS__ Root
	{
	public:
		virtual ~Root();

	public:
		void LinkDataBase(bool bSave, Database& dbJob);
		void LinkDataBaseCommon(bool bSave, Database& dbJob);
		bool CopyDataBase(CProcessingBase* src);

		CInspectionCell* GetInspectionCell(bool isUp);
		CProcessingBase* GetProcessingItem(bool isUp, ProcItemCategory categoryIndex);
		void SetPixelToUmX(double xup, double xdown);
		void SetPixelToUmY(double y);
		void SetCellCuttingMargin(long nMargin);
		void SetCellDesign(const CELL_DESIGN& design);
		void SetVersion(int nGenerationversion, int nSite);
        bool ShowDlg(int nLanguage, std::map<long, std::vector<CString>> mapExplainParamDimension, SetupImage& setupImage, long nType);
		void SetSurfaceOption(const bool& bUseISI);

		//Area À§ÇØ¼­ Ãß°¡
		CInspectionCell* GetInspectionCell(long nSide);
        CProcessingBase* GetProcessingItem(long nSide, ProcItemCategory categoryIndex);
        void SetPixelToUmX(double x, long nSide);
        void SetPixelToUmY(double y, long nSide);
	
	public:
		CELL_DESIGN		m_cellDesign;
		int				m_nAnodeCathode;	// ¾ç±Ø 0, À½±Ø 1
		int				m_nInsulateType;
		long			m_nCellCuttingMargin;
		Share::enLoginMode m_loginMode;

	private:
		double			m_pixelToUmXUp;
		double			m_pixelToUmXDown;
		double			m_pixelToUmY;
		CInspectionCell	m_inspectionCellUp;
		CInspectionCell	m_inspectionCellDown;
        CInspectionCell m_inspectionCell[4];

		Root(IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);

		friend class Accessor;
	};
}
