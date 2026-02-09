// DlgInspectionImageView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "InspectionRoot.h"
#include "DlgInspectionMain.h"
#include "afxdialogex.h"
#include "ImageViewEx.h"

#include "Types/Rect.h"
#include "Widget/ProfileView.h"

#include <Base/basedef.h>
#include "Algorithm/EdgeDetection.h"
#include "Algorithm/EdgeDetectionPara.h"
#include "Algorithm/EdgeDetectionDebugInfo.h"
#include "Algorithm/EdgeDetectionFilter.h"
#include "Algorithm/Geometry.h"
#include "Algorithm/ImageProcessing.h"
#include "Algorithm/Mathematics.h"
#include "Gadget/TimeCheck.h"

#include "CDlgClassificationST.h"
#include "Accessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgInspectionMain 대화 상자입니다.

void CDlgInspectionMain::CallbackOnROIChanged(const long id, void *userData, const wchar_t * szKey)
{
	((CDlgInspectionMain *)userData)->CallbackOnROIChanged(szKey);
}

void CDlgInspectionMain::CallbackOnROIChanged(const wchar_t * szKey)
{
	CString strKey = szKey;
	IPVM::Rect rect;
	IPVM::Rect rectOld;

	//CInspectionCellAlign* inspection = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	//
	//CInspectionBlob* blob = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	CInspectionCellAlign* inspection = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);
	CInspectionBlob* blob = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::Blob);
    CInspectionMeasure* measure
        = (CInspectionMeasure*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::measure);
    CInspectionChattering* chattering = (CInspectionChattering*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::Chattering);
	auto* abnormal = (CInspectionAbnormal*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::abnormalInspection);

	long blobCondtionSize = blob->m_para.m_vecBlobParaCondition.size();

	m_imageView->ROIGet(szKey, rect);
	
	if (strKey.Find(_T("Measure")) != -1)
	{
		CString number;
		number.Format(_T("%s"), strKey.Right(2));
		long num = _ttol(number);
		inspection->m_para.m_vecrtDetectLineMeasure[num].SetRect(rect.TopLeft(), rect.BottomRight());
	}
	else if (strKey.Find(_T("Blob")) != -1)
	{
		CString number;
		number.Format(_T("%s"), strKey.Right(2));
		long num = _ttol(number);
		inspection->m_para.m_vecrtDetectLineBlob[num].SetRect(rect.TopLeft(), rect.BottomRight());
	}
	else if (strKey.Find(_T("MSA")) != -1)
	{
		CString number;
		number.Format(_T("%s"), strKey.Right(2));
		long num = _ttol(number);
        measure->m_para.m_vecrtDetectLineMSA[num].SetRect(rect.TopLeft(), rect.BottomRight());
	}
    else if (strKey.Find(_T("Chattering ROI")) != -1)
    {
        chattering->m_para.m_rtSetInspArea.SetRect(rect.TopLeft(), rect.BottomRight());
    }
    else if (strKey.Find(_T("Abnormal ROI 1")) != -1)
    {
        abnormal->m_para.m_rtSetInspArea.SetRect(rect.TopLeft(), rect.BottomRight());
	}
    else if (strKey.Find(_T("Abnormal ROI 2")) != -1)
    {
        abnormal->m_para.m_rtSetInspArea_2.SetRect(rect.TopLeft(), rect.BottomRight());
    }
	else if (_T("Defect") == strKey.Left(6))
	{
		CString strNum;
		int nNum = strKey.GetLength() - 7;
		strNum = strKey.Mid(7, nNum);
		int nNumber = _wtoi(strNum);
		ShowRejectROI_Event();
		auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
		CInspectionCell* parent = m_inspectionRoot->GetInspectionCell(m_curIsUp);

		auto* inspection = (CInspectionBlob*)parent->GetProcessingItem(ProcItemCategory::Blob);
		long nAreaNum = inspection->m_para.m_vecBlobParaCondition.size();
		CDlgClassificationST dlg(isi_spec, this, &parent->m_ISICellAllDefect, nAreaNum,nNumber );


		if (dlg.DoModal() == IDOK)
		{
			isi_spec = dlg.m_ISI_Spec_table;
		}
	}
	else if (strKey == _T("EDGE_ROI"))
	{
		IPVM::EdgeDetection edge;

		IPVM::Rect rt1;
		IPVM::EdgeDetectionPara edgePara;
		IPVM::EdgeDetectionDebugInfo EdgeInfo;
		IPVM::EdgeDetectionFilter edgeFilter;
		IPVM::Point_32f_C3 edgePoint;
		CInspectionCell* cell = (CInspectionCell*)m_inspectionRoot->GetInspectionCell(m_curIsUp);

		m_imageView->ImageOverlayClear();
		m_imageView->ROIGet(_T("EDGE_ROI"), rt1);

		edgePara.m_findFirstEdge = m_FindFirst;
		edgePara.m_edgeType = (IPVM::EdgeType)m_DetectType;
		edgePara.m_searchDirection = (IPVM::SearchDirection)m_Direction;
		edgePara.m_edgeThreshold = m_EdgeTh;

		edgeFilter.SetParameter(m_FilterLength, m_Gaus);

		if (TRUE == m_isUseEdgeDetect)
		{
			long nFoundEdgeCount = 0;
			std::vector<IPVM::Point_32f_C3> vecEdgePoints;
			std::vector<IPVM::EdgeDetectionDebugInfo> vecEdgeInfos;
			vecEdgePoints.resize(m_nFindEdge);
			vecEdgeInfos.resize(m_nFindEdge);

			if (IPVM::Status::success == edge.DetectEdges(*m_curSelImage, edgePara, edgeFilter, rt1, m_nFindEdge, m_nSearchWidth, &vecEdgePoints[0], nFoundEdgeCount, &vecEdgeInfos[0]))
			{
				BYTE Red = 0, Blue = 0, Green = 0;
				m_profileView->Clear();
				for (int i = 0; i < nFoundEdgeCount; i++)
				{
					Red = 120 * i;
					Blue = 71 * i;
					Green = 17 * i;

					m_imageView->ImageOverlayAdd(IPVM::Point_32f_C2(vecEdgePoints[i].m_x, vecEdgePoints[i].m_y), RGB(Red, Blue, Green), 3);

					std::vector<IPVM::Point_32f_C2> vecProfiles;

					auto CorrelationData = vecEdgeInfos[i].GetCorrelationData();
					for (int j = 0; j < vecEdgeInfos[i].GetCorrelationDataCount(); j++)
					{
						auto value = *(CorrelationData + j);
						vecProfiles.emplace_back((float)j, (float)value);
					}
					m_profileView->SetData(i, &vecProfiles[0], (long)vecEdgeInfos[i].GetCorrelationDataCount(), NOISE_VALUE_32F, RGB(Red, Blue, Green), RGB(Red, Blue, Green));

				}
			}
			else
			{
				IPVM::Point_32f_C2 point(rt1.m_left, rt1.m_top);

				m_imageView->ImageOverlayAdd(point, _T("Can not find Edge"), RGB(255, 0, 0), 100);

			}
			m_imageView->ImageOverlayShow();
		}
		else
		{
			if (IPVM::Status::success == edge.DetectEdge(m_setupImage.m_imageBright[0], edgePara, edgeFilter, rt1, edgePoint, &EdgeInfo))
			{
				IPVM::Point_32f_C2 edge_C2(edgePoint.m_x, edgePoint.m_y);
				auto ProfileData = EdgeInfo.GetLineProfileData();
				std::vector<IPVM::Point_32f_C2> vecProfiles_1;
				std::vector<IPVM::Point_32f_C2> vecProfiles_2;
				for (int i = 0; i < EdgeInfo.GetLineProfileDataCount(); i++)
				{
					auto value = *(ProfileData + i);

					vecProfiles_1.emplace_back((float)i, (float)value);

				}

				auto CorrelationData = EdgeInfo.GetCorrelationData();
				for (int i = 0; i < EdgeInfo.GetCorrelationDataCount(); i++)
				{
					auto value = *(CorrelationData + i);

					vecProfiles_2.emplace_back((float)i, (float)value);
				}

				m_profileView->Clear();
				switch (m_ShowProfile)
				{
				case 0:
					m_profileView->SetData(1, &vecProfiles_2[0], (long)EdgeInfo.GetCorrelationDataCount(), NOISE_VALUE_32F, RGB(0, 255, 0), RGB(255, 0, 0));
					break;

				case 1:
					m_profileView->SetData(0, &vecProfiles_1[0], (long)EdgeInfo.GetLineProfileDataCount(), NOISE_VALUE_32F, RGB(255, 0, 0), RGB(255, 0, 0));
					break;

				default:
					break;
				}
				m_imageView->ImageOverlayAdd(edge_C2, RGB(255, 0, 0), 3);

			}

			else
			{
				IPVM::Point_32f_C2 point(rt1.m_left, rt1.m_top);

				m_imageView->ImageOverlayAdd(point, _T("Can not find Edge"), RGB(255, 0, 0), 100);

			}
			m_imageView->ImageOverlayShow();
		}
	}
	else
	{
		AfxMessageBox(_T("Oops"));
	}
}

