// DlgCalculationResolution.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgCalculationResolution.h"
#include "afxdialogex.h"
#include "resource.h"
#include "CalMap_Distortion_Impl.h"
#include "CalMap_Distortion.h"

#include <ipvmbasedef.h>
#include <Base/basedef.h>
#include "Algorithm/EdgeDetection.h"
#include "Algorithm/EdgeDetectionPara.h"
#include "Algorithm/EdgeDetectionDebugInfo.h"
#include "Algorithm/EdgeDetectionFilter.h"
#include "Types/Point_32f_C3.h"
#include "Types/Point_32f_C2.h"
#include "Types/LineEq_32f.h"
#include "Types/LineSeg_32f.h"
#include "Algorithm/DataFitting.h"
#include "Algorithm/Geometry.h"
// CDlgCalculationResolution 대화 상자

IMPLEMENT_DYNAMIC(CDlgCalculationResolution, CDialogEx)

CDlgCalculationResolution::CDlgCalculationResolution(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CALC_RESOLUTION, pParent)
	, m_pImageView(nullptr)
    , m_nLineThreshold(1)
{
	//m_gridCalcResol = new CXTPPropertyGrid;
}

CDlgCalculationResolution::~CDlgCalculationResolution()
{
	if (m_pImageView != NULL)
	{
		delete m_pImageView;
		m_pImageView = NULL;
	}
	//delete m_gridCalcResol;
}

void CDlgCalculationResolution::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_IMG_VIEW_RESOL, m_imgResol);
    DDX_Control(pDX, IDC_EDIT_LINE_TH, m_editLineThreshold);
}


BEGIN_MESSAGE_MAP(CDlgCalculationResolution, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_IMG_OPEN, &CDlgCalculationResolution::OnBnClickedButtonImgOpen)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_LINE, &CDlgCalculationResolution::OnBnClickedButtonShowLine)
    ON_BN_CLICKED(IDC_BUTTON_SET_LINE_TH, &CDlgCalculationResolution::OnBnClickedButtonSetLineTh)
    ON_BN_CLICKED(IDC_BTN_CALCRES, &CDlgCalculationResolution::OnBnClickedBtnCalcres)
    ON_BN_CLICKED(IDC_BTN_SAVEDISTOR, &CDlgCalculationResolution::OnBnClickedBtnSavedistor)
    END_MESSAGE_MAP()


// CDlgCalculationResolution 메시지 처리기


BOOL CDlgCalculationResolution::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rect;
	GetClientRect(rect);
	if (nullptr == m_pImageView)
	{
		//IPVM::Rect rect1(m_MapSize.left, m_MapSize.top, m_MapSize.right, m_MapSize.bottom);
		
		m_pImageView = new IPVM::ImageView(this->GetSafeHwnd(),  IDC_IMG_VIEW_RESOL);
	}
    SetDlgItemInt(IDC_EDIT_LINE_TH, m_nLineThreshold);
	//::MoveWindow(m_pImageView->GetSafeHwnd(), m_MapSize.left, m_MapSize.top, m_MapSize.right, m_MapSize.bottom, TRUE);

	SetDlgItemInt(IDC_EDIT_TH, 1);
    SetDlgItemInt(IDC_EDIT_THDEV, 1);
    SetDlgItemInt(IDC_EDIT_ROWCNT, 2);
    SetDlgItemInt(IDC_EDIT_COLCNT, 2);
    SetDlgItemInt(IDC_EDIT_EDGEMODE, 0);



	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgCalculationResolution::OnBnClickedButtonImgOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	const TCHAR szFilters[] =
		_T("Image files (*.bmp, *.png, *.jpg) | *.bmp;*.png;*.jpg|");

	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, szFilters);

	CString strFileList;
	const int nMaxFiles = 400;
	const int nBuffSize = (nMaxFiles * (MAX_PATH + 1)) + 1;
	dlg.GetOFN().lpstrFile = strFileList.GetBuffer(nBuffSize);
	dlg.GetOFN().nMaxFile = nBuffSize;

	//if (dlg.DoModal() != IDOK) return;

	if (dlg.DoModal() == IDOK)
	{
		m_pImageView->ImageOverlayClear();
		m_image.LoadFrom(dlg.GetPathName());
		m_pImageView->SetImage(m_image);
		m_pImageView->ROISet(_T("RES"), _T("RES"), IPVM::Rect(9744, 338, 10090, 432), RGB(0, 255, 0), 20);
		m_pImageView->ROISet(_T("ALL"), _T("ALL"), IPVM::Rect(9539, 109, 20307, 593), RGB(0, 0, 255), 20);
		m_pImageView->ROIShow();
		
	}
	else
		return;
}

bool compare(IPVM::Point_32f_C2 a, IPVM::Point_32f_C2 b)
{ return a.m_x < b.m_x; }

