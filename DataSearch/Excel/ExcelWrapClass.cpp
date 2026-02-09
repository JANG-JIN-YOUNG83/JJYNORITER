//#include "\"
//#include "stdafx.h"
#include "../pch.h"
#include "ExcelWrapClass.h"


//#include "..\dA_Modules\dA_ItpWraper\ItpWrapper.h"
//#include "..\dA_Modules\dA_ItpWraper\Image_8u_C1.h"
//#include "..\dA_Modules\dA_ItpWraper\Image_8u_C3.h"

//#include "..\dA_Modules\dA_Base\BasicImage.h"
//#include "..\dA_Modules\dA_Base\UserImage.h"
#include <Algorithm>
#include <Algorithm\ImageProcessing.h>
#include <../../iVisionMadangSDK/include/Types/Rect.h>
#include <../../iVisionMadangSDK/include/Types/Image_8u_C1.h>
#include <../../iVisionMadangSDK/include/Types/Image_8u_C3.h>
#include <../../iVisionMadangSDK/include/Types/Point_8u_C3.h>

#include <../../iVisionMadangSDK/include/Widget/ImageView.h>
#include <../../iVisionMadangSDK/include/Algorithm/ImageProcessing.h>
//#include <Base\Rect.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const double EXCELROW    = 0.1406;
const double EXCELCOLUMN = 0.7466;
const double WRITEAREA = 20;

#define WIDTHBYTE(bits)	(((bits)+31)/32*4)

CExcelWrapClass::CExcelWrapClass(void)
{
	m_pBook=NULL;
	m_nRowSize=128;//256
	m_nColumnSize=188;//375
	m_nColumnSize=(long)(m_nColumnSize+WRITEAREA);//문자 넣을공간.
}


CExcelWrapClass::~CExcelWrapClass(void)
{
	if(m_pBook)
	{
		m_pBook->release();
		//delete m_pBook; // delete가 아니라 release가 맞는듯...
		m_pBook = NULL;
	}
}
void CExcelWrapClass::Create(CRect ImageROI,long nZoomSize)
{
	m_rectOrigImageSize = ImageROI;
	m_rectSaveImageSize = m_rectOrigImageSize.MulDiv(1,nZoomSize);
	m_nZoomSize = nZoomSize;

	m_nRowSize = (long)(m_rectSaveImageSize.Width()*EXCELROW+0.5);
	m_nColumnSize = (long)(m_rectSaveImageSize.Height()*EXCELCOLUMN+WRITEAREA+0.5);

	if(m_pBook)
	{
		m_pBook->release();
		//delete m_pBook; // delete가 아니라 release가 맞는듯...
		m_pBook = NULL;
	}
	m_pBook = xlCreateXMLBook();
	m_pBook->setKey(L"Hyunmin Lee", L"windows-2f2d2c0605c0e10260b76164adk7dbp7");
	m_pSheet = m_pBook->addSheet(_T("검사결과"));

	m_format = m_pBook->addFormat();
	m_format->setAlignH(ALIGNH_CENTER);
	m_format->setBorderTop(BORDERSTYLE_THIN);
	m_format->setAlignV(ALIGNV_CENTER);
	m_format1 = m_pBook->addFormat();
	//m_format1->setAlignH(ALIGNH_CENTER);
	m_format1->setBorderTop(BORDERSTYLE_THIN);
	m_format1->setAlignV(ALIGNV_CENTER);
}

void CExcelWrapClass::Savexlsfile(LPCTSTR szPathName, BOOL bRelease)
{
	m_pBook->save(szPathName);

	if(bRelease == FALSE)
	{
		return;
	}

	if(m_pBook)
	{
		m_pBook->release();
		m_pBook = xlCreateXMLBook();
		m_pBook->setKey(L"Hyunmin Lee", L"windows-2f2d2c0605c0e10260b76164adk7dbp7");
		m_pSheet = m_pBook->addSheet(_T("검사결과"));
	}
}

void CExcelWrapClass::SetBrightImage(IPVM::Image_8u_C1& imgBright)
{
    if (imgBright.GetBitCountPerPixel() != 8)
    {
        AfxMessageBox(_T("Only Input GrayImage Plz! "));
        return;
    }

    CRect rtROI(0, 0, (int)imgBright.GetSizeX(), (int)imgBright.GetSizeY());
    long n = imgBright.GetSizeY();
    //m_imgSrc.SetImage(nullptr, _T("Source"), imgSrc.pbyImage, rtROI, (long)imgSrc.bpp);
    m_imgBright = imgBright;
    //m_imgSrc;
    //넓이 엑셀:10 이미지:80     1픽셀 = 0.125
    //행의 높이 140.. 넓이는 180 1픽셀 = 0.77엑셀
}

void CExcelWrapClass::SetDarkImage(IPVM::Image_8u_C1& imgDark)
{
    if (imgDark.GetBitCountPerPixel() != 8)
    {
        AfxMessageBox(_T("Only Input GrayImage Plz! "));
        return;
    }

    CRect rtROI(0, 0, (int)imgDark.GetSizeX(), (int)imgDark.GetSizeY());

    //m_imgSrc.SetImage(nullptr, _T("Source"), imgSrc.pbyImage, rtROI, (long)imgSrc.bpp);
    m_imgDark = imgDark;
    //m_imgSrc;
    //넓이 엑셀:10 이미지:80     1픽셀 = 0.125
    //행의 높이 140.. 넓이는 180 1픽셀 = 0.77엑셀
}

void CExcelWrapClass::SetBinImage(IPVM::Image_8u_C1& imgBin)
{
    if (imgBin.GetBitCountPerPixel() != 8)
    {
        AfxMessageBox(_T("Only Input GrayImage Plz! "));
        return;
    }

    CRect rtROI(0, 0, (int)imgBin.GetSizeX(), (int)imgBin.GetSizeY());

    //m_imgSrc.SetImage(nullptr, _T("Source"), imgSrc.pbyImage, rtROI, (long)imgSrc.bpp);
    m_imgBin = imgBin;
    //m_imgSrc;
    //넓이 엑셀:10 이미지:80     1픽셀 = 0.125
    //행의 높이 140.. 넓이는 180 1픽셀 = 0.77엑셀
}
//영상 원본 크기를 넣어준다.
void CExcelWrapClass::SetSourceImage(IPVM::Image_8u_C1& imgSrc)
{
	if(imgSrc.GetBitCountPerPixel() != 8)
	{
		AfxMessageBox(_T("Only Input GrayImage Plz! "));
			return;
	}

    CRect rtROI(0, 0, (int)imgSrc.GetSizeX(), (int)imgSrc.GetSizeY());

	//m_imgSrc.SetImage(nullptr, _T("Source"), imgSrc.pbyImage, rtROI, (long)imgSrc.bpp);
    m_imgSrc = imgSrc;
    //m_imgSrc;
	//넓이 엑셀:10 이미지:80     1픽셀 = 0.125
	//행의 높이 140.. 넓이는 180 1픽셀 = 0.77엑셀
}


