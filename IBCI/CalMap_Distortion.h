#pragma once

#ifdef __IDSSLIBRARY_WRAPPER_CLASS_EXPORT__
#define __IDSSLIBRARY_WRAPPER_CLASS__	__declspec(dllexport)
#else
#define __IDSSLIBRARY_WRAPPER_CLASS__	__declspec(dllimport)
#endif

class CCalMap_Distortion_Impl;
//class IPVM::Point_32f_C2;
//class IPVM::Image_8u_C1;
//class IV_dataImage_8u_C3;
//IPVM::Point_32f_C2  => IPVM::Point_32f_C2
// IPVM::Image_8u_C1 => IPVM::Image_8u_C1
class __IDSSLIBRARY_WRAPPER_CLASS__ CCalMap_Distortion
{
public:
	CCalMap_Distortion(void);
	CCalMap_Distortion(const CCalMap_Distortion& Object);

	~CCalMap_Distortion(void);

	BOOL IsLoadSuccess() const;
	BOOL Load(LPCTSTR szFilePath, long nPredictionSizeX=0, long nPredictionSizeY=0);
	BOOL Save(LPCTSTR szFilePath) const;
	BOOL SetTransform(long nPointNum, const IPVM::Point_32f_C2* i_pSrcPoint, const IPVM::Point_32f_C2* i_pDstPoint);

	BOOL Modify(const IPVM::Image_8u_C1 &i_Image, CPoint i_ptStart, IPVM::Image_8u_C1 &o_Image);
	//BOOL Modify(const IV_dataImage_8u_C3 &i_Image, CPoint i_ptStart, IV_dataImage_8u_C3 &o_Image);
	BOOL Modify(const IPVM::Point_32f_C2& i_ptPoint, IPVM::Point_32f_C2& o_ptPoint);

	CCalMap_Distortion& operator = (const CCalMap_Distortion& Object);

private:
	CCalMap_Distortion_Impl* m_pImpl;
};