void CDlgCalculationResolution::OnBnClickedButtonShowLine()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_pImageView->ROISet(_T("RES"), _T("RES"), IPVM::Rect(0, 0, 500, 500), RGB(0, 255, 0), 20);
    m_pImageView->ImageOverlayClear();

	long nSize = 0;

	IPVM::EdgeDetection edge;
	IPVM::EdgeDetectionPara edgePara;
	IPVM::EdgeDetectionDebugInfo EdgeInfo;
	IPVM::EdgeDetectionFilter edgeFilter;
	IPVM::Point_32f_C3 edgePoint;

	IPVM::Rect rt;
	IPVM::Rect rtAll;

	edgePara.m_findFirstEdge = false;
	edgePara.m_edgeType = IPVM::EdgeType::Falling;
	edgePara.m_searchDirection = IPVM::SearchDirection::ToRight;
	edgePara.m_edgeThreshold = m_nLineThreshold;
	edgeFilter.SetParameter(6, 1.5);
	m_nFindEdge = 10;
	m_nSearchWidth = 3;

	std::vector<IPVM::LineEq_32f> vecLineEq32;
	//vecLineEq32.resize(nSize);


	m_pImageView->ROIGet(_T("RES"), rt);
	m_pImageView->ROIGet(_T("ALL"), rtAll);

	nSize = (long)rtAll.Width() / (long)rt.Width();

	std::vector<IPVM::Point_32f_C3> vecEdgePointsFalling;
	std::vector<IPVM::Point_32f_C3> vecEdgePointsRising;
	std::vector<IPVM::Point_32f_C3> vecCenterPoint;
	std::vector<IPVM::EdgeDetectionDebugInfo> vecEdgeInfos;

	std::vector<IPVM::Point_32f_C2> vecCenterPointCalc;
	std::vector<float> vecRealX;
	std::vector<float> vecRealY;

		
	nSize = 30;

	for (int n = 0; n < nSize; n++)
	{
        if (rt.m_right > rtAll.m_right)
            break;

		vecEdgePointsFalling.clear();
		vecEdgePointsRising.clear();
		vecCenterPoint.clear();
		vecEdgeInfos.clear();

		vecEdgePointsFalling.resize(m_nFindEdge);
		vecEdgePointsRising.resize(m_nFindEdge);
		vecCenterPoint.resize(m_nFindEdge);
		vecEdgeInfos.resize(m_nFindEdge);
	
		

		long nFoundEdgeCount = 0;

		long nFoundEdgeCount2 = 0;

		//if (rtAll.m_left < rt.m_left && rtAll.m_right > rt.m_right)
		{
			edgePara.m_edgeType = IPVM::EdgeType::Falling;
			
			if (IPVM::Status::success == edge.DetectEdges(*((IPVM::Image_8u_C1*)&m_image), edgePara, edgeFilter, rt, m_nFindEdge, m_nSearchWidth, &vecEdgePointsFalling[0], nFoundEdgeCount, &vecEdgeInfos[0]))
			{
				for (int i = 0; i < nFoundEdgeCount; i++)
				{
					m_pImageView->ImageOverlayAdd(IPVM::Point_32f_C2(vecEdgePointsFalling[i].m_x, vecEdgePointsFalling[i].m_y), RGB(255, 0, 0), 3);
				}
			}

			edgePara.m_edgeType = IPVM::EdgeType::Rising;
			
			if (IPVM::Status::success == edge.DetectEdges(*((IPVM::Image_8u_C1*)&m_image), edgePara, edgeFilter, rt, m_nFindEdge, m_nSearchWidth, &vecEdgePointsRising[0], nFoundEdgeCount2, &vecEdgeInfos[0]))
			{
				for (int i = 0; i < nFoundEdgeCount2; i++)
				{
					m_pImageView->ImageOverlayAdd(IPVM::Point_32f_C2(vecEdgePointsRising[i].m_x, vecEdgePointsRising[i].m_y), RGB(0, 0, 255), 3);
				}
			}
			

			std::vector<IPVM::Point_32f_C2> ptUsed;
			std::vector<IPVM::Point_32f_C2> ptReal;

			ptUsed.clear();
			ptReal.clear();

			ptUsed.resize(nFoundEdgeCount);
			ptReal.resize(nFoundEdgeCount);
			if (nFoundEdgeCount2 > nFoundEdgeCount)
			{
				nFoundEdgeCount2 = nFoundEdgeCount;
			}
			else
			{
				nFoundEdgeCount = nFoundEdgeCount2;
			}
			
			for (int i = 0; i < nFoundEdgeCount; i++)
			{
				vecCenterPoint[i].m_x = vecEdgePointsFalling[i].m_x + (vecEdgePointsRising[i].m_x - vecEdgePointsFalling[i].m_x) / 2;
				vecCenterPoint[i].m_y = vecEdgePointsFalling[i].m_y;
				m_pImageView->ImageOverlayAdd(IPVM::Point_32f_C2(vecCenterPoint[i].m_x, vecCenterPoint[i].m_y), RGB(0, 255, 0), 3);

				ptReal[i] = (IPVM::Point_32f_C2(vecCenterPoint[i].m_x, vecCenterPoint[i].m_y));
				vecRealX.push_back(vecCenterPoint[i].m_x);
				vecRealY.push_back(vecCenterPoint[i].m_y);
			}
			//std::sort(&ptReal[0], &ptReal[ptReal.size()-1]);
			
			long usedNum = 0;
			long distanceThreshold = 5;

			IPVM::LineEq_32f lineEq32;

			IPVM::DataFitting::FitToLine_RANSAC(vecCenterPoint.size(), &ptReal[0], &ptUsed[0], usedNum, lineEq32, distanceThreshold);
			if (usedNum < 3)
				continue;
			vecLineEq32.push_back(lineEq32);
			m_pImageView->ImageOverlayAdd(lineEq32, RGB(0, 255, 0), 3);
			ptUsed.resize(usedNum);
			std::sort(ptUsed.begin(), ptUsed.end(), compare);


			vecCenterPointCalc.push_back(ptUsed[ptUsed.size() / 2]);

			long nSize = rt.Width();

			rt.m_left = ptUsed[ptUsed.size() / 2].m_x + nSize / 2;
			rt.m_right = rt.m_left + nSize;
			
		}
	}
	double dLengthSum = 0.f;
	for (long n = 0; n < vecCenterPointCalc.size() - 1; n++)
	{
		float fLength;
		IPVM::Geometry::GetDistance(vecLineEq32[n], vecCenterPointCalc[n+1], fLength);
		fLength = fabs(fLength);
		dLengthSum += fLength;
		CString str;
		str.Format(_T("%f"), 10000 / fLength);
		m_pImageView->ImageOverlayAdd(vecCenterPointCalc[n + 1], str, RGB(0, 0, 255), 40);
	}

	float dLendthAvg = dLengthSum / (vecCenterPointCalc.size() - 1);
	CString str;
	str.Format(_T("res avg = %f"), 10000 / dLendthAvg);
	m_pImageView->ImageOverlayAdd(vecCenterPointCalc[0], str, RGB(0, 0, 255), 40);

	m_pImageView->ImageOverlayShow();

	
}

void CDlgCalculationResolution::OnBnClickedButtonSetLineTh()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_nLineThreshold = GetDlgItemInt(IDC_EDIT_LINE_TH);
    SetDlgItemInt(IDC_EDIT_LINE_TH, m_nLineThreshold);
}