void CExcelWrapClass::SetDstImage(IPVM::Image_8u_C1& imgDst, IPVM::Image_8u_C1& imgTemp)
{
    m_imgDst = imgDst;
}

void CExcelWrapClass::SetBlobImage(IPVM::Image_8u_C1& imgDst, IPVM::Image_8u_C1& imgTemp, CRect rtBlob)
{
	//IPVM::Image_8u_C1 ipvmimgDst = imgDst;
	////IPVM::Rect rtROI(0, 0, imgDst.width, imgDst.height);
	//IPVM::Rect rtROI(rtBlob.left, rtBlob.top, rtBlob.right, rtBlob.bottom);
	////SBasicImage imgZoom(0,0, m_rectSaveImageSize.Width(), m_rectSaveImageSize.Height());
	//IPVM::Image_8u_C1 ipvmBlobimg;
	//ipvmBlobimg.Create(rtROI.Width(), rtROI.Height());

	//IPVM::Rect rtROI2(0, 0, rtROI.Width(), rtROI.Height());

	//IPVM::ImageProcessing::Copy(ipvmimgDst, rtROI, rtROI2, ipvmBlobimg);
	//memcpy(imgTemp.pbyImage, ipvmBlobimg.GetMem(), rtROI.Width()*rtROI.Height());

	//ipvmimgDst.SaveBmp(_T("D:\\ipvmimgDst.bmp"));
	//ipvmBlobimg.SaveBmp(_T("D:\\ipvmBlobimg.bmp"));
    m_imgTemp = imgDst;
	//m_imgDst.SetImage(nullptr, nullptr, imgDst.pbyImage, CRect(0,0,imgDst.width,imgDst.height), 8);
	//m_imgTemp.SetImage(nullptr, nullptr, ipvmBlobimg.GetMem(), rtBlob, 8);

}

void CExcelWrapClass::SetRejectROI(const vector<CRect> &rtReject)
{
	m_recReject.clear();
	for(long x = 0 ; x < rtReject.size() ; x++)
	{
		CRect rtTemp = rtReject[x].MulDiv(1,m_nZoomSize);
		rtTemp.InflateRect(4, 4);
		m_recReject.push_back(rtTemp);
	}

	// Crop 안하니까 주석해버림
	//m_rectOrigSize = rtReject;
	//m_rectSaveReject = rtReject;

	//const long nExtX = 40;
	//const long nExtY = 40;

	//m_rectSaveReject.InflateRect(nExtX, nExtY);

	//if(m_rectSaveReject.Width() % 4)
	//{
	//	long nOldw =  m_rectSaveReject.Width();
	//	long nNewW = (m_rectSaveReject.Width()+3)/4*4;

	//	m_rectSaveReject.right = m_rectSaveReject.right + (nNewW-nOldw);
	//}

	////음수 경우 예외처리
	//m_rectSaveReject.left	= max(m_rectSaveReject.left,0);
	//m_rectSaveReject.top	= max(m_rectSaveReject.top,0);
	//m_rectSaveReject.right	= max(m_rectSaveReject.right,0);		
	//m_rectSaveReject.bottom = max(m_rectSaveReject.bottom,0);

	m_rtOutImageOffset = CRect(0,0,0,0);
}

void CExcelWrapClass::AddImageFileDatas(long nRow, LPCTSTR szCellName, LPCTSTR szImageFileName_Dark,
    LPCTSTR szImageFileName_Bright, LPCTSTR szImageFileName_Ori, IPVM::Rect DefectROIrt, vector<CString> vecResultData,
    IPVM::Rect CropROIrt, IPVM::Rect ROIrt, long nIndex,
    BOOL bTotal)
{
    Step_Reset(DIR_ALL);
    // m_pSheet->setCol(GetPositionX(), GetPositionX(), 50);
    // m_pSheet->setRow(GetPositionY(), 50);
    Next_Y(nRow + 1);

    Next_X(FILEPATHNAME);
    //long nNumber = 1;
    Format* formatColor = m_pBook->addFormat();
    {
        formatColor->setPatternForegroundColor(COLOR_WHITE);
    }

    m_pSheet->setCellFormat(50, 50, formatColor);
    //m_pSheet->setCellFormat(GetPositionY() - 1, 50, formatColor);
    //m_pSheet->setCellFormat(GetPositionY() - 1, 26, formatColor);

    Format* format1 = m_pBook->addFormat();
    
    IPVM::Image_8u_C3 ROIImage;
    CString strNum;

    Next_X();

    SaveJPGImage_Bright_NoSuburb(szImageFileName_Bright, 63, 140, CropROIrt, FALSE, TRUE);
    SaveJPGImage_Dark_NoSuburb(szImageFileName_Dark, 63, 140, CropROIrt, FALSE, TRUE);
    //AddPicture(szImageFileName_Dark, ROIrt.Width() / 2, ROIrt.Height() / 2, vecResultData.size());
    
    SaveJPGImage_DefectROI(szImageFileName_Ori, 63, 140, ROIrt, DefectROIrt);
    
    //for (long n = 0; n < nNumber; n++)
    //{
    //    //Next_X();
    //    strNum.Format(_T("%d"), nIndex);
    //    m_pSheet->writeStr(GetPositionY(), 1, strNum, format1);
    //    m_pSheet->writeStr(GetPositionY(), n + 2, vecResultData[n], format1);
    //}

    if (bTotal == TRUE)
    {
        for (long n = 0; n < vecResultData.size(); n++)
        {
            if (n == 0)
            {
                strNum.Format(_T("%d"), nIndex);
                m_pSheet->writeStr(GetPositionY(), 1, strNum, format1);
                m_pSheet->writeStr(GetPositionY(), n + 7, vecResultData[n], format1);
            }
            else
            {
                m_pSheet->writeStr(GetPositionY(), n + 7, vecResultData[n], format1);
            }
        }
    }
    else
    {
        for (long n = 0; n < vecResultData.size(); n++)
        {
            if (n == 0)
            {
                strNum.Format(_T("%d"), nIndex);
                m_pSheet->writeStr(GetPositionY(), 1, strNum, format1);
                m_pSheet->writeStr(GetPositionY(), n + 2, vecResultData[n], format1);
            }
            else
            {
                m_pSheet->writeStr(GetPositionY(), n + 4, vecResultData[n], format1);
            }
        }
    }

    //for (long n = 1; n < vecResultData.size(); n++)
    //{
    //    //Next_X();

    //    m_pSheet->writeStr(GetPositionY(), n + 4, vecResultData[n], format1);
    //}

    //SaveJPGImage(szImageFileName_Dark, 80, 140, FALSE);
}

