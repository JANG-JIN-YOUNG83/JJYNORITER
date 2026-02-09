// CDlgGraphWindow.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgGraphWindow.h"
#include "afxdialogex.h"
#include "GraphOption.h"

#include "IBCIDlg.h"
#include "GraphCustomizeDefault.h"
#include "GraphCustomizeMismatch.h"
#include "../Inspection/Accessor.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define COLOR_BLUE					RGB(46, 117, 182)
#define COLOR_GRAY2					RGB(75, 75, 75)
#define COLOR_BLACK					RGB(0, 0, 0)

// CDlgGraphWindow 대화 상자
IMPLEMENT_DYNAMIC(CDlgGraphWindow, CDialogEx)

CDlgGraphWindow::CDlgGraphWindow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_GRAPH_WINDOW, pParent)
{
	m_pParent = (CIBCIDlg*)pParent;
	m_pvecDlgGraphFrame.reserve(25);
}

CDlgGraphWindow::~CDlgGraphWindow()
{
	for (short n = 0; n < m_pvecDlgGraphFrame.size(); n++)
	{
		if (NULL != m_pvecDlgGraphFrame[n])
			delete m_pvecDlgGraphFrame[n];
	}

	m_font.DeleteObject();
	m_font_2.DeleteObject();

	m_BrushBlock.DeleteObject();
	m_BrushBlue.DeleteObject();
}

void CDlgGraphWindow::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SPEC_X, m_editSpecX);
    DDX_Control(pDX, IDC_EDIT_SPEC_Y, m_editSpecY);
    DDX_Control(pDX, IDC_BUTTON_SETTING_SPEC, m_btnSettingSpec);
}


BEGIN_MESSAGE_MAP(CDlgGraphWindow, CDialogEx)
	ON_WM_SIZE()
ON_BN_CLICKED(IDC_BUTTON_SETTING_SPEC, &CDlgGraphWindow::OnBnClickedButtonSettingSpec)
END_MESSAGE_MAP()


// CDlgGraphWindow 메시지 처리기