void CDlgCalculationResolution::OnBnClickedBtnCalcres()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	IPVM::Rect rt;
    IPVM::Rect rtAll;

    UpdateData(TRUE);
    long nX = GetDlgItemInt(IDC_EDIT_ROWCNT);
    long nY = GetDlgItemInt(IDC_EDIT_COLCNT);

	m_pImageView->ROIGet(_T("RES"), rt);
    m_pImageView->ROIGet(_T("ALL"), rtAll);

    m_rtROI = rtAll;
    float fWidth = rtAll.Width() / nX;
    float fHeight = rtAll.Height() / nY;
    m_pImageView->ImageOverlayClear();
    m_pImageView->ROIClear();
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다..
    long nImageSizeX = m_image.GetSizeX();
    long nImageSizeY = m_image.GetSizeY();
    IPVM::Image_8u_C1 binImage(nImageSizeX, nImageSizeY);
    IPVM::Rect rtdown;
    IPVM::Rect rtright;

    for (long sx = 0; sx < nX; sx++)
    {
        for (long sy = 0; sy < nY; sy++)
        {
            rtdown.m_left = rtAll.m_left + (sx * fWidth) + 5;
            rtdown.m_right = rtAll.m_left + (fWidth * (sx + 1)) - 5;

            rtdown.m_top = rtAll.m_top + (fHeight * sy) + fHeight / 2;
            rtdown.m_bottom = rtAll.m_top + (fHeight * (sy + 1)) + fHeight / 2;

            rtright.m_left = rtAll.m_left + (sx * fWidth) + fWidth / 2;
            rtright.m_right = rtAll.m_left + (fWidth * (sx + 1)) + fWidth / 2;

            rtright.m_top = rtAll.m_top + (fHeight * sy) + 5;
            rtright.m_bottom = rtAll.m_top + (fHeight * (sy + 1)) - 5;

            m_pImageView->ImageOverlayAdd(rtdown, RGB(0, 255, 0));

            m_pImageView->ImageOverlayAdd(rtright, RGB(0, 0, 255));

            CalcScale(m_image.GetMem(), m_pImageView, rtdown, rtright);
        }
    }

    m_pImageView->ImageOverlayShow();

    

    std::vector<float> tempX;
    std::vector<float> tempY;
    if (m_vecTemp.size() > 0)
    {
        for (long sx = 0; sx < nX; sx++)
        {
            for (long sy = 0; sy < nY; sy++)
            {
                long nTemp = sx * nY + sy;
                long nTemp2 = sx * nY + sy - 1;
                if (sy > 0)
                {
                    tempY.push_back(fabs(double(m_vecTemp[nTemp].m_y - m_vecTemp[nTemp2].m_y)));
                }
            }
        }

        for (long sx = 0; sx < nX; sx++)
        {
            for (long sy = 0; sy < nY; sy++)
            {
                long nTemp = sx * nY + sy;
                long nTemp2 = ((sx - 1) * nY) + sy;

                if (sx > 0)
                {
                    tempX.push_back(fabs(double(m_vecTemp[nTemp].m_x - m_vecTemp[nTemp2].m_x)));
                }
            }
        }
        //m_vecTemp
    }

    double tempSumX = 0.f;

    double tempSumY = 0.f;
    for (long n = 0; n < tempY.size(); n++)
    {
        tempY[n] = tempY[n];
        tempSumY += tempY[n];
    }

    for (long n = 0; n < tempX.size(); n++)
    {
        tempX[n] = tempX[n];
        tempSumX += tempX[n];
    }
    tempSumY = tempSumY / tempY.size();
    tempSumX = tempSumX / tempX.size();
    m_tempX = tempSumX;
    m_tempY = tempSumY;
    CString str;

    //5미리 격자일때
    tempSumX = 5 / tempSumX * 1000;
    tempSumY = 5 / tempSumY * 1000;
    str.Format(_T("5mm 타겟 %f um, %f um"), tempSumX, tempSumY);

    //1미리 격자일때
    //tempSumX = 1/tempSumX*1000;
    //tempSumY = 1/tempSumY*1000;
    //str.Format(_T("1mm 타겟 %f um, %f um") ,tempSumX, tempSumY);

    for (long n = m_vecTemp.size() - 1; n > -1; n--)
    {
        if (m_vecTemp[n].m_x > rtAll.m_left && m_vecTemp[n].m_x < rt.m_right
            && m_vecTemp[n].m_y > rtAll.m_top && m_vecTemp[n].m_y < rt.m_bottom)
        {
        }
        else
        {
            m_vecTemp2.push_back(m_vecTemp[n]);
        }
    }

    m_vecTemp = m_vecTemp2;
    for (long sx = rt.m_left; sx < rt.m_right; sx++)
    {
        for (long sy = rt.m_top; sy < rt.m_bottom; sy++)
        {
        }
    }
    AfxMessageBox(str);
    long n = 0;

}

void CDlgCalculationResolution::OnBnClickedBtnSavedistor()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    long nImageSizeX = m_image.GetSizeX();
    long nImageSizeY = m_image.GetSizeY();
    IPVM::Image_8u_C1 binImage(nImageSizeX, nImageSizeY);
    CRect rtImageROI = IPVM::ToMFC(m_rtROI); //IPVM::ToMFC(m_Img_8uC1);

    std::vector<IPVM::Point_32f_C2> vecSortedInputPoints;
    std::vector<std::vector<IPVM::Point_32f_C2>> vecHorLinePoints;
    IPVM::Point_32f_C2* pPositionXY = &m_vecTemp[0];
    long nPositionCount = m_vecTemp.size();

    if (FALSE == GetHorLinePoints(pPositionXY, nPositionCount, vecHorLinePoints, vecSortedInputPoints))
    {
        return;
    }

    // 1.1 수평 레퍼런스 라인 구하기
    std::vector<IPVM::LineEq_32f> vecHorLineEquations;

    if (FALSE == GetHorLineEquations(rtImageROI, vecHorLinePoints, vecHorLineEquations))
    {
        return;
    }

    // 1.2 입력 격자 패턴의 각 점들을 수직 라인 기준으로 그룹화
    std::vector<std::vector<IPVM::Point_32f_C2>> vecVerLinePoints;

    if (FALSE == GetVerLinePoints(pPositionXY, nPositionCount, vecVerLinePoints))
    {
        return;
    }

    // 1.3 수직 레퍼런스 라인 구하기
    std::vector<IPVM::LineEq_32f> vecVerLineEquations;

    if (FALSE == GetVerLineEquations(rtImageROI, vecVerLinePoints, vecVerLineEquations))
    {
        return;
    }

    // 1.4 수평, 수직 라인으로부터 교점 구하기
    std::vector<IPVM::Point_32f_C2> vecRefPositions;

    if (FALSE == GetReferencePosition(vecHorLineEquations, vecVerLineEquations, vecSortedInputPoints, vecRefPositions))
    {
        return;
    }

    CCalMap_Distortion cal_Distortion;
    if (FALSE
        == cal_Distortion.SetTransform(
            (long)vecSortedInputPoints.size(), &vecSortedInputPoints[0], &vecRefPositions[0]))
    {
        // 계산실패
        return;
    }

    if (FALSE
        == cal_Distortion.SetTransform(
            (long)vecSortedInputPoints.size(), &vecRefPositions[0], &vecSortedInputPoints[0]))
    {
        // 계산실패
        return;
    }
    std::vector<float> vecfDistortion;

    if (FALSE == GetDistortion(vecRefPositions, cal_Distortion, vecfDistortion))
    {
        return;
    }
    CTime time = CTime::GetCurrentTime();
    CString szTime = time.Format("%Y-%m-%d-%a-%H-%M-%S_distortion.tmp");
    szTime = _T("C:\\Intekplus\\IBCI\\") + szTime;
    cal_Distortion.Save(szTime);
    cal_Distortion.Modify(m_image, CPoint(0, 0), binImage);
    m_pImageView->SetImage(binImage);
    binImage.SaveBmp(_T("D:\\ModifyDone.bmp"));
}