void CExcelWrapClass::AddImageFileDatas(long nRow, LPCTSTR szCellName, LPCTSTR szImageFileName_Dark,
    LPCTSTR szImageFileName_Bright, vector<CString> vecResultData, CString strDefectResult, IPVM::Rect ROIrt,
    long nIndex, BOOL bTotal)
{
    Step_Reset(DIR_ALL);
   // m_pSheet->setCol(GetPositionX(), GetPositionX(), 50);
   // m_pSheet->setRow(GetPositionY(), 50);
    Next_Y(nRow + 1);

    Next_X(FILEPATHNAME);
    //long nNumber = 1;
    Format* formatColor = m_pBook->addFormat();
    //formatColor->setBorderTop(BORDERSTYLE_THIN);
    //formatColor->setBorderLeft(BORDERSTYLE_THIN);
    //formatColor->setBorderRight(BORDERSTYLE_THIN);
    //formatColor->setBorderBottom(BORDERSTYLE_THIN);
    //formatColor->setFillPattern(FILLPATTERN_SOLID);
    //if (strDefectResult == _T("NG"))
    //{

    //    formatColor->setFillPattern(FILLPATTERN_SOLID);
    //    formatColor->setPatternForegroundColor(COLOR_PINK);
    //}
    //else
    {
        formatColor->setPatternForegroundColor(COLOR_WHITE);
    }
    

    m_pSheet->setCellFormat(GetPositionY() - 1, 50, formatColor);
    //m_pSheet->setCellFormat(GetPositionY() - 1, 26, formatColor);

    Format* format1 = m_pBook->addFormat();
    //format1->setBorderTop(BORDERSTYLE_THIN);
    //format1->setBorderLeft(BORDERSTYLE_THIN);
    //format1->setBorderRight(BORDERSTYLE_THIN);
    //format1->setBorderBottom(BORDERSTYLE_THIN);
    //format1->setAlignV(ALIGNV_CENTER);

    //m_pSheet->setRow(GetPositionY()-1, 50);
    //m_pSheet->setCol(FILEPATHNAME,FILEPATHNAME,10);//일부로 고정하였다.
    //m_pSheet->setCol(TOTALIMAGE, TOTALIMAGE, 11);

    //m_pSheet->setCol(FILEPATHNAME, FILEPATHNAME, 20); //일부로 고정하였다.
    //   m_pSheet->setCol(TOTALIMAGE, TOTALIMAGE, 20);
    //m_pSheet->writeStr(GetPositionY(), FILEPATHNAME, szCellName, format1);
    //for (long n = 0; n < nNumber; n++)
    //{
    //    //Next_X();
    //    m_pSheet->writeStr(GetPositionY(), n + 1, vecResultData[n], format1);
    //}
    IPVM::Image_8u_C3 ROIImage;
    CString strNum;

    Next_X();

    SaveJPGImage_Bright_NoSuburb(szImageFileName_Bright, 63, 140, ROIrt, FALSE, bTotal);
    SaveJPGImage_Dark_NoSuburb(szImageFileName_Dark, 63, 140, ROIrt, FALSE, bTotal);
    //SaveJPGImage_Bright(szImageFileName_Bright, 63, 140, ROIrt, FALSE);
    //for (long n = 0; n < nNumber; n++)
    //{
    //    //Next_X();
    //    strNum.Format(_T("%d"), nIndex);
    //    m_pSheet->writeStr(GetPositionY(), 1, strNum, format1);
    //    m_pSheet->writeStr(GetPositionY(), n + 2, vecResultData[n], format1);
    //}

    if (bTotal == TRUE)
    {

        for (long n = 0; n < vecResultData.size(); n++)
        {
            if (n == 0)
            {
                strNum.Format(_T("%d"), nIndex);
                m_pSheet->writeStr(GetPositionY(), 1, strNum, format1);
                m_pSheet->writeStr(GetPositionY(), n + 4, vecResultData[n], format1);
            }
            else
            {
                m_pSheet->writeStr(GetPositionY(), n + 4, vecResultData[n], format1);
            }
            
        }
    }
    else
    {
        for (long n = 0; n < vecResultData.size(); n++)
        {
            if (n == 0)
            {
                strNum.Format(_T("%d"), nIndex);
                m_pSheet->writeStr(GetPositionY(), 1, strNum, format1);
                m_pSheet->writeStr(GetPositionY(), n + 2, vecResultData[n], format1);
            }
            else
            {
                m_pSheet->writeStr(GetPositionY(), n + 4, vecResultData[n], format1);
            }
        }
    }


    //for (long n = 1; n < vecResultData.size(); n++)
    //{
    //    //Next_X();

    //    m_pSheet->writeStr(GetPositionY(), n + 4, vecResultData[n], format1);
    //}

    //SaveJPGImage(szImageFileName_Dark, 80, 140, FALSE);
}
void CExcelWrapClass::AddImageFileDatas(long nRow, LPCTSTR szCellName, LPCTSTR szImageFileName_Dark,
    LPCTSTR szImageFileName_Bright, LPCTSTR szImageFileName_Bin, vector<CString> vecResultData, CString strDefectResult,
    IPVM::Rect ROIrt)
{
    Step_Reset(DIR_ALL);
    // m_pSheet->setCol(GetPositionX(), GetPositionX(), 50);
    // m_pSheet->setRow(GetPositionY(), 50);
    Next_Y(nRow + 1);

    Next_X(FILEPATHNAME);
    long nNumber = 2;
    Format* formatColor = m_pBook->addFormat();
    //formatColor->setBorderTop(BORDERSTYLE_THIN);
    //formatColor->setBorderLeft(BORDERSTYLE_THIN);
    //formatColor->setBorderRight(BORDERSTYLE_THIN);
    //formatColor->setBorderBottom(BORDERSTYLE_THIN);
    //formatColor->setFillPattern(FILLPATTERN_SOLID);
    //if (strDefectResult == _T("NG"))
    //{

    //    formatColor->setFillPattern(FILLPATTERN_SOLID);
    //    formatColor->setPatternForegroundColor(COLOR_PINK);
    //}
    //else
    {
        formatColor->setPatternForegroundColor(COLOR_WHITE);
    }

    m_pSheet->setCellFormat(GetPositionY() - 1, 50, formatColor);
    //m_pSheet->setCellFormat(GetPositionY() - 1, 26, formatColor);

    Format* format1 = m_pBook->addFormat();
    //format1->setBorderTop(BORDERSTYLE_THIN);
    //format1->setBorderLeft(BORDERSTYLE_THIN);
    //format1->setBorderRight(BORDERSTYLE_THIN);
    //format1->setBorderBottom(BORDERSTYLE_THIN);
    //format1->setAlignV(ALIGNV_CENTER);

    //m_pSheet->setRow(GetPositionY()-1, 50);
    //m_pSheet->setCol(FILEPATHNAME,FILEPATHNAME,10);//일부로 고정하였다.
    //m_pSheet->setCol(TOTALIMAGE, TOTALIMAGE, 11);

    //m_pSheet->setCol(FILEPATHNAME, FILEPATHNAME, 20); //일부로 고정하였다.
    //   m_pSheet->setCol(TOTALIMAGE, TOTALIMAGE, 20);
    //m_pSheet->writeStr(GetPositionY(), FILEPATHNAME, szCellName, format1);
    for (long n = 0; n < nNumber; n++)
    {
        //Next_X();
        m_pSheet->writeStr(GetPositionY(), n + 1, vecResultData[n], format1);
    }
    IPVM::Image_8u_C3 ROIImage;
    Next_X();

    SaveJPGImage_Bright(szImageFileName_Bright, 63, 140, ROIrt, FALSE);
    SaveJPGImage_Dark(szImageFileName_Dark, 63, 140, ROIrt, FALSE);
    if (szImageFileName_Bin != _T(""))
    {
        SaveJPGImage_Bin(szImageFileName_Bin, 63, 140, ROIrt, FALSE);
    }

    for (long n = nNumber; n < vecResultData.size(); n++)
    {
        //Next_X();
        m_pSheet->writeStr(GetPositionY(), n + 4, vecResultData[n], format1);
    }

    //SaveJPGImage(szImageFileName_Dark, 80, 140, FALSE);
}
void CExcelWrapClass::AddImageFileData(long nRow, LPCTSTR szCellName, LPCTSTR szImageFileName_Dark, LPCTSTR szImageFileName_Bright, vector<CString> vecResultData, vector<long> vecResultValue)
 {
	//nRow
	
	Step_Reset(DIR_ALL);
	m_pSheet->setCol(GetPositionX(), GetPositionX(), 1);

	Next_Y(nRow+1);

	Next_X(FILEPATHNAME);

	Format* formatColor = m_pBook->addFormat();
	formatColor->setFillPattern(FILLPATTERN_SOLID);

	m_pSheet->setCellFormat(GetPositionY()-1, 50, formatColor);
	m_pSheet->setCellFormat(GetPositionY()-1, 50, formatColor);

	Format* format1 = m_pBook->addFormat();


	m_pSheet->setRow(GetPositionY()-1,26);
	Next_X();

	for (long n = 0; n < vecResultData.size(); n++)
	{
        //Next_X();
        m_pSheet->writeStr(GetPositionY(), n+2, vecResultData[n], format1);
        
	}
    //SaveJPGImage2(szImageFileName_Bright, 80, 140, FALSE);
    SaveJPGImage(szImageFileName_Dark, 80, 140, FALSE);

}

