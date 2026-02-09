// DlgDefectCount.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgDefectCount.h"
#include "afxdialogex.h"
#include "../Module/CustomControl/CustomStatic.h"
#include "..\\SharedModule\CommonShare\DefectDef.h"
#include "IBCIDlg.h"

// CDlgDefectCount 대화 상자

IMPLEMENT_DYNAMIC(CDlgDefectCount, CDialogEx)

CDlgDefectCount::CDlgDefectCount(long nRow, long nCol, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DEFECTCOUNT, pParent)
{
	//
	m_nRow = nRow;
	m_nCol = nCol;

	m_vecCustomStatic.resize(m_nRow * m_nCol*4);

	m_vecCustomTitle.resize(m_nRow * 4);

	for (long nX = 0; nX < m_nRow; nX++)
	{
		for (long nY = 0; nY < m_nCol; nY++)
		{
			m_vecCustomStatic[(nX * 4) + nY * m_nRow * 4] = new CCustomStatic;

			m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4] = new CCustomStatic;
			m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4] = new CCustomStatic;
			m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4] = new CCustomStatic;
		}
	}

	for (long nX = 0; nX < m_nRow; nX++)
    {
        m_vecCustomTitle[nX * 4] = new CCustomStatic;
        m_vecCustomTitle[nX * 4 + 1] = new CCustomStatic;
        m_vecCustomTitle[nX * 4 + 2] = new CCustomStatic;
        m_vecCustomTitle[nX * 4 + 3] = new CCustomStatic;
	}

	// lane 개수로 *4
    m_vecCustiomStaticMismatch.resize(4 * 4);

    for (long n = 0; n < m_vecCustiomStaticMismatch.size(); n++)
    {
        m_vecCustiomStaticMismatch[n] = new CCustomStatic;
    }


	m_bInit = FALSE;
}

CDlgDefectCount::~CDlgDefectCount()
{
	for (long nX = 0; nX < m_nRow * 4; nX++)
	{
		for (long nY = 0; nY < m_nCol; nY++)
		{
			delete m_vecCustomStatic[nX  + nY * m_nRow * 4];// = new CCustomStatic;
		}

	}
	
	for (long nX = 0; nX < m_nRow; nX++)
    {
        delete m_vecCustomTitle[nX * 4];
        delete m_vecCustomTitle[nX * 4 + 1];
        delete m_vecCustomTitle[nX * 4 + 2];
        delete m_vecCustomTitle[nX * 4 + 3];
	}

	for (long n = 0; n < m_vecCustiomStaticMismatch.size(); n++)
    {
        delete m_vecCustiomStaticMismatch[n];
    }
}

void CDlgDefectCount::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	for (long nY = 0; nY < m_nRow; nY++)
    {
        DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT_TITLE_1 + nY * 4, *m_vecCustomTitle[nY * 4]);
        DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT_TITLE_1 + nY * 4 + 1, *m_vecCustomTitle[nY * 4 + 1]);
        DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT_TITLE_1 + nY * 4 + 2, *m_vecCustomTitle[nY * 4 + 2]);
        DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT_TITLE_1 + nY * 4 + 3, *m_vecCustomTitle[nY * 4 + 3]);
		
	}

	for (long nX = 0; nX < m_nRow ; nX++)
	{
		for (long nY = 0; nY < m_nCol; nY++)
		{
			
			DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT1+nX*4+nY * m_nRow * 4, *m_vecCustomStatic[nX*4  + nY * m_nRow * 4]);
			DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT1 + nX * 4+1 + nY * m_nRow * 4, *m_vecCustomStatic[nX * 4+1 + nY * m_nRow * 4]);
			DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT1 + nX * 4+2 + nY * m_nRow * 4, *m_vecCustomStatic[nX * 4+2 + nY * m_nRow * 4]);
			DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT1 + nX * 4+3 + nY * m_nRow * 4, *m_vecCustomStatic[nX * 4+3 + nY * m_nRow * 4]);

		}

	}
    for (long n = 0; n < m_vecCustiomStaticMismatch.size(); n++)
    {
        DDX_Control(pDX, IDC_STATIC_DEFECTMISMATCH1 + n, *m_vecCustiomStaticMismatch[n]);
    }
}