BOOL CDlgCalculationResolution::CalcScale(BYTE* byImage, IPVM::ImageView* pImage, IPVM::Rect rtDown, IPVM::Rect rtRight)
{
    double dDev = GetDlgItemInt(IDC_EDIT_THDEV);
    double dThres = GetDlgItemInt(IDC_EDIT_TH);
    long nEdgeMode = GetDlgItemInt(IDC_EDIT_EDGEMODE);


    IPVM::EdgeDetection edge;
    IPVM::EdgeDetectionPara edgePara;
    IPVM::EdgeDetectionDebugInfo EdgeInfo;
    IPVM::EdgeDetectionFilter edgeFilter;
    IPVM::Point_32f_C3 edgePoint;

    IPVM::Rect rtROI1 = rtDown;
    IPVM::Rect rtROI2 = rtRight;

    edgePara.m_findFirstEdge = false;
    edgePara.m_edgeType = (IPVM::EdgeType)nEdgeMode;
    edgePara.m_searchDirection = IPVM::SearchDirection::ToDown;
    edgePara.m_edgeThreshold = dThres;
    edgeFilter.SetParameter(6, dDev);
    

    std::vector<IPVM::LineEq_32f> vecLineEq32;
    

    std::vector<IPVM::Point_32f_C3> vecEdgePointsROI1;
    std::vector<IPVM::Point_32f_C3> vecEdgePointsROI2;
    std::vector<IPVM::Point_32f_C3> vecCenterPoint;
    std::vector<IPVM::EdgeDetectionDebugInfo> vecEdgeInfos;

    std::vector<IPVM::Point_32f_C2> vecCenterPointCalc;

    m_nFindEdge = 1;

    vecEdgePointsROI1.clear();
    vecEdgePointsROI2.clear();
    vecCenterPoint.clear();
    vecEdgeInfos.clear();

    vecEdgePointsROI1.resize(m_nFindEdge);
    vecEdgePointsROI2.resize(m_nFindEdge);
    vecCenterPoint.resize(m_nFindEdge);
    vecEdgeInfos.resize(m_nFindEdge);

    
    std::vector<float> vecX;
    std::vector<float> vecY;

    
    long imageSizeX = m_image.GetSizeX();
    long imageSizeY = m_image.GetSizeY();
    long nSearchAreaDivision = 5;
    
    m_nSearchWidth = 20;
    long nFoundEdgeCount = 0;
    long nFoundEdgeCount2 = 0;
    BOOL bCalc_X = TRUE;
    BOOL bCalc_Y = TRUE;
    BOOL bError = FALSE;
    long n = 0;
    std::vector<float> Calc_angle;
    Calc_angle.resize(n + 1);
    for (n; n < 1; n++)
    {
        CString strNumber;
        strNumber.Format(_T("_%d"), n);


        BOOL bAlignSucces = TRUE;
        

        std::vector<IPVM::Point_32f_C2> vecEdges1, vecEdges2;
        

        long nAreaDivisionlength1 = (rtROI1.m_right - rtROI1.m_left - m_nSearchWidth)
            / (nSearchAreaDivision);
        long nAreaDivisionlength2 = (rtROI2.m_bottom - rtROI2.m_top - m_nSearchWidth)
            / (nSearchAreaDivision);
        long nSearchDir1, nSearchDir2;
        for (long idx = 0; idx < nSearchAreaDivision; idx++)
        {
            CPoint ptStart;

            switch (edgePara.m_searchDirection)
            {
                case IPVM::SearchDirection::ToUp: //up
                    if (idx != 0)
                    {
                        rtROI1.m_left = rtROI1.m_left + nAreaDivisionlength1;
                        rtROI1.m_right = rtROI1.m_right + nAreaDivisionlength1;
                        
                        if (rtDown.Height() < rtROI1.m_right)
                            rtROI1.m_right = rtDown.m_right - nAreaDivisionlength2;
                        
                    }
                    else
                    {
                        rtROI1.m_right = rtROI1.m_left + nAreaDivisionlength1;
                    }
                    break;
                case IPVM::SearchDirection::ToDown: //down
                    if (idx != 0)
                    {
                        
                        rtROI1.m_left = rtROI1.m_left + nAreaDivisionlength1;
                        rtROI1.m_right = rtROI1.m_right + nAreaDivisionlength1;
                        if (rtDown.m_right < rtROI1.m_right)
                            rtROI1.m_right = rtDown.m_right - nAreaDivisionlength2;
                        
                    }
                    else
                    {
                        rtROI1.m_right = rtROI1.m_left + nAreaDivisionlength1;
                    }
                    break;
                default:
                    break;
            }

            float fEdgeX = 0.f;
            float fEdgeY = 0.f;
            float fEdgeValue = 0.f;

            if (IPVM::Status::success
                != edge.DetectEdges(*((IPVM::Image_8u_C1*)&m_image), edgePara, edgeFilter, rtROI1, m_nFindEdge,
                    m_nSearchWidth, &vecEdgePointsROI1[0], nFoundEdgeCount, &vecEdgeInfos[0]))
            {
                bAlignSucces = FALSE;
            }
            fEdgeX = vecEdgePointsROI1.front().m_x;
            fEdgeY = vecEdgePointsROI1.front().m_y;
            vecEdges1.emplace_back(fEdgeX, fEdgeY);
        }
        edgePara.m_searchDirection = IPVM::SearchDirection::ToRight;
        for (long idx = 0; idx < nSearchAreaDivision; idx++)
        {
            CPoint ptStart;

            switch (edgePara.m_searchDirection)
            {
                case IPVM::SearchDirection::ToLeft: //left
                    if (idx != 0)
                    {
                        rtROI2.m_top = rtROI2.m_top + nAreaDivisionlength2;
                        rtROI2.m_bottom = rtROI2.m_bottom + nAreaDivisionlength2;
                        if (rtROI2.m_bottom > rtRight.Height())
                            rtROI2.m_bottom = rtRight.Height() - nAreaDivisionlength2;
                    }
                    else
                    {
                        rtROI2.m_bottom = rtROI2.m_top + nAreaDivisionlength2;
                    }
                    break;
                case IPVM::SearchDirection::ToRight: //right
                    if (idx != 0)
                    {
                        rtROI2.m_top = rtROI2.m_top + nAreaDivisionlength2;
                        rtROI2.m_bottom = rtROI2.m_bottom + nAreaDivisionlength2;
                        if (rtROI2.m_bottom > rtRight.m_bottom)
                            rtROI2.m_bottom = rtRight.m_bottom - nAreaDivisionlength2;
                    }
                    else
                    {
                        rtROI2.m_bottom = rtROI2.m_top + nAreaDivisionlength2;
                    }
                    break;
                default:
                    break;
            }

            float fEdgeX = 0.f;
            float fEdgeY = 0.f;
            float fEdgeValue = 0.f;

            if (IPVM::Status::success
                != edge.DetectEdges(*((IPVM::Image_8u_C1*)&m_image), edgePara, edgeFilter, rtROI2, m_nFindEdge,
                    m_nSearchWidth, &vecEdgePointsROI2[0], nFoundEdgeCount, &vecEdgeInfos[0]))
            {
                bAlignSucces = FALSE;
            }
            fEdgeX = vecEdgePointsROI2.front().m_x;
            fEdgeY = vecEdgePointsROI2.front().m_y;
            vecEdges2.emplace_back(fEdgeX, fEdgeY);
        }

        IPVM::LineEq_32f Line1, Line2;

        if (bAlignSucces)
        {
            /////////////////////
            long nLinePointsNum1 = vecEdges1.size();
            long nLinePointsNum2 = vecEdges2.size();
            IPVM::Point_32f_C2* pointsLine1 = new IPVM::Point_32f_C2[nLinePointsNum1];
            IPVM::Point_32f_C2* pointsLine2 = new IPVM::Point_32f_C2[nLinePointsNum2];

            for (long idx = 0; idx < nLinePointsNum1; idx++)
            {
                pointsLine1[idx].m_x = vecEdges1[idx].m_x;
                pointsLine1[idx].m_y = vecEdges1[idx].m_y;
            }
            for (long idx = 0; idx < nLinePointsNum2; idx++)
            {
                pointsLine2[idx].m_x = vecEdges2[idx].m_x;
                pointsLine2[idx].m_y = vecEdges2[idx].m_y;
            }

            IPVM::Point_32f_C2* pointsLineUsed1 = new IPVM::Point_32f_C2[nLinePointsNum1];
            IPVM::Point_32f_C2* pointsLineUsed2 = new IPVM::Point_32f_C2[nLinePointsNum2];
            long nLinePointsNumUsed1 = nLinePointsNum1;
            long nLinePointsNumUsed2 = nLinePointsNum2;

            float fInlierDistanceThreshold = 1;
            if (fInlierDistanceThreshold != 0)
            {
                IPVM::DataFitting::FitToLine_RANSAC(vecEdges1.size(), pointsLine1, pointsLineUsed1, nLinePointsNumUsed1,
                    Line1, fInlierDistanceThreshold /*m_AlignSpec[n].m_fInlierDistanceThreshold*/);
                IPVM::DataFitting::FitToLine_RANSAC(vecEdges2.size(), pointsLine2, pointsLineUsed2, nLinePointsNumUsed2,
                    Line2, fInlierDistanceThreshold /*m_AlignSpec[n].m_fInlierDistanceThreshold*/);
            }
            else
            {
                IPVM::DataFitting::FitToLine(vecEdges1.size(), pointsLine1, Line1);
                IPVM::DataFitting::FitToLine(vecEdges2.size(), pointsLine2, Line2);
                pointsLineUsed1 = pointsLine1;
                pointsLineUsed2 = pointsLine2;
            }

            const float a1 = Line1.m_a;
            const float b1 = Line1.m_b;
            const float c1 = Line1.m_c;

            const float a2 = Line2.m_a;
            const float b2 = Line2.m_b;
            const float c2 = Line2.m_c;

           
            IPVM::Point_32f_C2 fPos(0, 0);

            if ((a1 != 0.f || b1 != 0.f || c1 != 0.f))
            {
                IPVM::Geometry::GetCrossPoint_LNvLN(Line1, Line2, fPos);
            }

            IPVM::LineSeg_32f LineSeg;
            if (fabs(Line1.m_a) < fabs(Line1.m_b))
            {
                LineSeg.m_sx = 0.f;
                LineSeg.m_sy = Line1.GetPositionY(0.f);
                LineSeg.m_ex = (float)imageSizeX;
                LineSeg.m_ey = Line1.GetPositionY((float)imageSizeY);
            }
            else
            {
                LineSeg.m_sx = Line1.GetPositionX(0.f);
                LineSeg.m_sy = 0.f;
                LineSeg.m_ex = Line1.GetPositionX((float)imageSizeX);
                LineSeg.m_ey = (float)imageSizeY;
            }

            IPVM::LineSeg_32f LineSeg2;
            if (fabs(Line2.m_a) < fabs(Line2.m_b))
            {
                LineSeg2.m_sx = 0.f;
                LineSeg2.m_sy = Line2.GetPositionY(0.f);
                LineSeg2.m_ex = (float)imageSizeX;
                LineSeg2.m_ey = Line2.GetPositionY((float)imageSizeY);
            }
            else
            {
                LineSeg2.m_sx = Line2.GetPositionX(0.f);
                LineSeg2.m_sy = 0.f;
                LineSeg2.m_ex = Line2.GetPositionX((float)imageSizeX);
                LineSeg2.m_ey = (float)imageSizeY;
            }

            for (int count = 0; count < nLinePointsNumUsed1; count++)
            {
                IPVM::CircleEq_32f circle;
                circle.m_x = pointsLineUsed1[count].m_x;
                circle.m_y = pointsLineUsed1[count].m_y;
                circle.m_radius = 2.f;
                m_pImageView->ImageOverlayAdd(circle, RGB(0, 0, 255));
            }
            for (int count = 0; count < nLinePointsNumUsed2; count++)
            {
                IPVM::CircleEq_32f circle;
                circle.m_x = pointsLineUsed2[count].m_x;
                circle.m_y = pointsLineUsed2[count].m_y;
                circle.m_radius = 2.f;
                m_pImageView->ImageOverlayAdd(circle, RGB(0, 0, 255));
            }

            if (bCalc_X == TRUE && bCalc_Y == TRUE)
            {
                float ftemp;
                Get1LineAngle(LineSeg.m_ex, LineSeg.m_ey, LineSeg.m_sx, LineSeg.m_sy, Calc_angle[0]);
                Calc_angle[0] = -(Calc_angle[0] + 90);
                Get1LineAngle(LineSeg2.m_sx, LineSeg2.m_sy, LineSeg2.m_ex, LineSeg2.m_ey, ftemp);
                //ftemp = ftemp + 90;
                ftemp = -(ftemp);
                Calc_angle[0] = (Calc_angle[0] + ftemp) / 2;
            }
            else
            {
                if (bCalc_X)
                {
                    Get1LineAngle(LineSeg.m_ex, LineSeg.m_ey, LineSeg.m_sx, LineSeg.m_sy, Calc_angle[0]);
                    Calc_angle[0] = -(Calc_angle[0] + 90);
                }
                else if (bCalc_Y)
                {
                    Get1LineAngle(LineSeg2.m_sx, LineSeg2.m_sy, LineSeg2.m_ex, LineSeg2.m_ey, Calc_angle[0]);
                    Calc_angle[0] = -(Calc_angle[0]);
                }
                else
                {
                    Calc_angle[0] = 0;
                }
            }

            
            IPVM::Point_32f_C2 p_ctop = {(float)20, (float)(20 + 40 * n)};
            IPVM::Point_32f_C2 p_c2 = {fPos.m_x, fPos.m_y};
            IPVM::CircleEq_32f circle;
            circle.m_x = fPos.m_x;
            circle.m_y = fPos.m_y;
            circle.m_radius = 5.f;
            m_vecTemp.push_back(p_c2);
            m_pImageView->ImageOverlayAdd(circle, RGB(255, 0, 0));
        }
        else
        {
            CString strPoint;
            strPoint.Format(_T("Calc_%d ERROR"), n + 1);
            IPVM::Point_32f_C2 p_ctop = {(float)20, (float)(20 + 40 * n)};
            m_pImageView->ImageOverlayAdd(p_ctop, strPoint, RGB(255, 0, 0), 40);
            bError = TRUE;
        }
    }
    m_pImageView->ImageOverlayShow();
    return TRUE;
}