IPVM::Image_8u_C1& CExcelWrapClass::SaveJPGImage(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, BOOL bCrop)
{
	if(bCrop)
	{
		IPVM::Image_8u_C1 BlobJpgImg(m_imgTemp.GetSizeX(), m_imgTemp.GetSizeY());

		memcpy(BlobJpgImg.GetMem2D(), m_imgTemp.GetMem2D(), (m_imgTemp.GetSizeX() * m_imgTemp.GetSizeY()));
		//TempJpgImg = m_imgDst;
		BlobJpgImg.SaveJpeg(szImageFileName);

		AddPicture(szImageFileName, nImageWidth, nImageHeight);
		//ROI 크롭을 한다.
		return m_imgTemp;
	}
    else
    {
        IPVM::Image_8u_C1 TempJpgImg(m_imgSrc.GetSizeX(), m_imgSrc.GetSizeY());

        //memcpy(TempJpgImg.GetMem2D(), m_imgDst.GetMem2D(), (m_imgDst.GetSizeX() * m_imgDst.GetSizeY()));
        IPVM::ImageProcessing::Copy(m_imgSrc, IPVM::Rect(m_imgSrc), IPVM::Rect(TempJpgImg), TempJpgImg);
        //TempJpgImg = m_imgDst;
        TempJpgImg.SaveJpeg(szImageFileName);

        //CItpWrapper::ItpImageDumpBMP(szImageFileName, m_imgDst.pbyImage, m_imgDst.ROI.Width(), m_imgDst.ROI.Height(), m_imgDst.bpp, m_imgDst.ROI, 0);
        AddPicture(szImageFileName, nImageWidth, nImageHeight);

        return m_imgSrc; //실제로는 ::GetProcBuffer(nThread, 0) 주소
    }
}