BEGIN_MESSAGE_MAP(CDlgDefectCount, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgDefectCount 메시지 처리기


BOOL CDlgDefectCount::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bInit = TRUE;
	//SetUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgDefectCount::SetUI()
{
	if(m_bInit)
	{
        if (m_vecData.size() == 0)
            return;

		float fXtemp = m_rtDlgSize.Width()/ m_nRow/20;

		long nYTitle = 30;
		//m_rtDlgSize.Height()/ m_nCol;
		
#if INSPECTION_MACHINE_UC1
        long cy = (m_rtDlgSize.Height() - nYTitle) / m_nCol; //50;
#else
        long cy = (m_rtDlgSize.Height() - nYTitle - 80) / m_nCol; //50;
#endif // INSPECTION_MACHINE_UC1

		
		long cx2 = fXtemp * 3;
		long cx = m_rtDlgSize.Width() / m_nRow - (cx2*3);
		long xterm = cx + cx2 * 3;
        //long cy = (m_rtDlgSize.Height() - 80) / m_nCol; //50;
		long nCnt = 0;


		

		// Title
        for (long nY = 0; nY < m_nRow; nY++)
        {
            CString strTemp0;
            strTemp0 = _T("Defect Name");
            m_vecCustomTitle[nY * 4]->ModifyStyle(0x0FL, SS_OWNERDRAW);
            m_vecCustomTitle[nY * 4]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
            m_vecCustomTitle[nY * 4]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 12, 1, 5);
            m_vecCustomTitle[nY * 4]->SetWindowText(strTemp0);

			strTemp0 = _T("Top");
            m_vecCustomTitle[nY * 4 + 1]->ModifyStyle(0x0FL, SS_OWNERDRAW);
            m_vecCustomTitle[nY * 4 + 1]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
            m_vecCustomTitle[nY * 4 + 1]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 12, 1, 5);
            m_vecCustomTitle[nY * 4 + 1]->SetWindowText(strTemp0);

			strTemp0 = _T("Back");
            m_vecCustomTitle[nY * 4 + 2]->ModifyStyle(0x0FL, SS_OWNERDRAW);
            m_vecCustomTitle[nY * 4 + 2]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
            m_vecCustomTitle[nY * 4 + 2]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 12, 1, 5);
            m_vecCustomTitle[nY * 4 + 2]->SetWindowText(strTemp0);

			strTemp0 = _T("Total");
            m_vecCustomTitle[nY * 4 + 3]->ModifyStyle(0x0FL, SS_OWNERDRAW);
            m_vecCustomTitle[nY * 4 + 3]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
            m_vecCustomTitle[nY * 4 + 3]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 12, 1, 5);
            m_vecCustomTitle[nY * 4 + 3]->SetWindowText(strTemp0);

            m_vecCustomTitle[nY * 4]->MoveWindow(0 + nY * xterm, 0, cx - 1, nYTitle);
            m_vecCustomTitle[nY * 4 + 1]->MoveWindow(0 + nY * xterm + cx, 0, cx2 - 1, nYTitle);
            m_vecCustomTitle[nY * 4 + 2]->MoveWindow(0 + nY * xterm + cx + cx2, 0, cx2 - 1, nYTitle);
            m_vecCustomTitle[nY * 4 + 3]->MoveWindow(0 + nY * xterm + cx + cx2 + cx2, 0, cx2 - 1, nYTitle);

            m_vecCustomTitle[nY * 4]->ShowWindow(TRUE);
            m_vecCustomTitle[nY * 4 + 1]->ShowWindow(TRUE);
            m_vecCustomTitle[nY * 4 + 2]->ShowWindow(TRUE);
            m_vecCustomTitle[nY * 4 + 3]->ShowWindow(TRUE);
		}
		


		for (long nX = 0; nX < m_nRow; nX++)
		{
			for (long nY = 0; nY < m_nCol; nY++)
			{
                
                CString strTemp0; 
				CString strTemp1;

				if (m_vecData.size() != 0 && nCnt < m_vecData.size())
                {
                    strTemp0 = m_vecData[nCnt].m_strName;
                    
					if (strTemp0.GetLength() >= 15)
                    {
                        CString strLeft, strRight;
                        strTemp0.Find(_T("_"), 0);
                        strLeft = strTemp0.Left(strTemp0.Find(_T("_"), 0) + 1);
                        strRight = strTemp0.Right(strTemp0.GetLength() - ((strTemp0.Find(_T("_"), 0) + 1)));
                        strTemp0.Format(_T("%s\r\n%s"), strLeft, strRight);
                        //strTemp0.Replace(_T("_"), _T("\r\n"));
                    }

					strTemp1.Format(_T("0"));

                }
                else
                {
                    strTemp0 = _T("");
                    strTemp1.Format(_T(""));
				}
                
				
				// strTemp0.Format(_T("temp"));
				m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->ModifyStyle(0x0FL, SS_OWNERDRAW);
                m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->SetCenterMultiLine();
				m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
				m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 15, 1, 5);
				m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->SetWindowText(strTemp0);

				
				if (nCnt < m_vecData.size())
                {
                    strTemp1.Format(_T("%d"), m_vecData[nCnt].m_nTopNum);
                }    
                
				/*if (nCnt < m_vecData.size() && m_vecData[nCnt].m_nTopNum)
                    strTemp1.Format(_T("%d"), m_vecData[nCnt].m_nTopNum);*/

				m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->ModifyStyle(0x0FL, SS_OWNERDRAW);
				m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
                m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 12, 1, 5);
				m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->SetWindowText(strTemp1);


				if (nCnt < m_vecData.size())
                {
                    strTemp1.Format(_T("%d"), m_vecData[nCnt].m_nBottomNum);
                }    

				/*if (nCnt < m_vecData.size() && m_vecData[nCnt].m_nBottomNum)
                    strTemp1.Format(_T("%d"), m_vecData[nCnt].m_nBottomNum);*/

				m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->ModifyStyle(0x0FL, SS_OWNERDRAW);
				m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
                m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 12, 1, 5);
				m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->SetWindowText(strTemp1);



				if (nCnt < m_vecData.size())
                {
                    strTemp1.Format(_T("%d"), m_vecData[nCnt].m_nSumNum);
                }

                /*if (nCnt < m_vecData.size() && m_vecData[nCnt].m_nSumNum)
                    strTemp1.Format(_T("%d"), m_vecData[nCnt].m_nSumNum);*/

				m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->ModifyStyle(0x0FL, SS_OWNERDRAW);
				m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
                m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 12, 1, 5);
				m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->SetWindowText(strTemp1);

				m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm,nYTitle + cy * nY, cx - 1, cy - 1);
				m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm + cx, nYTitle + cy * nY, cx2 - 1, cy - 1);
				m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm + cx + cx2, nYTitle + cy * nY, cx2 - 1, cy - 1);
				m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm + cx + cx2 + cx2, nYTitle + cy * nY, cx2 - 1, cy - 1);


				m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->ShowWindow(TRUE);
				m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->ShowWindow(TRUE);
				m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->ShowWindow(TRUE);
				m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->ShowWindow(TRUE);

				if (m_vecData.size() != 0 && nCnt < m_vecData.size())
                {
					WarningSpecCountBackColor(m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4], m_vecData[nCnt].m_nTopNum);
					WarningSpecCountBackColor(m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4], m_vecData[nCnt].m_nBottomNum);
					WarningSpecCountBackColor(m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4], m_vecData[nCnt].m_nSumNum);
				}

                nCnt++;
			}

		}