BOOL CDlgCalculationResolution::GetHorLinePoints(const IPVM::Point_32f_C2* pPoints, long nPointCount,
    std::vector<std::vector<IPVM::Point_32f_C2>>& vecHorLinePoints, std::vector<IPVM::Point_32f_C2>& vecSortedPointsByX)
{
    if (nPointCount == 0)
        return FALSE;

    std::vector<IPVM::Point_32f_C2> vecInputPoints(pPoints, pPoints + nPointCount);

    IPVM::Point_32f_C2* pInputPoints = &vecInputPoints[0];

    // Y 기준으로 먼저 소팅을 수행한다.
    for (long idx = 0; idx < nPointCount; idx++)
    {
        for (long idx2 = idx + 1; idx2 < nPointCount; idx2++)
        {
            if (pInputPoints[idx].m_y > pInputPoints[idx2].m_y)
            {
                std::swap(pInputPoints[idx], pInputPoints[idx2]);
            }
        }
    }

    // Y 좌표 증가값의 평균을 구한다.
    float fAverageIncY = 0.f;

    for (long idx = 1; idx < nPointCount; idx++)
    {
        fAverageIncY += (pInputPoints[idx].m_y - pInputPoints[idx - 1].m_y);
    }

    fAverageIncY /= (nPointCount - 1);

    // Y 좌표 증가값의 평균보다 큰 증가를 하는 경우가 라인이 바뀌는 경우이므로
    // 라인이 바뀔 때의 Y 값 증가 평균을 다시 구한다.
    float fAverageLineIncY = 0.f;
    long nAverageLineIncCount = 0;

    for (long idx = 1; idx < nPointCount; idx++)
    {
        float fInc = pInputPoints[idx].m_y - pInputPoints[idx - 1].m_y;

        if (fInc > fAverageIncY)
        {
            fAverageLineIncY += fInc;
            nAverageLineIncCount++;
        }
    }

    if (nAverageLineIncCount == 0)
    {
        return FALSE;
    }

    fAverageLineIncY /= nAverageLineIncCount;

    // 라인 간격 평균의 절반을 기준값으로 해서,
    // 이 값보다 크게 증가하는 경우를 라인 변경으로 생각해서 포인트들을 그룹화한다.

    fAverageLineIncY *= 0.5f;

    vecHorLinePoints.clear();
    vecHorLinePoints.resize(1, std::vector<IPVM::Point_32f_C2>(1, pInputPoints[0]));

    for (long idx = 1; idx < nPointCount; idx++)
    {
        if (pInputPoints[idx].m_y - pInputPoints[idx - 1].m_y > fAverageLineIncY)
        {
            vecHorLinePoints.resize(vecHorLinePoints.size() + 1);
        }

        vecHorLinePoints.back().push_back(pInputPoints[idx]);
    }

    // 각 라인 그룹 내에서 X 좌표 기준으로 소팅한다.
    const long nHorLineCount = (long)vecHorLinePoints.size();

    for (long nHorLine = 0; nHorLine < nHorLineCount; nHorLine++)
    {
        std::vector<IPVM::Point_32f_C2>& vecLinePoints = vecHorLinePoints[nHorLine];

        const long nPointCounts = (long)vecLinePoints.size();

        if (nPointCounts == 0)
        {
            return FALSE;
        }

        IPVM::Point_32f_C2* pLinePoints = &vecLinePoints[0];

        for (long nX1 = 0; nX1 < nPointCounts; nX1++)
        {
            for (long nX2 = nX1 + 1; nX2 < nPointCounts; nX2++)
            {
                if (pLinePoints[nX2].m_x < pLinePoints[nX1].m_x)
                {
                    std::swap(pLinePoints[nX2], pLinePoints[nX1]);
                }
            }
        }
    }

    // 소팅된 결과를 쫙 넣는다.
    vecSortedPointsByX.clear();

    for (long nHorLine = 0; nHorLine < nHorLineCount; nHorLine++)
    {
        std::vector<IPVM::Point_32f_C2>& vecLinePoints = vecHorLinePoints[nHorLine];

        vecSortedPointsByX.insert(vecSortedPointsByX.end(), vecLinePoints.begin(), vecLinePoints.end());
    }

    // 각 라인별 개수를 체크해서 이상하면 포기한다.
    const long nLinePointCount = long(vecSortedPointsByX.size() / nHorLineCount);

    for (long nHorLine = 0; nHorLine < nHorLineCount; nHorLine++)
    {
        std::vector<IPVM::Point_32f_C2>& vecLinePoints = vecHorLinePoints[nHorLine];

        if ((long)vecLinePoints.size() < nLinePointCount / 2)
        {
            return FALSE;
        }
    }

    return TRUE;
}


