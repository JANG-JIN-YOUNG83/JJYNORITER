#pragma once
#include "../pch.h"
#include <vector>
#include ".\libxl-3.5.3.0\libxl.h"
#include <../../iVisionMadangSDK/include/Types/Image_8u_C1.h>
#include <../../iVisionMadangSDK/include/Types/Image_8u_C3.h>
//#include "..\stdafx.h"
using namespace libxl;
using namespace std;

enum DIRb
{
	DIR_X=0,DIR_Y,DIR_ALL
};

enum TITLE
{
	FILEPATHNAME=1,TOTALIMAGE, REJECTNAME
};
#define REJECTNAMETITLE 1


//struct SBasicImage;
class CExcelWrapClass
{
public:
	void Create(CRect ImageROI,long nZoomSize);
	CExcelWrapClass(void);
	~CExcelWrapClass(void);
protected:
	CPoint			m_cpStep;	//row ,column
	IPVM::Image_8u_C1		m_imgSrc;	// 실제 사용할 원본 이미지
    IPVM::Image_8u_C1		m_imgDst; // 파일 저장에 사용할 Resize 된 이미지
    IPVM::Image_8u_C1		m_imgTemp; // 불량 위치가 Resize 된 이미지

	IPVM::Image_8u_C1 m_imgBright; // Bright Crop Image
    IPVM::Image_8u_C1 m_imgDark; // Dark Crop Image
    IPVM::Image_8u_C1 m_imgBin; // Binary Image

	long			m_nRejectList;//검사 항목의 갯수
	long			m_nZoomSize;
	CRect			m_rectOrigImageSize;
	CRect			m_rectSaveImageSize;
	vector<CRect>	m_recReject;
	CRect			m_rectOrigSize;
	CRect			m_rectSaveReject;//m_rect  m_recReject원래 영역에서 40만큼 확장 된 영역
	CRect			m_rtOutImageOffset;	//Reject ROI 크기 가공후 사이즈가 이미지 크기를 벗어날경우 다시 안쪽으로 밀어넣기 위한 Offset

	long m_nRowSize;
	long m_nColumnSize;
	
	Book* m_pBook;
	Sheet* m_pSheet;
	
	Format* m_format;
	Format* m_format1;

public:
	void Next_X(long nSum=1){
		m_cpStep.x+=nSum;
	}
	void Next_Y(long nSum=1){
		m_cpStep.y+=nSum;
	}
	long GetPositionX()
	{
		return m_cpStep.x;
	}
	long GetPositionY()
	{
		return m_cpStep.y;
	}
	void Step_Reset(long nDIR)
	{
		switch (nDIR)
		{
		case DIR_X:
			m_cpStep.x=0;
			break;
		case DIR_Y:
			m_cpStep.y=0;
			break;
		case DIR_ALL:
			m_cpStep.SetPoint(0,0);
			break;
		default :
			break;
		}
	}
	void Savexlsfile(LPCTSTR szPathName, BOOL bRelease);
    void SetSourceImage(IPVM::Image_8u_C1& imgSrc);
    void SetDstImage(IPVM::Image_8u_C1& imgDst, IPVM::Image_8u_C1& imgTemp);
	void SetRejectROI(const vector<CRect> &rtReject);
	void RejectListName(const vector<CString> &strRejectName);

	void SetBrightImage(IPVM::Image_8u_C1& imgBright);
    void SetDarkImage(IPVM::Image_8u_C1& imgDark);
    void SetBinImage(IPVM::Image_8u_C1& imgBin);
	void AddImageFileData(long nRow, LPCTSTR szCellName, LPCTSTR szImageFileName_Dark, LPCTSTR szImageFileName_Bright, vector<CString> vecResultData, vector<long> vecResultValue);
    void AddImageFileDatas(long nRow, LPCTSTR szCellName, LPCTSTR szImageFileName_Dark, LPCTSTR szImageFileName_Bright, LPCTSTR szImageFileName_Bin, vector<CString> vecResultData, CString strDefectResult, IPVM::Rect ROIrt);
    void AddImageFileDatas(long nRow, LPCTSTR szCellName, LPCTSTR szImageFileName_Dark, LPCTSTR szImageFileName_Bright,
        vector<CString> vecResultData, CString strDefectResult, IPVM::Rect ROIrt, long nIndex, BOOL bTotal = FALSE);
    void AddImageFileDatas(long nRow, LPCTSTR szCellName, LPCTSTR szImageFileName_Dark, LPCTSTR szImageFileName_Bright,  LPCTSTR szImageFileName_Ori, IPVM::Rect DefectROIrt, vector<CString> vecResultData, IPVM::Rect CropROIrt, IPVM::Rect ROIrt, long nIndex, BOOL bTotal = FALSE);
    //void AddImageFileDatas(long nRow, LPCTSTR szCellName, LPCTSTR szImageFileName_Dark, LPCTSTR szImageFileName_Bright, vector<CString> vecResultData, CString strDefectResult, IPVM::Rect ROIrt, long nIndex);
	//void AddRejectData(long nRow, LPCTSTR szImageFileName, LPCTSTR szInspName, SBlob sBlob, CString strCriteriaName, vector<CString> vecInspInfo);
	
	IPVM::Image_8u_C1& SaveJPGImage(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, BOOL bCrop = FALSE);
    IPVM::Image_8u_C1& SaveJPGImage_DefectROI(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, IPVM::Rect ROIrt, IPVM::Rect ROIDefectrt);
    IPVM::Image_8u_C1& SaveJPGImage_Bright(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, IPVM::Rect ROIrt, BOOL bCrop = FALSE);
    IPVM::Image_8u_C1& SaveJPGImage_Dark(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, IPVM::Rect ROIrt,  BOOL bCrop = FALSE);
    IPVM::Image_8u_C1& SaveJPGImage_Bin(LPCTSTR szImageFileName, long nImageWidth, long nImageHeigh, IPVM::Rect ROIrt,  BOOL bCrop = FALSE);
    void AddPicture(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, long nCol = 0, BOOL bCrop = FALSE);
    void SetBlobImage(IPVM::Image_8u_C1& imgDst, IPVM::Image_8u_C1& imgTemp, CRect rtBlob);

	IPVM::Image_8u_C1& SaveJPGImage_Bright_NoSuburb(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight,
        IPVM::Rect ROIrt, BOOL bCrop = FALSE, BOOL bTotal = FALSE);
    IPVM::Image_8u_C1& SaveJPGImage_Dark_NoSuburb(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight,
        IPVM::Rect ROIrt, BOOL bCrop = FALSE, BOOL bTotal = FALSE);
    void AddTextDatas(long nRow, LPCTSTR szCellName, vector<CString> vecResultData, CString strDefectResult, long nIndex);

	//Draw 
	void Image_RectDraw(IPVM::Image_8u_C1& Image, const vector<CRect>& rectROI, long nThickness);
    void Image_LineDraw(IPVM::Image_8u_C1& Image, const CPoint& ImagePos, long nDir);
    void Image_MultiLineDraw(IPVM::Image_8u_C1& Image, const CPoint& ImagePos, long nDir, long nThickness);
    static long m_nIndex;
	//void RectROICopy();
};