void CDlgInspectionMain::CallbackOnMeasurementChanged(const long id, void *userData, const IPVM::LineSeg_32f &line)
{
	((CDlgInspectionMain *)userData)->CallbackOnMeasurementChanged(line);
}

void CDlgInspectionMain::CallbackOnMeasurementEnd(const long id, void *userData, const IPVM::LineSeg_32f &line)
{
	((CDlgInspectionMain *)userData)->CallbackOnMeasurementEnd();
}

void CDlgInspectionMain::CallbackOnMeasurementChanged(const IPVM::LineSeg_32f &line)
{
	std::vector<BYTE> profiles;
	m_imageView->GetProfile(line, profiles);

	std::vector<IPVM::Point_32f_C2> profileDatas;
	for (long index = 0; index < (long)profiles.size(); index++)
	{
		auto& value = profiles[index];
		profileDatas.emplace_back((float)index, (float)value);
	}

	m_profileView->Clear();
	if (profileDatas.size() > 0)
	{
		m_profileView->SetData(0, &profileDatas[0], (long)profileDatas.size(), NOISE_VALUE_32F, RGB(255, 0, 0), RGB(255, 0, 0));
	}
}

void CDlgInspectionMain::CallbackOnMeasurementEnd()
{
	m_profileView->Clear();
}

