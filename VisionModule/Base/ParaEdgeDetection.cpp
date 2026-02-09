#include "stdafx.h"
#include "ParaEdgeDetection.h"
#include "Algorithm/EdgeDetectionPara.h"
#include "Algorithm/EdgeDetectionFilter.h"
#include "../../CompanySharedLibrary/Persistence/Database.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Base;

ParaEdgeDetection::ParaEdgeDetection(LPCTSTR name)
	: m_name(name)
{
	m_ArrName[enParaEdgeDetection::HalfFilterLength].	Format(_T("%s_HalfFilterLength"), m_name);
	m_ArrName[enParaEdgeDetection::GaussianDeviation].	Format(_T("%s_GaussianDeviation"), m_name);
	m_ArrName[enParaEdgeDetection::FindFirstEdge].		Format(_T("%s_FindFirstEdge"), m_name);
	m_ArrName[enParaEdgeDetection::EdgeType].			Format(_T("%s_EdgeType"), m_name);
	m_ArrName[enParaEdgeDetection::SearchDirection].	Format(_T("%s_SearchDirection"), m_name);
	m_ArrName[enParaEdgeDetection::EdgeThreshold].		Format(_T("%s_EdgeThreshold"), m_name);
	
	m_ArrName[enParaEdgeDetection::RansacThreshold].Format(_T("%s_RansacThreshold"), m_name);
	m_ArrName[enParaEdgeDetection::DetectCount].Format(_T("%s_DetectCount"), m_name);

	m_ArrName[enParaEdgeDetection::ImageTypeDark].Format(_T("%s_ImageTypeDark"), m_name);
	
	m_HalfFilterLength = 6;
	m_GaussianDeviation = 1.5;
	m_FindFirstEdge = false;
	m_DetectType = 1;
	m_SearchDirection = 1;
	m_EdgeThreshold = 1.0f;
	m_RansacThreshold = 10;
	m_DetectCount = 10;
    m_ImageTypeDark = true;
    m_EdgeDetectMode = 0;
}

ParaEdgeDetection::ParaEdgeDetection(LPCTSTR name, long nHalfFilterLength, float fGaussianDeviation, bool bFirstEdge, long nDetectType, long nSearchDirection, float fEdgeThreshold, float fRansacThreshold, long nDetectCount, long nImageTypeDark, long nEdgeDetectMode)
	: m_name(name)
	, nCount(1)
{
	m_ArrName[enParaEdgeDetection::HalfFilterLength].Format(_T("%s_HalfFilterLength"), m_name);
	m_ArrName[enParaEdgeDetection::GaussianDeviation].Format(_T("%s_GaussianDeviation"), m_name);
	m_ArrName[enParaEdgeDetection::FindFirstEdge].Format(_T("%s_FindFirstEdge"), m_name);
	m_ArrName[enParaEdgeDetection::EdgeType].Format(_T("%s_EdgeType"), m_name);
	m_ArrName[enParaEdgeDetection::SearchDirection].Format(_T("%s_SearchDirection"), m_name);
	m_ArrName[enParaEdgeDetection::EdgeThreshold].Format(_T("%s_EdgeThreshold"), m_name);
	m_ArrName[enParaEdgeDetection::RansacThreshold].Format(_T("%s_RansacThreshold"), m_name);
	m_ArrName[enParaEdgeDetection::DetectCount].Format(_T("%s_DetectCount"), m_name);
    m_ArrName[enParaEdgeDetection::ImageTypeDark].Format(_T("%s_ImageTypeDark"), m_name);

	m_HalfFilterLength = nHalfFilterLength;
	m_GaussianDeviation = fGaussianDeviation;
	m_FindFirstEdge = bFirstEdge;
	m_DetectType = nDetectType;
	m_SearchDirection = nSearchDirection;
	m_EdgeThreshold = fEdgeThreshold;

	m_RansacThreshold = fRansacThreshold;
	m_DetectCount = nDetectCount;
    m_ImageTypeDark = nImageTypeDark;
    m_EdgeDetectMode = nEdgeDetectMode;
}

ParaEdgeDetection::~ParaEdgeDetection()
{

}

bool ParaEdgeDetection::Link(bool save, Database& db)
{

	bool success = db[m_ArrName[enParaEdgeDetection::HalfFilterLength]].Link(save, m_HalfFilterLength);
	success &= db[m_ArrName[enParaEdgeDetection::GaussianDeviation]].Link(save, m_GaussianDeviation);
	success &= db[m_ArrName[enParaEdgeDetection::FindFirstEdge]].Link(save, m_FindFirstEdge);
	success &= db[m_ArrName[enParaEdgeDetection::EdgeType]].Link(save, m_DetectType);
	success &= db[m_ArrName[enParaEdgeDetection::SearchDirection]].Link(save, m_SearchDirection);
	success &= db[m_ArrName[enParaEdgeDetection::EdgeThreshold]].Link(save, m_EdgeThreshold);
	success &= db[m_ArrName[enParaEdgeDetection::RansacThreshold]].Link(save, m_RansacThreshold);
	success &= db[m_ArrName[enParaEdgeDetection::DetectCount]].Link(save, m_DetectCount);
	success &= db[m_ArrName[enParaEdgeDetection::ImageTypeDark]].Link(save, m_ImageTypeDark);

	return success;
}

void ParaEdgeDetection::SetParameter(IPVM::EdgeDetectionPara& edgePara, IPVM::EdgeDetectionFilter& edgeFilter) const
{
	edgeFilter.SetParameter(m_HalfFilterLength, m_GaussianDeviation);
	edgePara.m_edgeType = (IPVM::EdgeType)m_DetectType;
	edgePara.m_findFirstEdge = m_FindFirstEdge;
	edgePara.m_edgeThreshold = m_EdgeThreshold;
	edgePara.m_searchDirection = (IPVM::SearchDirection)m_SearchDirection;
	
	// IPVM에 없다고 안쓸꺼야??
	//edgePara.m_RansacThreshold = m_RansacThreshold;
	//edgePara.m_DetectCount = m_DetectCount;
}