#if INSPECTION_MACHINE_UC1

#else
        nCnt = 0;
        for (long n = 0; n < m_vecCustiomStaticMismatch.size(); n += 4)
        {
            CString strTemp0 = _T("");
            CString strTopTemp = _T("0.0");
            CString strBotTemp = _T("0.0");
            CString strMismatchTemp = _T("0.0");

            if (nCnt < m_vecMismatchData.size())
            {
                strTemp0.Format(_T("%s"), m_vecMismatchData[nCnt].m_strName);

                CString strLeft, strRight;
                strLeft = strTemp0.Left(strTemp0.Find(_T(" "), 0));
                strRight = strTemp0.Right(strTemp0.GetLength() - ((strTemp0.Find(_T(" "), 0) + 1)));
                strTemp0.Format(_T("%s\r\n%s"), strLeft, strRight);

                strTopTemp.Format(_T("%.2f"), m_vecMismatchData[nCnt].m_fTopValue);
                strBotTemp.Format(_T("%.2f"), m_vecMismatchData[nCnt].m_fBottomValue);
                strMismatchTemp.Format(_T("%.2f"), m_vecMismatchData[nCnt].m_fMismatchValue);
            }

            if (strTemp0 == _T(""))
            {
                strTopTemp = _T("");
                strBotTemp = _T("");
                strMismatchTemp = _T("");
            }

            m_vecCustiomStaticMismatch[n]->ModifyStyle(0x0FL, SS_OWNERDRAW);
            m_vecCustiomStaticMismatch[n]->SetCenterMultiLine();
            m_vecCustiomStaticMismatch[n]->SetBackGradation(
                RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
            m_vecCustiomStaticMismatch[n]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 12, 1, 5);
            m_vecCustiomStaticMismatch[n]->SetWindowText(strTemp0);

            m_vecCustiomStaticMismatch[n + 1]->ModifyStyle(0x0FL, SS_OWNERDRAW);
            m_vecCustiomStaticMismatch[n + 1]->SetBackGradation(
                RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
            m_vecCustiomStaticMismatch[n + 1]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 12, 1, 5);
            m_vecCustiomStaticMismatch[n + 1]->SetWindowText(strTopTemp);

            m_vecCustiomStaticMismatch[n + 2]->ModifyStyle(0x0FL, SS_OWNERDRAW);
            m_vecCustiomStaticMismatch[n + 2]->SetBackGradation(
                RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
            m_vecCustiomStaticMismatch[n + 2]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 12, 1, 5);
            m_vecCustiomStaticMismatch[n + 2]->SetWindowText(strBotTemp);

            m_vecCustiomStaticMismatch[n + 3]->ModifyStyle(0x0FL, SS_OWNERDRAW);
            m_vecCustiomStaticMismatch[n + 3]->SetBackGradation(
                RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
            m_vecCustiomStaticMismatch[n + 3]->SetFont(_T("나눔고딕"), RGB(0, 0, 0), 12, 1, 5);
            m_vecCustiomStaticMismatch[n + 3]->SetWindowText(strMismatchTemp);

            float fXtemp = m_rtDlgSize.Width() / m_nRow / 20;
            //m_rtDlgSize.Height()/ m_nCol;
            long fYtemp = m_rtDlgSize.Height() / m_nCol;
            fYtemp = 40;
            long cy = m_rtDlgSize.Height() / m_nCol; //50;
            long cx2 = fXtemp * 3;
            long cx = m_rtDlgSize.Width() / m_nRow - (cx2 * 3);
            long xterm = cx + cx2 * 3;

            long xtemp = m_rtDlgSize.Width() / 8;

            if (n == 0)
            {
                m_vecCustiomStaticMismatch[n]->MoveWindow(
                    0 + xtemp * 0, m_rtDlgSize.Height() - fYtemp * 2, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 1]->MoveWindow(
                    0 + xtemp * 1, m_rtDlgSize.Height() - fYtemp * 2, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 2]->MoveWindow(
                    0 + xtemp * 2, m_rtDlgSize.Height() - fYtemp * 2, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 3]->MoveWindow(
                    0 + xtemp * 3, m_rtDlgSize.Height() - fYtemp * 2, xtemp - 1, fYtemp - 1);
            }
            else if (n == 4)
            {
                m_vecCustiomStaticMismatch[n]->MoveWindow(
                    m_rtDlgSize.Width() / 2 + xtemp * 0, m_rtDlgSize.Height() - fYtemp * 2, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 1]->MoveWindow(
                    m_rtDlgSize.Width() / 2 + xtemp * 1, m_rtDlgSize.Height() - fYtemp * 2, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 2]->MoveWindow(
                    m_rtDlgSize.Width() / 2 + xtemp * 2, m_rtDlgSize.Height() - fYtemp * 2, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 3]->MoveWindow(
                    m_rtDlgSize.Width() / 2 + xtemp * 3, m_rtDlgSize.Height() - fYtemp * 2, xtemp - 1, fYtemp - 1);
            }
            else if (n == 8)
            {
                m_vecCustiomStaticMismatch[n]->MoveWindow(
                    0 + xtemp * 0, m_rtDlgSize.Height() - fYtemp * 1, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 1]->MoveWindow(
                    0 + xtemp * 1, m_rtDlgSize.Height() - fYtemp, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 2]->MoveWindow(
                    0 + xtemp * 2, m_rtDlgSize.Height() - fYtemp, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 3]->MoveWindow(
                    0 + xtemp * 3, m_rtDlgSize.Height() - fYtemp, xtemp - 1, fYtemp - 1);
            }
            else if (n == 12)
            {
                m_vecCustiomStaticMismatch[n]->MoveWindow(
                    m_rtDlgSize.Width() / 2 + xtemp * 0, m_rtDlgSize.Height() - fYtemp, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 1]->MoveWindow(
                    m_rtDlgSize.Width() / 2 + xtemp * 1, m_rtDlgSize.Height() - fYtemp, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 2]->MoveWindow(
                    m_rtDlgSize.Width() / 2 + xtemp * 2, m_rtDlgSize.Height() - fYtemp, xtemp - 1, fYtemp - 1);
                m_vecCustiomStaticMismatch[n + 3]->MoveWindow(
                    m_rtDlgSize.Width() / 2 + xtemp * 3, m_rtDlgSize.Height() - fYtemp, xtemp - 1, fYtemp - 1);
            }

            m_vecCustiomStaticMismatch[n]->ShowWindow(TRUE);
            m_vecCustiomStaticMismatch[n + 1]->ShowWindow(TRUE);
            m_vecCustiomStaticMismatch[n + 2]->ShowWindow(TRUE);
            m_vecCustiomStaticMismatch[n + 3]->ShowWindow(TRUE);

            nCnt++;
        }
#endif // INSPECTION_MACHINE_UC1
		UpdateData();
	}
}
void CDlgDefectCount::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	GetClientRect(&m_rtDlgSize);

	SetUI();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgDefectCount::DefectDraw()
{
    //SendMessage(WM_SETREDRAW, FALSE, 0);
    int n = 0;
    for (long nX = 0; nX < m_nRow; nX++)
    {
        for (long nY = 0; nY < m_nCol; nY++)
        {
            if (n < m_vecData.size())
            {
                CString strTemp = m_vecData[n].m_strName;
                if (strTemp.GetLength() >= 15)
                {
                    CString strLeft, strRight;
                    strTemp.Find(_T("_"), 0);
                    strLeft = strTemp.Left(strTemp.Find(_T("_"), 0) + 1);
                    strRight = strTemp.Right(strTemp.GetLength() - ((strTemp.Find(_T("_"), 0) + 1)));
                    strTemp.Format(_T("%s\r\n%s"), strLeft, strRight);
                }

                m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->SetWindowText(strTemp);

                CString str;
                str.Format(_T("%d"), m_vecData[n].m_nTopNum);
                m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->SetWindowText(str);
                WarningSpecCountBackColor(m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4], m_vecData[n].m_nTopNum);
                str.Format(_T("%d"), m_vecData[n].m_nBottomNum);
                m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->SetWindowText(str);
                WarningSpecCountBackColor(m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4], m_vecData[n].m_nBottomNum);
                str.Format(_T("%d"), m_vecData[n].m_nSumNum);
                m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->SetWindowText(str);
                WarningSpecCountBackColor(m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4], m_vecData[n].m_nSumNum);

                n++;
			}
		}
	}
    //SendMessage(WM_SETREDRAW, TRUE, 0);
    //RedrawWindow();

	//for (long n = 0; n < m_vecData.size(); n++)
	//{
	//	m_vecData[n].m_strName;
	//	m_vecData[n].m_nTopNum;
	//	m_vecData[n].m_nBottomNum;
	//	m_vecData[n].m_nSumNum;
 //       long nSizeX = n / m_nRow;
	//	long nSizeY = n % m_nCol;
 //       
	//	//m_nCol
	//	//m_nRow

	//	//
 //       m_vecCustomStatic[nSizeX * 4 + nSizeY * m_nRow * 4]->SetWindowText(m_vecData[n].m_strName);
 //       CString str;
 //       str.Format(_T("%d"), m_vecData[n].m_nTopNum);
 //       m_vecCustomStatic[(nSizeX * 4 + 1) + nSizeY * m_nRow * 4]->SetWindowText(str);
 //       str.Format(_T("%d"), m_vecData[n].m_nBottomNum);
 //       m_vecCustomStatic[(nSizeX * 4 + 2) + nSizeY * m_nRow * 4]->SetWindowText(str);
 //       str.Format(_T("%d"), m_vecData[n].m_nSumNum);
 //       m_vecCustomStatic[(nSizeX * 4 + 3) + nSizeY * m_nRow * 4]->SetWindowText(str);

	//	/*m_vecCustomStatic[nSizeX * 4 + nSizeY * m_nRow * 4]->SetWindowText(m_vecData[n].m_strName);
	//	CString str;
	//	str.Format(_T("%d"), m_vecData[n].m_nTopNum);
	//	m_vecCustomStatic[(nSizeX * 4 + 1) + nSizeY * m_nRow * 4]->SetWindowText(str);
	//	str.Format(_T("%d"), m_vecData[n].m_nBottomNum);
	//	m_vecCustomStatic[(nSizeX * 4 + 2) + nSizeY * m_nRow * 4]->SetWindowText(str);
	//	str.Format(_T("%d"), m_vecData[n].m_nSumNum);
	//	m_vecCustomStatic[(nSizeX * 4 + 3) + nSizeY * m_nRow * 4]->SetWindowText(str);*/
	//}
	//UpdateData();
