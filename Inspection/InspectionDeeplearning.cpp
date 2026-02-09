#include "stdafx.h"
#include "InspectionDeeplearning.h"
#include "InspectionCell.h"
#include "InspectionCellAlign.h"
#include "DebugInfo.h"
#include "Accessor.h"
#include "UnionCellResult.h"

#include "Types/LineEq_32f.h"
#include "Algorithm/DataFitting.h"
#include "Types/LineEq_64f.h"
#include "Algorithm/Geometry.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "Algorithm/ImageProcessing.h"
#include <math.h>

#include "Algorithm/ImageProcessing.h"
#include "Algorithm/DataFitting.h"
#include "Algorithm/Geometry.h"
#include "Algorithm/EdgeDetection.h"
#include "Algorithm/EdgeDetectionFilter.h"
#include "Algorithm/EdgeDetectionDebugInfo.h"
#include "Algorithm/Mathematics.h"
#include "Base/LoggerInterface.h"
#include "Types/Point_32f_C2.h"
#include "Types/Point_32f_C3.h"
#include "Types/Image_8u_C1.h"
#include "Types/Point_64f_C2.h"
#include "Types/LineEq_64f.h"
#include "Types/LineEq_32f.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInspectionDeeplearning::CInspectionDeeplearning(CProcessingBase* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
	: CInspectionBase(parent, ProcItemCategory::measure, logger, loggerResult)
	, m_parent((CInspectionCell*) parent)
{
}

CInspectionDeeplearning::~CInspectionDeeplearning()
{
}

bool CInspectionDeeplearning::IsInspectionEnabled() const
{
	return true;
}

void CInspectionDeeplearning::ResetInspection()
{
}

void CInspectionDeeplearning::PrepareDebugInfo(Inspection::DebugInfo* debugInfo)
{
	//debugInfo->AddItem(LanguageDef::Debug_Coater_EdgeDetectPoint);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_A_FullLength);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_B_LeftCoating);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_C_RightCoating);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_D_LeftFoil);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_E_MiddleFoil);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_E1_MiddleFoilLeftSlitting);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_E2_MiddleFoilRightSlitting);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_F_RightFoil);



	//debugInfo->AddItem(LanguageDef::Debug_LinePoint);
	//debugInfo->AddItem(LanguageDef::Debug_VHomeTiltPosition);
	//debugInfo->AddItem(LanguageDef::Debug_TabAreaForHeight);
	//debugInfo->AddItem(LanguageDef::Debug_CellPitch);
	//debugInfo->AddItem(LanguageDef::Debug_CellLength);
	//debugInfo->AddItem(LanguageDef::Debug_CellLength2);
	//debugInfo->AddItem(LanguageDef::Debug_TabWidth);
	//debugInfo->AddItem(LanguageDef::Debug_TabHeight);
	//debugInfo->AddItem(LanguageDef::Debug_TabHeight2);
	//debugInfo->AddItem(LanguageDef::Debug_Misalign);
	//debugInfo->AddItem(LanguageDef::Debug_VHomeTilt);
	//debugInfo->AddItem(LanguageDef::Debug_TabSide);
	//debugInfo->AddItem(LanguageDef::Debug_CuttingHeightTop);
	//debugInfo->AddItem(LanguageDef::Debug_CuttingHeightBot);
	//debugInfo->AddItem(LanguageDef::Debug_VHomeHeight);
}

bool CInspectionDeeplearning::ProcInspection(Inspection::DebugInfo* debugInfo)
{
    m_resultValue = II_RESULT_VALUE::PASS;
	return true;
}

void CInspectionDeeplearning::ProcInspectionEnd(Inspection::DebugInfo* debugInfo)
{
	if (FALSE == m_parent->m_isTest) return;
}

void CInspectionDeeplearning::LinkDataBase(bool bSave, Database &dbJob)
{
	m_para.LinkDataBase(bSave, m_isUp, dbJob);
}

bool CInspectionDeeplearning::CopyDataBase(CProcessingBase* src)
{
	auto *ptr = dynamic_cast<CInspectionDeeplearning *>(src);
	if (ptr == nullptr)
	{
		ASSERT(!_T("���� Ÿ���� Inspection�� ���簡 �����ϴ�"));
		return false;
	}

	auto& rhs = *ptr;

	m_pixelToUmX = rhs.m_pixelToUmX;
	m_pixelToUmY = rhs.m_pixelToUmY;

	m_para = rhs.m_para;

	m_isUp = rhs.m_isUp;

	return true;
}