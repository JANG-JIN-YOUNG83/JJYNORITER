//#include "stdafx.h"
#include "pch.h"
#include "CalMap_Distortion.h"
#include "CalMap_Distortion_Impl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCalMap_Distortion::CCalMap_Distortion(void)
{
	m_pImpl = new CCalMap_Distortion_Impl;
}

CCalMap_Distortion::CCalMap_Distortion(const CCalMap_Distortion& Object)
{
	m_pImpl = new CCalMap_Distortion_Impl;
	*this = Object;
}

CCalMap_Distortion::~CCalMap_Distortion(void)
{
	delete m_pImpl;
}

BOOL CCalMap_Distortion::IsLoadSuccess() const
{
	return m_pImpl->IsLoadSuccess();
}

BOOL CCalMap_Distortion::Load(LPCTSTR szFilePath, long nPredictionSizeX, long nPredictionSizeY)
{
	return m_pImpl->Load(szFilePath, nPredictionSizeX, nPredictionSizeY);
}

BOOL CCalMap_Distortion::Save(LPCTSTR szFilePath) const
{
	return m_pImpl->Save(szFilePath);
}

BOOL CCalMap_Distortion::SetTransform(long nPointNum, const IPVM::Point_32f_C2* i_pSrcPoint, const IPVM::Point_32f_C2* i_pDstPoint)
{
	return m_pImpl->SetTransform(nPointNum, i_pSrcPoint, i_pDstPoint);
}

BOOL CCalMap_Distortion::Modify(const IPVM::Image_8u_C1 &i_Image, CPoint i_ptStart, IPVM::Image_8u_C1 &o_Image)
{
	return m_pImpl->Modify(i_Image, i_ptStart, o_Image);
}
//
//BOOL CCalMap_Distortion::Modify(const IV_dataImage_8u_C3 &i_Image, CPoint i_ptStart, IV_dataImage_8u_C3 &o_Image)
//{
//	return m_pImpl->Modify(i_Image, i_ptStart, o_Image);
//}

BOOL CCalMap_Distortion::Modify(const IPVM::Point_32f_C2& i_ptPoint, IPVM::Point_32f_C2& o_ptPoint)
{
	return m_pImpl->Modify(i_ptPoint, o_ptPoint);
}

CCalMap_Distortion& CCalMap_Distortion::operator = (const CCalMap_Distortion& Object)
{
	*m_pImpl = *Object.m_pImpl;

	return *this;
}