IPVM::Image_8u_C1& CExcelWrapClass::SaveJPGImage_DefectROI(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, IPVM::Rect ROIrt, IPVM::Rect ROIDefectrt)
{

    IPVM::Image_8u_C1 TempJpgImg_R(m_imgBin.GetSizeX(), m_imgBin.GetSizeY());
    IPVM::Image_8u_C1 TempJpgImg_G(m_imgBin.GetSizeX(), m_imgBin.GetSizeY());
    IPVM::Image_8u_C1 TempJpgImg_B(m_imgBin.GetSizeX(), m_imgBin.GetSizeY());
    IPVM::Image_8u_C3 CombineImg;
    CombineImg.Create(m_imgBin.GetSizeX(), m_imgBin.GetSizeY());
    //memcpy(TempJpgImg.GetMem2D(), m_imgDst.GetMem2D(), (m_imgDst.GetSizeX() * m_imgDst.GetSizeY()));
    TempJpgImg_R.Create(m_imgBin.GetSizeX(), m_imgBin.GetSizeY());
    IPVM::ImageProcessing::Copy(m_imgBin, IPVM::Rect(m_imgBin), TempJpgImg_R);
    TempJpgImg_G.Create(m_imgBin.GetSizeX(), m_imgBin.GetSizeY());
    IPVM::ImageProcessing::Copy(m_imgBin, IPVM::Rect(m_imgBin), TempJpgImg_G);
    TempJpgImg_B.Create(m_imgBin.GetSizeX(), m_imgBin.GetSizeY());
    IPVM::ImageProcessing::Copy(m_imgBin, IPVM::Rect(m_imgBin), TempJpgImg_B);

        IPVM::ImageProcessing::CombineRGB(TempJpgImg_R, TempJpgImg_G, TempJpgImg_B, IPVM::Rect(m_imgBin), CombineImg);
    IPVM::Rect rt;
    IPVM::Point_8u_C3 val2;
    val2.m_x = 0;
    val2.m_y = 0;
    val2.m_z = 255;
    IPVM::Image_8u_C3 CombineImg_Linear(m_imgBin.GetSizeX() / 32, m_imgBin.GetSizeY() / 32);
    // IPVM::ImageProcessing::Resize_Linear(CombineImg, CombineImg_Linear);

    IPVM::Rect rtt(CombineImg);
    //IPVM::Rect ROIDefectrt_Linear(
    //    ROIDefectrt.m_left * 0.025, ROIDefectrt.m_top * 0.1, ROIDefectrt.m_right * 0.025, ROIDefectrt.m_bottom * 0.1);
    //IPVM::ImageProcessing::Resize_Linear(CombineImg, ROIDefect_Linear);
    rt.m_left = ROIDefectrt.m_left - 200;
    rt.m_right = ROIDefectrt.m_right + 200;
    rt.m_top = ROIDefectrt.m_top - 200;
    rt.m_bottom = ROIDefectrt.m_bottom + 200;
    if (rt.m_left < 0)
        rt.m_left = 0;
    if (rt.m_right > m_imgBin.GetSizeX() - 1)
        rt.m_right = m_imgBin.GetSizeX() - 1;
    if (rt.m_top < 0)
        rt.m_top = 0;
    if (rt.m_bottom > m_imgBin.GetSizeY())
        rt.m_bottom = m_imgBin.GetSizeY() - 1;

    IPVM::ImageProcessing::Fill(rt, val2, CombineImg);

    TCHAR programpath[_MAX_PATH];
    GetCurrentDirectory(_MAX_PATH, programpath);
    CString ImageFolderName = _T("\\res\\DataSearchImage_.jpg");
    CString strImagePath = programpath + ImageFolderName;

    //CombineImg_Linear.SaveJpeg(szImageFileName);

    IPVM::ImageProcessing::Resize_Linear(CombineImg, CombineImg_Linear);
    //CombineImg_Linear.SaveJpeg(strImagePath);
    CombineImg_Linear.SaveJpeg(_T("D:\\CombineImg_Linear.jpg"));
    //AddPicture_Origin(strImagePath, ROIrt.Width() * 2.5, ROIrt.Height()*10 , 4);
    //AddPicture_Origin(_T("D:\\CombineImg_Linear.jpg"), ROIrt.Width(), ROIrt.Height(), 4);
    IPVM::Rect rttc(CombineImg_Linear);
    AddPicture(_T("D:\\CombineImg_Linear.jpg"), ROIrt.Width() / 32, ROIrt.Height() / 32, 8, TRUE);

    return m_imgSrc; //실제로는 ::GetProcBuffer(nThread, 0) 주소

}

IPVM::Image_8u_C1& CExcelWrapClass::SaveJPGImage_Bright(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, IPVM::Rect ROIrt, BOOL bCrop)
{
    if (bCrop)
    {
        IPVM::Image_8u_C1 BlobJpgImg(m_imgTemp.GetSizeX(), m_imgTemp.GetSizeY());

        memcpy(BlobJpgImg.GetMem2D(), m_imgTemp.GetMem2D(), (m_imgTemp.GetSizeX() * m_imgTemp.GetSizeY()));
        //TempJpgImg = m_imgDst;
        BlobJpgImg.SaveJpeg(szImageFileName);

        AddPicture(szImageFileName, nImageWidth, nImageHeight);
        //ROI 크롭을 한다.
        //::DeleteFile(szImageFileName);
        return m_imgTemp;
    }
    else
    {
        IPVM::Image_8u_C1 TempJpgImg_R(m_imgBright.GetSizeX(), m_imgBright.GetSizeY());
        IPVM::Image_8u_C1 TempJpgImg_G(m_imgBright.GetSizeX(), m_imgBright.GetSizeY());
        IPVM::Image_8u_C1 TempJpgImg_B(m_imgBright.GetSizeX(), m_imgBright.GetSizeY());
        IPVM::Image_8u_C3 CombineImg;
        CombineImg.Create(m_imgBright.GetSizeX(), m_imgBright.GetSizeY());
        //memcpy(TempJpgImg.GetMem2D(), m_imgDst.GetMem2D(), (m_imgDst.GetSizeX() * m_imgDst.GetSizeY()));
        TempJpgImg_R.Create(m_imgBright.GetSizeX(), m_imgBright.GetSizeY());
        IPVM::ImageProcessing::Copy(m_imgBright, IPVM::Rect(m_imgBright), TempJpgImg_R);
        TempJpgImg_G.Create(m_imgBright.GetSizeX(), m_imgBright.GetSizeY());
        IPVM::ImageProcessing::Copy(m_imgBright, IPVM::Rect(m_imgBright), TempJpgImg_G);
        TempJpgImg_B.Create(m_imgBright.GetSizeX(), m_imgBright.GetSizeY());
        IPVM::ImageProcessing::Copy(m_imgBright, IPVM::Rect(m_imgBright), TempJpgImg_B);

        IPVM::ImageProcessing::CombineRGB(TempJpgImg_R, TempJpgImg_G, TempJpgImg_B, IPVM::Rect(m_imgBright), CombineImg);

        IPVM::Rect rt;
        IPVM::Point_8u_C3 val2;
        val2.m_x = 0;
        val2.m_y = 255;
        val2.m_z = 0;
        for (long nCnt = 0; nCnt < 4; nCnt++)
        {
            switch (nCnt)
            {
                case 0: //top

                    rt.m_left = ROIrt.m_left;
                    rt.m_right = ROIrt.m_right;
                    rt.m_top = ROIrt.m_top;
                    rt.m_bottom = rt.m_top + 2;

                    IPVM::ImageProcessing::Fill(rt, val2, CombineImg);
                    break;
                case 1: // left

                    rt.m_left = ROIrt.m_left;
                    rt.m_right = rt.m_left + 2;
                    rt.m_top = ROIrt.m_top;
                    rt.m_bottom = ROIrt.m_bottom;

                    IPVM::ImageProcessing::Fill(rt, val2, CombineImg);
                    break;
                case 2: // right

                    rt.m_right = ROIrt.m_right;
                    rt.m_left = rt.m_right - 2;
                    rt.m_top = ROIrt.m_top;
                    rt.m_bottom = ROIrt.m_bottom;

                    IPVM::ImageProcessing::Fill(rt, val2, CombineImg);
                    break;
                case 3:

                    rt.m_left = ROIrt.m_left;
                    rt.m_right = ROIrt.m_right;
                    rt.m_bottom = ROIrt.m_bottom;
                    rt.m_top = rt.m_bottom - 2;

                    IPVM::ImageProcessing::Fill(rt, val2, CombineImg);
                    break;
                default:
                    break;
            }
        }
        CombineImg.SaveJpeg(szImageFileName);

        //CItpWrapper::ItpImageDumpBMP(szImageFileName, m_imgDst.pbyImage, m_imgDst.ROI.Width(), m_imgDst.ROI.Height(), m_imgDst.bpp, m_imgDst.ROI, 0);
        AddPicture(szImageFileName, nImageWidth, nImageHeight,1);
        //::DeleteFile(szImageFileName);
        //IPVM::Image_8u_C1 TempJpgImg(m_imgBright.GetSizeX(), m_imgBright.GetSizeY());

        //long nWidth = m_imgBright.GetSizeX();
        ////memcpy(TempJpgImg.GetMem2D(), m_imgDst.GetMem2D(), (m_imgDst.GetSizeX() * m_imgDst.GetSizeY()));
        ////m_imgBin.SaveJpeg(_T("D:\\1234.jpg"));
        //IPVM::ImageProcessing::Copy(m_imgBright, IPVM::Rect(m_imgBright), IPVM::Rect(TempJpgImg), TempJpgImg);
        ////TempJpgImg = m_imgDst;
        //TempJpgImg.SaveJpeg(szImageFileName);

        ////CItpWrapper::ItpImageDumpBMP(szImageFileName, m_imgDst.pbyImage, m_imgDst.ROI.Width(), m_imgDst.ROI.Height(), m_imgDst.bpp, m_imgDst.ROI, 0);
        ////if (nWidth < nImageWidth)
        ////{
        ////    AddPicture(szImageFileName, nWidth, nImageHeight, 1);
        ////}
        ////else
        //{
        //    AddPicture(szImageFileName, nImageWidth, nImageHeight, 1);
        //}
        //::DeleteFile(szImageFileName);
        return m_imgSrc; //실제로는 ::GetProcBuffer(nThread, 0) 주소
    }
}

