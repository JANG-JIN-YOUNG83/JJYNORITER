#pragma once
#include <afxmt.h>

class CCalMap_Distortion_Impl
{
public:
	CCalMap_Distortion_Impl(void);
	~CCalMap_Distortion_Impl(void);

	BOOL IsLoadSuccess() const;
	BOOL Load(LPCTSTR szFilePath, long nPredictionSizeX, long nPredictionSizeY);
	BOOL Save(LPCTSTR szFilePath) const;
	BOOL SetTransform(long nPointNum, const IPVM::Point_32f_C2* i_pSrcPoint, const IPVM::Point_32f_C2* i_pDstPoint);

	BOOL Modify(const IPVM::Image_8u_C1 &i_Image, CPoint i_ptStart, IPVM::Image_8u_C1 &o_Image);
	//BOOL Modify(const IV_dataImage_8u_C3 &i_Image, CPoint i_ptStart, IV_dataImage_8u_C3 &o_Image);
	BOOL Modify(const IPVM::Point_32f_C2& i_ptPoint, IPVM::Point_32f_C2& o_ptPoint);

	CCalMap_Distortion_Impl& operator = (const CCalMap_Distortion_Impl& Object);

private:
	IPVM::Matrix		m_mtxRef;
	IPVM::Image_32f_C1	m_mtxRefX;
	IPVM::Image_32f_C1	m_mtxRefY;
	BOOL				m_bLoadMatrixFail;
	CCriticalSection	m_csDataLock;

	BOOL LoadMatrixFile(LPCTSTR szPathName, IPVM::Matrix &matrix);
	BOOL Modify_Old(const IPVM::Image_8u_C1 &i_ImageGray, CPoint i_ptStart, IPVM::Image_8u_C1 &o_ImageGray);
	BOOL Modify_New(const IPVM::Image_8u_C1 &i_ImageGray, CPoint i_ptStart, IPVM::Image_8u_C1 &o_ImageGray);
	void CreateImageMap(long nSizeX, long nSizeY);
};