afx_msg LRESULT CDlgInspectionMain::ShowProfileView(WPARAM wParam, LPARAM lParam)
{
	auto EdgeDetectDebugInfo = (IPVM::EdgeDetectionDebugInfo*)lParam;
	auto ProfileData = EdgeDetectDebugInfo->GetLineProfileData();
	std::vector<IPVM::Point_32f_C2> vecProfiles_1;
	std::vector<IPVM::Point_32f_C2> vecProfiles_2;
	for (int i = 0; i < EdgeDetectDebugInfo->GetLineProfileDataCount(); i++)
	{
		auto value = *(ProfileData + i);

		vecProfiles_1.emplace_back((float)i, (float)value);

	}

	auto CorrelationData = EdgeDetectDebugInfo->GetCorrelationData();
	for (int i = 0; i < EdgeDetectDebugInfo->GetCorrelationDataCount(); i++)
	{
		auto value = *(CorrelationData + i);

		vecProfiles_2.emplace_back((float)i, (float)value);
	}
	COLORREF color;
	if ((int)wParam == 0)
		color = RGB(0, 255, 0);
	else if ((int)wParam == 1)
		color = RGB(0, 255, 255);
	m_profileView->SetData((int)wParam, &vecProfiles_2[0], (long)EdgeDetectDebugInfo->GetCorrelationDataCount(), NOISE_VALUE_32F, color, color);//edge th
	return 0;
}



#ifdef _DRAW_DEFECT_
void CDlgInspectionMain::callbackMouseClickedDown(const long id, void* userData, const IPVM::Point_32f_C2& pt)
{
	((CDlgInspectionMain*)userData)->callbackMouseClickedDown(id, pt);
}

void CDlgInspectionMain::callbackMouseClickedDown(const long /*id*/, const IPVM::Point_32f_C2& pt)
{
	bDraw = true;
	if (true == bDraw && true == m_isUseDrawDefect)
		DrawDefect(pt);
	else
		return;
}
void CDlgInspectionMain::callbackMouseMoved(const long id, void* userData, const IPVM::Point_32f_C2& pt)
{
	((CDlgInspectionMain*)userData)->callbackMouseMoved(id, pt);
}

void CDlgInspectionMain::callbackMouseMoved(const long /*id*/, const IPVM::Point_32f_C2& pt)
{
	if (true == bDraw && true == m_isUseDrawDefect)
		DrawDefect(pt);
	else
		return;
}
void CDlgInspectionMain::callbackMouseClickedUp(const long id, void* userData, const IPVM::Point_32f_C2& pt)
{
	((CDlgInspectionMain*)userData)->callbackMouseClickedUp(id, pt);
}