IPVM::Image_8u_C1& CExcelWrapClass::SaveJPGImage_Dark(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, IPVM::Rect ROIrt, BOOL bCrop)
{
    if (bCrop)
    {
        IPVM::Image_8u_C1 BlobJpgImg(m_imgTemp.GetSizeX(), m_imgTemp.GetSizeY());

        memcpy(BlobJpgImg.GetMem2D(), m_imgTemp.GetMem2D(), (m_imgTemp.GetSizeX() * m_imgTemp.GetSizeY()));
        //TempJpgImg = m_imgDst;
        BlobJpgImg.SaveJpeg(szImageFileName);

        AddPicture(szImageFileName, nImageWidth, nImageHeight);
        //ROI 크롭을 한다.
        //::DeleteFile(szImageFileName);
        return m_imgTemp;
    }
    else
    {
        IPVM::Image_8u_C1 TempJpgImg_R(m_imgDark.GetSizeX(), m_imgDark.GetSizeY());
        IPVM::Image_8u_C1 TempJpgImg_G(m_imgDark.GetSizeX(), m_imgDark.GetSizeY());
        IPVM::Image_8u_C1 TempJpgImg_B(m_imgDark.GetSizeX(), m_imgDark.GetSizeY());
        IPVM::Image_8u_C3 CombineImg;
        CombineImg.Create(m_imgDark.GetSizeX(), m_imgDark.GetSizeY());
        //memcpy(TempJpgImg.GetMem2D(), m_imgDst.GetMem2D(), (m_imgDst.GetSizeX() * m_imgDst.GetSizeY()));
        TempJpgImg_R.Create(m_imgDark.GetSizeX(), m_imgDark.GetSizeY());
        IPVM::ImageProcessing::Copy(m_imgDark, IPVM::Rect(m_imgDark), TempJpgImg_R);
        TempJpgImg_G.Create(m_imgDark.GetSizeX(), m_imgDark.GetSizeY());
        IPVM::ImageProcessing::Copy(m_imgDark, IPVM::Rect(m_imgDark), TempJpgImg_G);
        TempJpgImg_B.Create(m_imgDark.GetSizeX(), m_imgDark.GetSizeY());
        IPVM::ImageProcessing::Copy(m_imgDark, IPVM::Rect(m_imgDark), TempJpgImg_B);

        IPVM::ImageProcessing::CombineRGB(TempJpgImg_R, TempJpgImg_G, TempJpgImg_B, IPVM::Rect(m_imgDark), CombineImg);

         IPVM::Rect rt;
        IPVM::Point_8u_C3 val2;
        val2.m_x = 0;
        val2.m_y = 255;
        val2.m_z = 0;
        for (long nCnt = 0; nCnt < 4; nCnt++)
        {
            switch (nCnt)
            {
                case 0: //top

                    rt.m_left = ROIrt.m_left;
                    rt.m_right = ROIrt.m_right;
                    rt.m_top = ROIrt.m_top;
                    rt.m_bottom = rt.m_top + 2;

                    IPVM::ImageProcessing::Fill(rt, val2, CombineImg);
                    break;
                case 1: // left

                    rt.m_left = ROIrt.m_left;
                    rt.m_right = rt.m_left + 2;
                    rt.m_top = ROIrt.m_top;
                    rt.m_bottom = ROIrt.m_bottom;

                    IPVM::ImageProcessing::Fill(rt, val2, CombineImg);
                    break;
                case 2: // right

                    rt.m_right = ROIrt.m_right;
                    rt.m_left = rt.m_right - 2;
                    rt.m_top = ROIrt.m_top;
                    rt.m_bottom = ROIrt.m_bottom;

                    IPVM::ImageProcessing::Fill(rt, val2, CombineImg);
                    break;
                case 3:

                    rt.m_left = ROIrt.m_left;
                    rt.m_right = ROIrt.m_right;
                    rt.m_bottom = ROIrt.m_bottom;
                    rt.m_top = rt.m_bottom - 2;

                    IPVM::ImageProcessing::Fill(rt, val2, CombineImg);
                    break;
                default:
                    break;
            }
        }
        CombineImg.SaveJpeg(szImageFileName);
        AddPicture(szImageFileName, nImageWidth, nImageHeight,2);
        //::DeleteFile(szImageFileName);
        //IPVM::Image_8u_C1 TempJpgImg(m_imgDark.GetSizeX(), m_imgDark.GetSizeY());

        //long nWidth = m_imgDark.GetSizeX();
        ////memcpy(TempJpgImg.GetMem2D(), m_imgDst.GetMem2D(), (m_imgDst.GetSizeX() * m_imgDst.GetSizeY()));
        ////m_imgBin.SaveJpeg(_T("D:\\1234.jpg"));
        //IPVM::ImageProcessing::Copy(m_imgDark, IPVM::Rect(m_imgDark), IPVM::Rect(TempJpgImg), TempJpgImg);
        ////TempJpgImg = m_imgDst;
        //TempJpgImg.SaveJpeg(szImageFileName);

        ////CItpWrapper::ItpImageDumpBMP(szImageFileName, m_imgDst.pbyImage, m_imgDst.ROI.Width(), m_imgDst.ROI.Height(), m_imgDst.bpp, m_imgDst.ROI, 0);
        ////if (nWidth < nImageWidth)
        ////{
        ////    AddPicture(szImageFileName, nWidth, nImageHeight, 1);
        ////}
        ////else
        //{
        //    AddPicture(szImageFileName, nImageWidth, nImageHeight,2);
        //}
        //::DeleteFile(szImageFileName);
        return m_imgDark; //실제로는 ::GetProcBuffer(nThread, 0) 주소
    }
}