BOOL CDlgCalculationResolution::GetVerLinePoints(
    const IPVM::Point_32f_C2* pPoints, long nPointCount, std::vector<std::vector<IPVM::Point_32f_C2>>& vecVerLinePoints)
{
    std::vector<IPVM::Point_32f_C2> vecInputPoints(pPoints, pPoints + nPointCount);

    IPVM::Point_32f_C2* pInputPoints = &vecInputPoints[0];

    // X 기준으로 먼저 소팅을 수행한다.
    for (long idx = 0; idx < nPointCount; idx++)
    {
        for (long idx2 = idx + 1; idx2 < nPointCount; idx2++)
        {
            if (pInputPoints[idx].m_x > pInputPoints[idx2].m_x)
            {
                std::swap(pInputPoints[idx], pInputPoints[idx2]);
            }
        }
    }

    // X 좌표 증가값의 평균을 구한다.
    float fAverageIncX = 0.f;

    for (long idx = 1; idx < nPointCount; idx++)
    {
        fAverageIncX += (pInputPoints[idx].m_x - pInputPoints[idx - 1].m_x);
    }

    fAverageIncX /= (nPointCount - 1);

    // Y 좌표 증가값의 평균보다 큰 증가를 하는 경우가 라인이 바뀌는 경우이므로
    // 라인이 바뀔 때의 Y 값 증가 평균을 다시 구한다.
    float fAverageLineIncX = 0.f;
    long nAverageLineIncCount = 0;

    for (long idx = 1; idx < nPointCount; idx++)
    {
        float fInc = pInputPoints[idx].m_x - pInputPoints[idx - 1].m_x;

        if (fInc > fAverageIncX)
        {
            fAverageLineIncX += fInc;
            nAverageLineIncCount++;
        }
    }

    if (nAverageLineIncCount == 0)
    {
        return FALSE;
    }

    fAverageLineIncX /= nAverageLineIncCount;

    // 라인 간격 평균의 절반을 기준값으로 해서,
    // 이 값보다 크게 증가하는 경우를 라인 변경으로 생각해서 포인트들을 그룹화한다.

    fAverageLineIncX *= 0.5f;

    vecVerLinePoints.clear();
    vecVerLinePoints.resize(1, std::vector<IPVM::Point_32f_C2>(1, pInputPoints[0]));

    for (long idx = 1; idx < nPointCount; idx++)
    {
        if (pInputPoints[idx].m_x - pInputPoints[idx - 1].m_x > fAverageLineIncX)
        {
            vecVerLinePoints.resize(vecVerLinePoints.size() + 1);
        }

        vecVerLinePoints.back().push_back(pInputPoints[idx]);
    }

    // 각 라인 그룹 내에서 Y 좌표 기준으로 소팅한다.
    const long nVerLineCount = (long)vecVerLinePoints.size();

    for (long nHorLine = 0; nHorLine < nVerLineCount; nHorLine++)
    {
        std::vector<IPVM::Point_32f_C2>& vecLinePoints = vecVerLinePoints[nHorLine];

        const long nPointCounts = (long)vecLinePoints.size();

        if (nPointCounts == 0)
        {
            return FALSE;
        }

        IPVM::Point_32f_C2* pLinePoints = &vecLinePoints[0];

        for (long nY1 = 0; nY1 < nPointCounts; nY1++)
        {
            for (long nY2 = nY1 + 1; nY2 < nPointCounts; nY2++)
            {
                if (pLinePoints[nY2].m_y < pLinePoints[nY1].m_y)
                {
                    std::swap(pLinePoints[nY2], pLinePoints[nY1]);
                }
            }
        }
    }

    return TRUE;
}