//	m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->SetWindowText(strTemp0);
}

void CDlgDefectCount::SetZero()
{
	for (long n = 0; n < m_vecData.size(); n++)
	{
		m_vecData[n].m_nSumNum = 0;
		m_vecData[n].m_nTopNum = 0;
		m_vecData[n].m_nBottomNum = 0;
        m_vecData[n].m_fTopValue = 0.f;
        m_vecData[n].m_fBottomValue = 0.f;
        m_vecData[n].m_fMismatchValue = 0.f;
	}
	SetUI();
}

void CDlgDefectCount::WarningSpecCountBackColor(CCustomStatic* customStatic, int nValue)
{
    if (g_systemParameter.nWarningCnt < nValue && g_systemParameter.nSpecCnt > nValue)
    {
        customStatic->SetBackGradation(RGB(255, 127, 0), RGB(255, 127, 0), 1);
        customStatic->Invalidate(false);
    }
    else if (g_systemParameter.nSpecCnt < nValue && g_systemParameter.nSpecCnt!=0)
    {
        customStatic->SetBackGradation(RGB(255, 0, 0), RGB(255, 0, 0), 1);
        customStatic->Invalidate(false);
    }
}

void CDlgDefectCount::DefectMismatchDraw()
{
    int n = 0;
    for (long nTemp = 0; nTemp < m_vecCustiomStaticMismatch.size(); nTemp += 4)
    {
        if (n < m_vecMismatchData.size())
        {
            //m_vecCustiomStaticMismatch[nTemp]->SetWindowText(m_vecMismatchData[n].m_strName);
            CString str;
            str.Format(_T("%.2f"), m_vecMismatchData[n].m_fTopValue);
            m_vecCustiomStaticMismatch[nTemp + 1]->SetWindowText(str);
            str.Format(_T("%.2f"), m_vecMismatchData[n].m_fBottomValue);
            m_vecCustiomStaticMismatch[nTemp + 2]->SetWindowText(str);
            str.Format(_T("%.2f"), m_vecMismatchData[n].m_fMismatchValue);
            m_vecCustiomStaticMismatch[nTemp + 3]->SetWindowText(str);
        }
        n++;
    }

}