BOOL CDlgGraphWindow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_BrushBlue.CreateSolidBrush(COLOR_GRAY2);
	m_BrushBlock.CreateSolidBrush(COLOR_BLACK);

	CRect ctlPos;
	this->GetClientRect(ctlPos);

	//CWnd* pCtl;
	////Title Text
	//pCtl = GetDlgItem(IDC_STATIC_TITLE);
	//if (pCtl != NULL)
	//{
	//	pCtl->MoveWindow(ctlPos.left, ctlPos.top - 1, ctlPos.Width() * 0.12, ctlPos.Height() * 0.0423);
	//}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDlgGraphWindow::PreTranslateMessage(MSG* pMsg)
{
	if (VK_RETURN || VK_ESCAPE == pMsg->wParam)
	{
		return FALSE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgGraphWindow::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	GetClientRect(&m_rtDlgSize);
	if (m_pvecDlgGraphFrame.size() != 0)
	{
		auto nCount = m_pvecDlgGraphFrame.size();
		if (2 == nCount)
		{
			ArrangeWindow_Graph(1, 2);
		}
		else if (4 == nCount)
		{
			ArrangeWindow_Graph(4, 1);
		}
		else if (5 == nCount || 6 == nCount)
		{
			ArrangeWindow_Graph(2, 3);
		}
		else if (7 == nCount || 8 == nCount)
		{
			ArrangeWindow_Graph(2, 4);
		}
		else if (10 == nCount)
		{
			ArrangeWindow_Graph(2, 5);
		}
		else if (11 == nCount || 12 == nCount)
		{
			ArrangeWindow_Graph(3, 4);
		}
		else
		{
			long nZ(1);
			long nPOW(0);
			while (1)
			{
				nPOW = pow(nZ, 2);
				if (nPOW >= nCount)
				{
					break;
				}
				nZ++;
			}
			ArrangeWindow_Graph(nZ, nZ);
		}
        
		m_btnSettingSpec.MoveWindow(m_rtDlgSize.right - 100, m_rtDlgSize.bottom - 20, 100, 20, TRUE);
        m_editSpecY.MoveWindow(m_rtDlgSize.right - 205, m_rtDlgSize.bottom - 20, 100, 20, TRUE);
        m_editSpecX.MoveWindow(m_rtDlgSize.right - 305, m_rtDlgSize.bottom - 20, 100, 20, TRUE);
	}

	
}

void CDlgGraphWindow::ArrangeWindow_Graph(long nHorizentalCnt, long nVerticalCount)
{
	//정렬
	CRect rtROI;
	GetWindowRect(&rtROI);
	ScreenToClient(rtROI);
	
	long FrameTop = rtROI.top * 0.01;
	long GraphTop = rtROI.top * 0.02;
	long nWidthStep;
	if (nHorizentalCnt == 1)
	{
		nWidthStep = rtROI.Width();
	}
	else
	{
		nWidthStep = rtROI.Width() / nHorizentalCnt;
	}
	
	long nHeightStep;
	if (nVerticalCount == 1)
	{
		nHeightStep = rtROI.Height() * 0.9;
	}
	else
	{
		nHeightStep = (rtROI.Height() / (nVerticalCount)) * 0.9;
	}
	
	long nPos(0);

	for (long y = 0; y < nVerticalCount; y++)
	{
		for (long x = 0; x < nHorizentalCnt; x++)
		{
			if (m_pvecDlgGraphFrame.size() <= nPos)
				continue;

			//Set Position of Frame and Graph
			int nStartX = 2 + (nWidthStep * x);
			int nStartY = FrameTop + (nHeightStep * y);

			::MoveWindow(m_pvecDlgGraphFrame[nPos]->GetSafeHwnd(), nStartX, nStartY, nWidthStep, nHeightStep, TRUE);
			::ShowWindow(m_pvecDlgGraphFrame[nPos]->GetSafeHwnd(), SW_SHOW);

			m_pvecDlgGraphFrame[nPos]->FontChange(BOX, (long)(rtROI.Height() * 0.30));
			m_pvecDlgGraphFrame[nPos]->FontChange(TEXT, (long)(rtROI.Height() * 0.30));
			m_pvecDlgGraphFrame[nPos]->FontChange(SPEC, (long)(rtROI.Height() * 0.30));
			nPos++;
		}
	}
}

BOOL CDlgGraphWindow::SetGraphFrame(const std::vector<DefectType>& vecInspTypes, BOOL bShow)
{
	//--------------------------------------------------------------------------------------------
	// 검사항목 리스트를 이용하여 보여줄 Graph 리스트를 만든다
	//--------------------------------------------------------------------------------------------
	struct GraphInfo
	{
		std::vector<DefectType> m_inspTypes;
		CString m_title;
		CString m_key;
		bool m_isOnlyMaxDiffValueShowed;
	};

	std::vector<GraphInfo> graphInsps;

	for (auto& inspType : vecInspTypes)
	{
		bool isOnlyMaxDiffValueShowed = false;
		CString graphKey = GetDefectName(inspType);
		CString title;

		auto graphGroupType = GraphOption::GetGroupGraphType(inspType);
		if (graphGroupType == GroupGraphType::None)
		{
			//title = Language::GetTranslatedText(inspType);
		}
		else
		{
			isOnlyMaxDiffValueShowed = GraphOption::IsOnlyMaxDiffValueShowed(graphGroupType);
			graphKey = GraphOption::GetGroupGraphName(graphGroupType);
			//title = Language::GetTranslatedText(graphGroupType);
		}
		title = graphKey;

		GraphInfo* graphInspPtr = nullptr;

		for (auto& graphInsp : graphInsps)
		{
			if (graphInsp.m_title == title)
			{
				graphInspPtr = &graphInsp;
				break;
			}
		}

		if (graphInspPtr == nullptr)
		{
			// 새로 추가
			long index = (long)graphInsps.size();
			graphInsps.resize(index + 1);
			graphInspPtr = &graphInsps[index];
		}

		graphInspPtr->m_inspTypes.push_back(inspType);
		graphInspPtr->m_title = title;
		graphInspPtr->m_key = graphKey;
		graphInspPtr->m_isOnlyMaxDiffValueShowed = isOnlyMaxDiffValueShowed;
	}

	// 없어진 Graph 삭제
	for (long graphIndex = 0; graphIndex < (long)m_pvecDlgGraphFrame.size(); graphIndex++)
	{
		CString graphKey = m_pvecDlgGraphFrame[graphIndex]->GetGraphKey();
		BOOL bExist = false;
		for (int i = 0; i < (int)graphInsps.size(); i++)
		{
			if (graphKey == graphInsps[i].m_key)
			{
				bExist = true;
				break;
			}
		}

		if (!bExist)
		{
			delete m_pvecDlgGraphFrame[graphIndex];
			m_pvecDlgGraphFrame.erase(m_pvecDlgGraphFrame.begin() + graphIndex);
			graphIndex--;
		}
	}

	// 일단 Graph List를 아직 안만들어진 것까지 nullptr 채워서 생성
	m_pvecDlgGraphFrame.resize(graphInsps.size(), nullptr);

	for (int realIndex = 0; realIndex < (int)graphInsps.size(); realIndex++)
	{
		for (long graphIndex = 0; graphIndex < (long)m_pvecDlgGraphFrame.size(); graphIndex++)
		{
			if (m_pvecDlgGraphFrame[graphIndex] == nullptr) continue;
			CString graphKey = m_pvecDlgGraphFrame[graphIndex]->GetGraphKey();

			if (graphKey == graphInsps[realIndex].m_key)
			{
				// 기존 Graph를 자기 Index로 이동
				std::swap(m_pvecDlgGraphFrame[realIndex], m_pvecDlgGraphFrame[graphIndex]);
				break;
			}
		}
	}

	//신규항목 찾기
	for (long graphIndex = 0; graphIndex < (long)m_pvecDlgGraphFrame.size(); graphIndex++)
	{
		if (m_pvecDlgGraphFrame[graphIndex] == nullptr)
		{
			CDlgGraphFrame* PDlgGF = new CDlgGraphFrame(this);
			PDlgGF->Create(CDlgGraphFrame::IDD, this);
			PDlgGF->SetTextBox(TRUE, TRUE, FALSE, TRUE, TRUE);   //Title, min, max 표시

			m_pvecDlgGraphFrame[graphIndex] = PDlgGF;
		}

		m_pvecDlgGraphFrame[graphIndex]->Initialize(
			graphInsps[graphIndex].m_inspTypes,
			graphInsps[graphIndex].m_key,
			graphInsps[graphIndex].m_title,
			graphInsps[graphIndex].m_isOnlyMaxDiffValueShowed);
	}

	//다끝나면 map은 새로 만들어줘야 한다.
	m_mapDlgGraphFrame.clear();
	for (int index = 0; index < (int)m_pvecDlgGraphFrame.size(); index++)
	{
		for (auto& inspType : graphInsps[index].m_inspTypes)
		{
			m_mapDlgGraphFrame[inspType] = m_pvecDlgGraphFrame[index];
		}
	}

	auto nCount = (long)graphInsps.size();

	if (2 == nCount)
	{
		ArrangeWindow_Graph(1, 2);
	}
	else if (4 == nCount)
	{
		ArrangeWindow_Graph(4, 1);
	}
	else if (5 == nCount || 6 == nCount)
	{
		ArrangeWindow_Graph(2, 3);
	}
	else if (7 == nCount || 8 == nCount)
	{
		ArrangeWindow_Graph(2, 4);
	}
	else if (10 == nCount)
	{
		ArrangeWindow_Graph(2, 5);
	}
	else if (11 == nCount || 12 == nCount)
	{
		ArrangeWindow_Graph(3, 4);
	}
	else
	{
		long nZ(1);
		long nPOW(0);
		while (1)
		{
			nPOW = pow(nZ, 2);
			if (nPOW >= nCount)
			{
				break;
			}
			nZ++;
		}
		ArrangeWindow_Graph(nZ, nZ);
	}

	//SetShowWindow(bShow, ViewType::GRAPH_VIEW);
	return TRUE;
}

long CDlgGraphWindow::GetGraphFrameCount() const
{
	return (long)m_pvecDlgGraphFrame.size();
}

CDlgGraphFrame* CDlgGraphWindow::GetGraphFrame(long inspIndex)
{
	return m_pvecDlgGraphFrame[inspIndex];
}

CDlgGraphFrame* CDlgGraphWindow::FindGraphFrame(DefectType inspType)
{
	auto itGraphFrame = m_mapDlgGraphFrame.find(inspType);

	if (itGraphFrame == m_mapDlgGraphFrame.end())
	{
		return nullptr;
	}

	return itGraphFrame->second;
}

void CDlgGraphWindow::DataReset()
{
	for (auto* graph : m_pvecDlgGraphFrame)
	{
		graph->DataSetZero();
	}
}

void CDlgGraphWindow::OnBnClickedButtonSettingSpec()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    long nCount = GetGraphFrameCount();
	long nSpecY = GetDlgItemInt(IDC_EDIT_SPEC_Y);
    long nSpecX = GetDlgItemInt(IDC_EDIT_SPEC_X);

	CInspectionCell* unitUp = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    CInspectionMeasure* CellMeasure = (CInspectionMeasure*)unitUp->GetProcessingItem(ProcItemCategory::measure);
    
	long nFrameCnt = GetGraphFrameCount();
	for (long n = 0; n < nFrameCnt; n++)
    {
        GetGraphFrame(n)->DataSetZero();
        if (nSpecX > 0)
			GetGraphFrame(n)->m_pDlgGraph->SetDataGroupCount(nSpecX);
	}

	for (long meas = 0; meas < CellMeasure->m_para.m_nCountMeasureItem; meas++)
    {
        float fLCL, fUCL, fLSL, fUSL;
		
		switch (CellMeasure->m_para.m_vecMeasureType[meas])
        {
            case 1:
                fLCL = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];
                fUCL = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                fLSL = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];
                fUSL = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];
				//SetSpec(float fOffset, float fSpecNominal, float fSapeMin, float fSpecMax, float fLCL, float fUCL, float fSpecMinAi = -9999, float fSpecMaxAi = -9999, float fSpecMinNg = -9999, float fSpecMaxNg = -9999);
				GetGraphFrame(1)->SetSpec(20.f, 0.f, -nSpecY, nSpecY, fLCL, fUCL, fLSL, fUSL);
                break;

            case 2:
                if (g_systemParameter.nLaneCount == 2)
                {
                    fLCL = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];
                    fUCL = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                    fLSL = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];
                    fUSL = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];

					GetGraphFrame(2)->SetSpec(10.f, 0.f, -nSpecY, nSpecY, fLCL, fUCL, fLSL, fUSL);
				}
                break;

            case 3:
                fLCL = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];
                fUCL = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                fLSL = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];
                fUSL = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];

                GetGraphFrame(0)->SetSpec(10.f, 0.f, -nSpecY, nSpecY, fLCL, fUCL, fLSL, fUSL);
                break;

            case 7:
                fLCL = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];
                fUCL = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                fLSL = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];
                fUSL = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];

				if (g_systemParameter.nLaneCount == 2)
                {
                    GetGraphFrame(3)->SetSpec(10.f, 0.f, -nSpecY, nSpecY, fLCL, fUCL, fLSL, fUSL);
				}
                else if (g_systemParameter.nLaneCount == 1)
                {
                    GetGraphFrame(2)->SetSpec(10.f, 0.f, -nSpecY, nSpecY, fLCL, fUCL, fLSL, fUSL);
				}
                break;
		}
	}



}