IPVM::Image_8u_C1& CExcelWrapClass::SaveJPGImage_Bin(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, IPVM::Rect ROIrt, BOOL bCrop)
{
    if (bCrop)
    {
        IPVM::Image_8u_C1 BlobJpgImg(m_imgTemp.GetSizeX(), m_imgTemp.GetSizeY());

        memcpy(BlobJpgImg.GetMem2D(), m_imgTemp.GetMem2D(), (m_imgTemp.GetSizeX() * m_imgTemp.GetSizeY()));
        //TempJpgImg = m_imgDst;
        BlobJpgImg.SaveJpeg(szImageFileName);

        AddPicture(szImageFileName, nImageWidth, nImageHeight,1);
        //ROI 크롭을 한다.
        ::DeleteFile(szImageFileName);
        return m_imgTemp;
    }
    else
    {
        IPVM::Image_8u_C1 TempJpgImg(m_imgBin.GetSizeX(), m_imgBin.GetSizeY());

		long nWidth = m_imgBin.GetSizeX();
        //memcpy(TempJpgImg.GetMem2D(), m_imgDst.GetMem2D(), (m_imgDst.GetSizeX() * m_imgDst.GetSizeY()));
        //m_imgBin.SaveJpeg(_T("D:\\1234.jpg"));
        IPVM::ImageProcessing::Copy(m_imgBin, IPVM::Rect(m_imgBin), IPVM::Rect(TempJpgImg), TempJpgImg);
        //TempJpgImg = m_imgDst;
        TempJpgImg.SaveJpeg(szImageFileName);

        //CItpWrapper::ItpImageDumpBMP(szImageFileName, m_imgDst.pbyImage, m_imgDst.ROI.Width(), m_imgDst.ROI.Height(), m_imgDst.bpp, m_imgDst.ROI, 0);
  //      if (nWidth < nImageWidth)
		//{
  //          AddPicture(szImageFileName, nWidth, nImageHeight,1);
		//}
  //      else
        {
            AddPicture(szImageFileName, nImageWidth, nImageHeight,3);
		}
        ::DeleteFile(szImageFileName);

        return m_imgBin; //실제로는 ::GetProcBuffer(nThread, 0) 주소
    }
}
void CExcelWrapClass::RejectListName(const vector<CString> &strRejectName)
{
	m_nRejectList = (long)strRejectName.size();

	Format* format1 = m_pBook->addFormat();
	format1->setBorderTop(BORDERSTYLE_THIN);
    long n = ALIGNV_CENTER;
	format1->setBorderLeft(BORDERSTYLE_THIN);
	format1->setBorderRight(BORDERSTYLE_THIN);
	format1->setBorderBottom(BORDERSTYLE_THIN);
	format1->setAlignV(ALIGNV_CENTER);

	//m_pSheet->writeStr(REJECTNAMETITLE, FILEPATHNAME, _T("Cell ID"), format1);

	//m_pSheet->writeStr(REJECTNAMETITLE, TOTALIMAGE, _T("Align Image"), format1);

	long nStartPosition = FILEPATHNAME;
	for(long i = 0 ; i <m_nRejectList;i++)
	{
		if( !m_pSheet->writeStr(REJECTNAMETITLE, nStartPosition++, strRejectName[i],format1) )
		{
			int k = 0;
		}
	}
}

void CExcelWrapClass::AddPicture(LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, long nCol, BOOL bCrop)
{
	Format* format1 = m_pBook->addFormat();
	format1->setBorderTop(BORDERSTYLE_THIN);
	format1->setBorderLeft(BORDERSTYLE_THIN);
	format1->setBorderRight(BORDERSTYLE_THIN);
	format1->setBorderBottom(BORDERSTYLE_THIN);

	//m_pSheet->setCol(GetPositionY(), GetPositionX(), 50);
	m_pSheet->setRow(GetPositionY(), 50);

	long nImageID = m_pBook->addPicture(szImageFileName);
	Sleep(1);
    //m_pSheet->firstCol();
	//m_pSheet->setPicture2(GetPositionY(), GetPositionX() + nCol, nImageID, nImageWidth, nImageHeight); // 요놈이 메인 이미지
    //m_pSheet->setPicture(GetPositionY(), GetPositionX() + nCol, nImageID, 1.2); // 요놈이 메인 이미지

    // m_pSheet->setPicture(GetPositionY(), GetPositionX() + nCol, nImageID, 1); 
    if(bCrop == TRUE)
    {
        //m_pSheet->setPicture2(
        //    GetPositionY(), GetPositionX() + nCol, nImageID, nImageWidth, nImageHeight); // 요놈이 메인 이미지
        m_pSheet->setPicture2(
            GetPositionY(), /*GetPositionX() + nCol*/4, nImageID,  190, 60); // 요놈이 메인 이미지
    }
    else
    {
        m_pSheet->setPicture2(GetPositionY(), GetPositionX() + nCol, nImageID, 60, 60); // 요놈이 메인 이미지
    }
    
	//m_pSheet->setCellFormat(GetPositionY(), FILEPATHNAME, format1);
	//m_pSheet->setCellFormat(GetPositionY(), TOTALIMAGE, format1);
	//m_pSheet->setCellFormat(GetPositionY(), GetPositionX(), format1);

	Sleep(1);
}

void CExcelWrapClass::Image_RectDraw(IPVM::Image_8u_C1& image, const vector<CRect>& vecrectROI, long nThickness)
{
 //   long nX;
 //   long nY;
	//CPoint cpPostion;

	//for(long x = 0; x < vecrectROI.size() ; x++)
	//{
	//	CRect rectROI = vecrectROI[x];
	//	for(nX = rectROI.left; nX<rectROI.right; nX++)
	//	{			
	//		cpPostion.x=min(image.ROI.Width(), max(nX, 0));
	//		cpPostion.y=min(image.ROI.Height(), max(0,rectROI.top));
	//		Image_MultiLineDraw(image, cpPostion, UP, nThickness);

	//		cpPostion.x=min(image.ROI.Width(), max(nX, 0));
	//		cpPostion.y=min(image.ROI.Height(), max(0,rectROI.bottom - 1));
	//		Image_MultiLineDraw(image, cpPostion, DOWN, nThickness);
	//	}

	//	for(nY = rectROI.top; nY<rectROI.bottom; nY++)
	//	{	
	//		cpPostion.x=min(image.ROI.Width(), max(0, rectROI.left));
	//		cpPostion.y=min(image.ROI.Height(), max(0,nY));
	//		Image_MultiLineDraw(image,cpPostion, LEFT, nThickness);

	//		cpPostion.x=min(image.ROI.Width(), max(0, rectROI.right - 1));
	//		cpPostion.y=min(image.ROI.Height(), max(0,nY));
	//		Image_MultiLineDraw(image,cpPostion, RIGHT, nThickness);
	//	}
	//}
}