BOOL CDlgCalculationResolution::GetHorLineEquations(CRect rtImageROI,
    const std::vector<std::vector<IPVM::Point_32f_C2>>& vecHorLinePoints, std::vector<IPVM::LineEq_32f>& vecHorLineEquations)
{
    const long nHorLineCount = (long)vecHorLinePoints.size();

    vecHorLineEquations.resize(nHorLineCount);

    // ax + by + c = 0 타입으로 방정식 구하기.
    for (long nHorLine = 0; nHorLine < nHorLineCount; nHorLine++)
    {
        const std::vector<IPVM::Point_32f_C2>& vecPoints = vecHorLinePoints[nHorLine];
        IPVM::Point_32f_C2* pTemp;
        pTemp = new IPVM::Point_32f_C2[vecPoints.size()];
        for (long n = 0; n < vecPoints.size(); n++)
        {
            pTemp[n] = vecPoints[n];
        }
        IPVM::LineEq_32f& line = vecHorLineEquations[nHorLine];

        if (IPVM::Status::success != IPVM::DataFitting::FitToLine(vecPoints.size(), pTemp, line))
        {
            return FALSE;
        }
    }

    // y = Ax + B 타입으로 변환
    std::vector<float> vecCoefA(nHorLineCount);
    std::vector<float> vecCoefB(nHorLineCount);

    for (long nHorLine = 0; nHorLine < nHorLineCount; nHorLine++)
    {
        //		const vector<IV_dataPoint> &vecPoints = vecHorLinePoints[nHorLine];

        IPVM::LineEq_32f& line = vecHorLineEquations[nHorLine];

        vecCoefA[nHorLine] = -line.m_a / line.m_b;
        vecCoefB[nHorLine] = -line.m_c / line.m_b;
    }

    // 평균 라인과 Y 절편 간격의 평균을 구한다.
    float fAvgA = 0.f;
    float fAvgB = 0.f;
    float fIncB = 0.f;

    for (long nHorLine = 0; nHorLine < nHorLineCount; nHorLine++)
    {
        fAvgA += vecCoefA[nHorLine];
        fAvgB += vecCoefB[nHorLine];

        if (nHorLine > 0)
        {
            fIncB += (vecCoefB[nHorLine] - vecCoefB[nHorLine - 1]);
        }
    }

    fAvgA /= nHorLineCount;
    fAvgB /= nHorLineCount;
    fIncB /= (nHorLineCount - 1);

    // 수평 라인을 새로 만든다.
    // 라인 각도는 평균 각도를 사용하고,
    // 라인 절편은 평균 절편을 기준으로 절편 증분을 고려하도록 한다.
    const float fOffsetB = fAvgB - (nHorLineCount - 1) * fIncB * 0.5f;

    for (long nHorLine = 0; nHorLine < nHorLineCount; nHorLine++)
    {
        IPVM::LineEq_32f& line = vecHorLineEquations[nHorLine];

        line.m_a = fAvgA;
        line.m_b = -1.f;
        line.m_c = fOffsetB + nHorLine * fIncB;
    }

    // 수평 라인이므로 기울기가 0 도인 라인을 다시 찾는다.
    const float fImageCenX = 0.5f * (rtImageROI.Width() - 1);

    for (long nHorLine = 0; nHorLine < nHorLineCount; nHorLine++)
    {
        IPVM::LineEq_32f& line = vecHorLineEquations[nHorLine];

        const float y0 = -line.m_a / line.m_b * fImageCenX - line.m_c / line.m_b;

        line.m_a = 0.f;
        line.m_b = -1.f;
        line.m_c = y0;
    }

    return TRUE;
}