void CDlgInspectionMain::callbackMouseClickedUp(const long /*id*/, const IPVM::Point_32f_C2& pt)
{
	bDraw = false;
}
void CDlgInspectionMain::DrawDefect(const IPVM::Point_32f_C2& pt)
{
	IPVM::Rect rtimage(m_inspectionImageBright);
	long Pos_X = pt.m_x;
	long Pos_Y = pt.m_y;

	if (true == rtimage.PtInRect(pt))
	{
		if((int)DefectType::END != m_Type)
		{
			IPVM::Image_8u_C1 cropimage;

			TCHAR szTemp[300];
			::GetModuleFileName(NULL, szTemp, 300);
			CString path = szTemp;
			auto a = path.ReverseFind('\\');
			path = path.Left(path.ReverseFind('\\'));
			path = path.Left(path.ReverseFind('\\'));
			path = path.Left(path.ReverseFind('\\'));
			path.Append(_T("\\CropDefectImage\\"));

			CString strPath(path);
			CString Name;
			switch ((DefectType)m_Type)
			{
#if INSPECTION_MACHINE_UC1
    case DefectType::White_Pinhole:
        Name = _T("PinholeCrater.bmp");
        break;
    case DefectType::White_Line:
        Name = _T("PinholeLine.bmp");
        break;
    case DefectType::Crater:
        Name = _T("Stain_Pinhole.bmp");
        break;
#else
    case DefectType::Pinhole:
        Name = _T("PinholeCrater.bmp");
        break;
    case DefectType::Line:
        Name = _T("PinholeLine.bmp");
        break;
    case DefectType::Stain_Pinhole:
        Name = _T("Stain_Pinhole.bmp");
        break;
#endif // INSPECTION_MACHINE_UC1

			
			case DefectType::Crack:
				Name = _T("Crack.bmp");
				break;
			case DefectType::Seam:
				Name = _T("Seam.bmp");
				break;
			case DefectType::ConnectionTape:
				Name = _T("ConnectionTape.bmp");
				break;
			case DefectType::DarkConnectionTape:
				Name = _T("WhiteTape.bmp");
				break;
			
			default:
				break;
			}
			strPath += Name;

			auto res = cropimage.LoadFrom(strPath);
			if (res != IPVM::Status::success)
			{
				MessageBox(_T("Load Failed"));
			}
			IPVM::Rect rt(cropimage);
			rt &= IPVM::Rect(m_inspectionImageBright);

			for (int y = 0; y < rt.Height(); y++)
			{
				for (int x = 0; x < rt.Width(); x++)
				{
					IPVM::Image_8u_C1* ptimage;

					if (0 == y % 2)
						ptimage = &m_inspectionImageBright;
					else
						ptimage = &m_inspectionImageDark;

					if (Pos_X + x >= m_inspectionImageDark.GetSizeX() || Pos_Y + y >= m_inspectionImageDark.GetSizeY())
						break;
					if (255 != cropimage.GetValue(x, y))
						*ptimage->GetMem(Pos_X + x, Pos_Y + (y/2)) = cropimage.GetValue(x, y);
				}
			}
		}
		else
		{

			CString strB = CXTPPropertyGridItemColor::RGBToString(m_BrightColor);
			CString strD = CXTPPropertyGridItemColor::RGBToString(m_DarkColor);

			CString BColor, DColor;
			AfxExtractSubString(BColor, strB, 0, ';');
			AfxExtractSubString(DColor, strD, 0, ';');

			BYTE Bvalue = _wtoi(BColor);
			BYTE Dvalue = _wtoi(DColor);


			if (m_Size > 1)
			{
				IPVM::Rect Rect(Pos_X, Pos_Y, Pos_X, Pos_Y);
				Rect.InflateRect(m_Size, m_Size, m_Size, m_Size);
				Rect &= rtimage;

				IPVM::ImageProcessing::Fill(Rect, Bvalue, m_inspectionImageBright);
				IPVM::ImageProcessing::Fill(Rect, Dvalue, m_inspectionImageDark);
			}
			else if (m_Size == 1)
			{
				auto& BrightValue = m_inspectionImageBright.GetMem()[Pos_Y * m_inspectionImageBright.GetWidthBytes() + Pos_X];
				BrightValue = Bvalue;
				auto& DarkValue = m_inspectionImageDark.GetMem()[Pos_Y * m_inspectionImageDark.GetWidthBytes() + Pos_X];
				DarkValue = Dvalue;
			}
		}
		if (true == bisBrightImage)
			m_imageView->SetImage(m_inspectionImageBright);
		else
			m_imageView->SetImage(m_inspectionImageDark);

	}
	else
		return;
}
#endif