void CExcelWrapClass::Image_MultiLineDraw(IPVM::Image_8u_C1& image, const CPoint& ImagePos, long nDir, long nThickness)
{
	//long nThicknessCnt=0;
	//CPoint CurrentPos;

	//switch(nDir)
	//{
	//case LEFT://X--
	//	for(nThicknessCnt=0; nThicknessCnt<nThickness ;nThicknessCnt++)
	//	{
	//		CurrentPos=ImagePos;
	//		CurrentPos.x = CurrentPos.x -nThicknessCnt;
	//		Image_LineDraw(image,CurrentPos,HORIZONTAL);
	//	}
	//	break;
	//case UP://Y--
	//	for(nThicknessCnt=0; nThicknessCnt<nThickness ;nThicknessCnt++)
	//	{
	//		CurrentPos=ImagePos;
	//		CurrentPos.y = CurrentPos.y-nThicknessCnt;
	//		Image_LineDraw(image,CurrentPos,HORIZONTAL);
	//	}
	//	break;
	//case RIGHT://X++
	//	for(nThicknessCnt=0; nThicknessCnt<nThickness ;nThicknessCnt++)
	//	{
	//		CurrentPos=ImagePos;
	//		CurrentPos.x = CurrentPos.x +nThicknessCnt;
	//		Image_LineDraw(image,CurrentPos,HORIZONTAL);
	//	}
	//	break;
	//case DOWN://Y++
	//	for(nThicknessCnt=0; nThicknessCnt<nThickness ;nThicknessCnt++)
	//	{
	//		CurrentPos=ImagePos;
	//		CurrentPos.y = CurrentPos.y +nThicknessCnt;
	//		Image_LineDraw(image,CurrentPos,HORIZONTAL);
	//	}
	//	break;//X
	//}		
}

void CExcelWrapClass::Image_LineDraw(IPVM::Image_8u_C1& Image, const CPoint& ImagePos, long nDir)
{ 
	//switch(nDir)
	//{
	//case HORIZONTAL://X
	//	Image.pbyImage[(ImagePos.y)*Image.ROI.Width()*3+ImagePos.x*3+0]   =0;
	//	Image.pbyImage[(ImagePos.y)*Image.ROI.Width()*3+ImagePos.x*3+1]   =0;
	//	Image.pbyImage[(ImagePos.y)*Image.ROI.Width()*3+ImagePos.x*3+2]   =255;
	//	break;
	//case VERTICAL://Y
	//	Image.pbyImage[(ImagePos.y)*Image.ROI.Width()*3+ImagePos.x*3+0]   =0;
	//	Image.pbyImage[(ImagePos.y)*Image.ROI.Width()*3+ImagePos.x*3+1]   =0;
	//	Image.pbyImage[(ImagePos.y)*Image.ROI.Width()*3+ImagePos.x*3+2]   =255;
	//	break;//X
	//}		
}

IPVM::Image_8u_C1& CExcelWrapClass::SaveJPGImage_Bright_NoSuburb(
    LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, IPVM::Rect ROIrt, BOOL bCrop, BOOL bTotal)
{
    if (bCrop)
    {
        IPVM::Image_8u_C1 BlobJpgImg(m_imgTemp.GetSizeX(), m_imgTemp.GetSizeY());

        memcpy(BlobJpgImg.GetMem2D(), m_imgTemp.GetMem2D(), (m_imgTemp.GetSizeX() * m_imgTemp.GetSizeY()));
        //TempJpgImg = m_imgDst;
        //BlobJpgImg.SaveJpeg(szImageFileName);

        AddPicture(szImageFileName, nImageWidth, nImageHeight);
        return m_imgTemp;
    }
    else
    {
        if (bTotal == TRUE)
        {
            AddPicture(szImageFileName, nImageWidth, nImageHeight, 0);
        }
        else
        {
            AddPicture(szImageFileName, nImageWidth, nImageHeight, 1);
        }
        

        return m_imgSrc; //실제로는 ::GetProcBuffer(nThread, 0) 주소
    }
}

IPVM::Image_8u_C1& CExcelWrapClass::SaveJPGImage_Dark_NoSuburb(
    LPCTSTR szImageFileName, long nImageWidth, long nImageHeight, IPVM::Rect ROIrt, BOOL bCrop, BOOL bTotal)
{
    if (bCrop)
    {
        IPVM::Image_8u_C1 BlobJpgImg(m_imgTemp.GetSizeX(), m_imgTemp.GetSizeY());

        memcpy(BlobJpgImg.GetMem2D(), m_imgTemp.GetMem2D(), (m_imgTemp.GetSizeX() * m_imgTemp.GetSizeY()));
        //TempJpgImg = m_imgDst;
        BlobJpgImg.SaveJpeg(szImageFileName);

        AddPicture(szImageFileName, nImageWidth, nImageHeight);
        return m_imgTemp;
    }
    else
    {
        if (bTotal == TRUE)
        {
            AddPicture(szImageFileName, nImageWidth, nImageHeight, 1);
        }
        else
        {
            AddPicture(szImageFileName, nImageWidth, nImageHeight, 2);
        }
        return m_imgDark; //실제로는 ::GetProcBuffer(nThread, 0) 주소
    }
} 

void CExcelWrapClass::AddTextDatas(long nRow, LPCTSTR szCellName, vector<CString> vecResultData, CString strDefectResult, long nIndex)
{
    Step_Reset(DIR_ALL);

    Next_Y(nRow + 1);

    Next_X(FILEPATHNAME);
    long nNumber = 2;
    Format* formatColor = m_pBook->addFormat();

    {

        formatColor->setPatternForegroundColor(COLOR_WHITE);
    }

    m_pSheet->setCellFormat(GetPositionY() - 1, 50, formatColor);

    Format* format1 = m_pBook->addFormat();

    for (long n = 0; n < nNumber; n++)
    {
        //Next_X();
        m_pSheet->writeStr(GetPositionY(), n + 1, vecResultData[n], format1);
    }
    Next_X();
    CString strNum;
    //static long nIndex = 0;
    for (long n = 0; n < vecResultData.size(); n++)
    {

        m_pSheet->writeStr(GetPositionY(), n+2 , vecResultData[n], format1);
    }
}