BOOL CDlgCalculationResolution::GetVerLineEquations(CRect rtImageROI,
    const std::vector<std::vector<IPVM::Point_32f_C2>>& vecVerLinePoints, std::vector<IPVM::LineEq_32f>& vecVerLineEquations)
{
    const long nVerLineCount = (long)vecVerLinePoints.size();

    vecVerLineEquations.resize(nVerLineCount);

    // ax + by + c = 0 타입으로 방정식 구하기.
    for (long nVerLine = 0; nVerLine < nVerLineCount; nVerLine++)
    {
        const std::vector<IPVM::Point_32f_C2>& vecPoints = vecVerLinePoints[nVerLine];

        IPVM::LineEq_32f& line = vecVerLineEquations[nVerLine];
        IPVM::Point_32f_C2* pTemp;
        pTemp = new IPVM::Point_32f_C2[vecPoints.size()];
        for (long n = 0; n < vecPoints.size(); n++)
        {
            pTemp[n] = vecPoints[n];
        }
        if (IPVM::Status::success != IPVM::DataFitting::FitToLine(vecPoints.size(), pTemp, line))
        {
            return FALSE;
        }
    }

    // x = Ay + B 타입으로 변환
    std::vector<float> vecCoefA(nVerLineCount);
    std::vector<float> vecCoefB(nVerLineCount);

    for (long nVerLine = 0; nVerLine < nVerLineCount; nVerLine++)
    {
        //		const vector<IV_dataPoint> &vecPoints = vecVerLinePoints[nVerLine];

        IPVM::LineEq_32f& line = vecVerLineEquations[nVerLine];

        vecCoefA[nVerLine] = -line.m_b / line.m_a;
        vecCoefB[nVerLine] = -line.m_c / line.m_a;
    }

    // 평균 라인과 X 절편 간격의 평균을 구한다.
    float fAvgA = 0.f;
    float fAvgB = 0.f;
    float fIncB = 0.f;

    for (long nVerLine = 0; nVerLine < nVerLineCount; nVerLine++)
    {
        fAvgA += vecCoefA[nVerLine];
        fAvgB += vecCoefB[nVerLine];

        if (nVerLine > 0)
        {
            fIncB += (vecCoefB[nVerLine] - vecCoefB[nVerLine - 1]);
        }
    }

    fAvgA /= nVerLineCount;
    fAvgB /= nVerLineCount;
    fIncB /= (nVerLineCount - 1);

    // 수평 라인을 새로 만든다.
    // 라인 각도는 평균 각도를 사용하고,
    // 라인 절편은 평균 절편을 기준으로 절편 증분을 고려하도록 한다.
    const float fOffsetB = fAvgB - (nVerLineCount - 1) * fIncB * 0.5f;

    for (long nVerLine = 0; nVerLine < nVerLineCount; nVerLine++)
    {
        IPVM::LineEq_32f& line = vecVerLineEquations[nVerLine];

        line.m_a = 1.f;
        line.m_b = -fAvgA;
        line.m_c = -(fOffsetB + nVerLine * fIncB);
    }

    // 수직 라인이므로 기울기가 90 도인 라인을 다시 찾는다.
    const float fImageCenY = 0.5f * (rtImageROI.Height() - 1);

    for (long nVerLine = 0; nVerLine < nVerLineCount; nVerLine++)
    {
        IPVM::LineEq_32f& line = vecVerLineEquations[nVerLine];

        const float x0 = -line.m_b / line.m_a * fImageCenY - line.m_c / line.m_a;

        line.m_a = -1.f;
        line.m_b = 0.f;
        line.m_c = x0;
    }
    return TRUE;
}

BOOL CDlgCalculationResolution::GetReferencePosition(const std::vector<IPVM::LineEq_32f>& vecHorLineEquations,
    const std::vector<IPVM::LineEq_32f>& vecVerLineEquations, const std::vector<IPVM::Point_32f_C2>& vecSortedInputPoints,
    std::vector<IPVM::Point_32f_C2>& vecRefPositions)
{
    //const vector<IPVM::LineEq_32f> &vecHorLineEquations, const vector<IPVM::LineEq_32f> &vecVerLineEquations, const vector<IPVM::Point_32f_C2> &vecSortedInputPoints, vector<IPVM::Point_32f_C2> &vecRefPositions)
    //BOOL CIIFtoBMP01_16Dlg::GetReferencePosition((const vector<IPVM::LineEq_32f> &vecHorLineEquations, const vector<IPVM::LineEq_32f> &vecVerLineEquations, const vector<IPVM::Point_32f_C2> &vecSortedInputPoints, vector<IPVM::Point_32f_C2> &vecRefPositions)
    //{
    const long nHorLineCount = (long)vecHorLineEquations.size();
    const long nVerLineCount = (long)vecVerLineEquations.size();

    vecRefPositions.resize(nHorLineCount * nVerLineCount);
    std::vector<IPVM::Point_32f_C2> temp;
    temp.resize(nHorLineCount * nVerLineCount);

    //temp = vecRefPositions;
    for (long nHorLine = 0; nHorLine < nHorLineCount; nHorLine++)
    {
        for (long nVerLine = 0; nVerLine < nVerLineCount; nVerLine++)
        {
            if (IPVM::Geometry::GetCrossPoint_LNvLN(vecHorLineEquations[nHorLine], vecVerLineEquations[nVerLine],
                    temp[nHorLine * nVerLineCount + nVerLine])
                != IPVM::Status::success)
            {
                return FALSE;
            }
        }
    }

    for (long n = 0; n < vecRefPositions.size(); n++)
    {
        vecRefPositions[n].m_x = temp[n].m_x;
        vecRefPositions[n].m_y = temp[n].m_y;
    }

    const float fHalfLineGap = 0.5f * (vecRefPositions[1].m_x - vecRefPositions[0].m_x);

    std::vector<IPVM::Point_32f_C2>::iterator itr = vecRefPositions.begin();

    while (itr != vecRefPositions.end())
    {
        float fMinDistance = 10000000.f;

        std::vector<IPVM::Point_32f_C2>::const_iterator itrInput = vecSortedInputPoints.begin();

        while (itrInput != vecSortedInputPoints.end())
        {
            float fDistance = (itr->m_x - itrInput->m_x) * (itr->m_x - itrInput->m_x);
            fDistance += (itr->m_y - itrInput->m_y) * (itr->m_y - itrInput->m_y);
            fDistance = sqrt(fDistance);

            if (fDistance < fMinDistance)
            {
                fMinDistance = fDistance;
            }

            itrInput++;
        }

        if (fMinDistance > fHalfLineGap)
        {
            itr = vecRefPositions.erase(itr);
            continue;
        }

        ++itr;
    }

    return TRUE;
}

BOOL CDlgCalculationResolution::GetDistortion(const std::vector<IPVM::Point_32f_C2>& vecRefPoints, CCalMap_Distortion& distortionMap, std::vector<float>& vecfDistortion)
{
    const long nPointCount = (long)vecRefPoints.size();

    vecfDistortion.clear();
    vecfDistortion.resize(nPointCount, 0.f);

    const long nRowCount = 10;

    IPVM::Matrix matrixTempIN(nRowCount, 1);
    IPVM::Matrix matrixTempOUT;

    //	float **ppTempIN = matrixTempIN.GetBuffer();

    //	float fError = 0.f;

    for (long nPoint = 0; nPoint < nPointCount; nPoint++)
    {
        const IPVM::Point_32f_C2& ptInput = vecRefPoints[nPoint];
        IPVM::Point_32f_C2 ptDest;

        distortionMap.Modify(ptInput, ptDest);

        const float fdx = ptDest.m_x - ptInput.m_x;
        const float fdy = ptDest.m_y - ptInput.m_y;

        const float fd = sqrt(fdx * fdx + fdy * fdy);

        vecfDistortion[nPoint] = fd;
    }

    return TRUE;
}

void CDlgCalculationResolution::Get1LineAngle(float x0, float y0, float x1, float y1, float& o_angle)
{
    // 2점을 가지고 각도를 구한다

    o_angle = (float)(atan2(x1 - x0, y1 - y0) / M_PI * 180);
}