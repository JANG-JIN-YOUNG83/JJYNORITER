// CDlgClassificationST.cpp: 구현 파일
//

#include "stdafx.h"
#include "Inspection.h"
#include "CDlgClassificationST.h"
#include "afxdialogex.h"

#include "basedef.h"
#include "CDlgEditGridText.h"

#include "Process_XTPFlowGraph.h"

using namespace classification;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// 이건 안써도 돼는거...
enum enColTitle
{
	ITEM = 0,
	SIZE_X,
	SIZE_X_CO,
	SIZE_Y,
	SIZE_Y_CO,
	ORAND,

	INSPECTION_IMAGE_TYPE,
	DEFECT_BRIGHTNESS,
	BLOB_THRESHOLD,
	BLOB_THRESHOLD_CO,
	CATEGORY,
	THICKNESS,
	THICKNESS_CO,
	LENGTH,
	LENGTH_CO,
	MIN,
	MIN_CO,
	AVG,
	AVG_CO,
	MAX,
	MAX_CO,
	AREA,
	AREA_CO,
	RATIO_YX,
	RATIO_YX_CO,
	RATIO_XY,
	RATIO_XY_CO,
	COMPACT,
	COMPACT_CO,
	ANGLE,
	ANGLE_CO,
	STD,
	STD_CO,
	INSPECTION_AREA,
};

// CDlgClassificationST 대화 상자

void CDlgClassificationST::DrawFlow(std::vector<enLogicResult> answers)
{
	BOOL bReset(TRUE);
	StartFlowChart(bReset);

	CXTPFlowGraphPage* pPage = m_wndControl.GetActivePage();
	if (!pPage)
		return;

	std::vector<FGNode>& nodes = m_nodes;
	long nSizeNode = nodes.size();
	CXTPFlowGraphConnection* pConnection;
	long from = 0;
	long to = 0;
	while (true)
	{
		if (from >= nSizeNode - 1)
			break;

		// set from
		//FGNode nodeFrom& = nodes[from];

		// set to
		int to = nodes[from].m_sJunctionInfo.toNodeNoJunction;
		FGDir dirFrom = nodes[from].m_sJunctionInfo.dirs[0];
		FGDir dirTo = nodes[from].m_sJunctionInfo.dirs[1];
		if (nodes[from].m_sJunctionInfo.toNodeNoJunction == 0)
		{
			if (answers.size() <= 0)
			{
				AfxMessageBox(_T("error occur, answers size is too shrot. {846E6EED-9A6F-4FE8-A11C-30E29EE9ED29}"));
				return;
			}
			switch (answers[0])
			{
			case enLogicResult::YES:
				to = nodes[from].m_sJunctionInfo.toNodeYes;
				dirFrom = nodes[from].m_sJunctionInfo.dirs[2];
				dirTo = nodes[from].m_sJunctionInfo.dirs[3];
				break;
			case enLogicResult::NO:
				to = nodes[from].m_sJunctionInfo.toNodeNo;
				dirFrom = nodes[from].m_sJunctionInfo.dirs[4];
				dirTo = nodes[from].m_sJunctionInfo.dirs[5];
				break;
			case enLogicResult::NONE:
				AfxMessageBox(_T("? {846E6EED-9A6F-4FE8-A11C-30E29EE9ED29}"));
				return;
				break;
			default:;
			}
			answers.erase(answers.begin());
		}

		FGColor color = FGColor::RED;
		Process_XTPFlowGraph::ConnectNode(pPage, nodes[from], nodes[to], color, dirFrom, dirTo, 0);
		from = to;
	}
	//for (long nNode = 0; nNode < nSizeNode - 1; nNode++)
	//{
    //}
}

void CDlgClassificationST::StartFlowChart(BOOL bResetPage)
{
	CXTPFlowGraphPage* pPage;
	if (bResetPage)
	{
		pPage = m_wndControl.GetActivePage();
		if (pPage)
			pPage->Remove();
		pPage = m_wndControl.GetPages()->AddPage(new CXTPFlowGraphPage());
		m_wndControl.SetActivePage(pPage);
		m_nodes.clear();
	}
	pPage = m_wndControl.GetActivePage();
	pPage->GetConnections()->RemoveAll();
	pPage->GetNodes()->RemoveAll();

	// create control & flow Chart
	m_wndControl.GetPaintManager()->m_clrConnection = RGB(80, 80, 80);
	m_wndControl.GetPaintManager()->m_bShowGrid = false;
	m_wndControl.GetPaintManager()->m_clrBackground = RGB(240, 240, 240);
	//m_wndControl.SetZoomRange(1, 1);

	long nSizeNode = 16;
	std::vector<CString> captions;
	captions.resize(nSizeNode);
	captions[0] = _T("시작");
	captions[1] = _T("InspImage Bright");
	captions[2] = _T("Category Tabfolding");
	captions[3] = _T("Category Wrinkle");
	captions[4] = _T("Category ng tag");
	captions[5] = _T("70>60");
	captions[6] = _T("Rut io L/W 5<4");
	captions[7] = _T("Section Th 40>250");
	captions[8] = _T("X 5<3.3\nY 5>2.2");
	captions[9] = _T("X 10<9\nY 10<20");
	captions[10] = _T("Length 28(s)<41(A)");
	captions[11] = _T("X 5<4\nY3>7");
	captions[12] = _T("");
	captions[13] = _T("");
	captions[14] = _T("");
	captions[15] = _T("");

	std::vector<CPoint> coords;
	coords.resize(nSizeNode);
	coords[0] = CPoint(1, 4);
	coords[1] = CPoint(2, 1);
	coords[2] = CPoint(2, 2);
	coords[3] = CPoint(2, 3);
	coords[4] = CPoint(2, 4);
	coords[5] = CPoint(3, 1);
	coords[6] = CPoint(4, 1);
	coords[7] = CPoint(5, 1);
	coords[8] = CPoint(6, 1);
	coords[9] = CPoint(6, 2);
	coords[10] = CPoint(6, 3);
	coords[11] = CPoint(6, 4);
	coords[12] = CPoint(7, 1);
	coords[13] = CPoint(7, 2);
	coords[14] = CPoint(7, 3);
	coords[15] = CPoint(7, 4);

	std::vector<FGSymbol> symbols;
	symbols.resize(nSizeNode);
	//InputOutput;	//Decision;	//Process_Simple;	//Process_Complex;
	symbols[0] = FGSymbol::StartEnd;
	symbols[1] = FGSymbol::Decision;
	symbols[2] = FGSymbol::Decision;
	symbols[3] = FGSymbol::Decision;
	symbols[4] = FGSymbol::Decision;
	symbols[5] = FGSymbol::Decision;
	symbols[6] = FGSymbol::Decision;
	symbols[7] = FGSymbol::Decision;
	symbols[8] = FGSymbol::Decision;
	symbols[9] = FGSymbol::Decision;
	symbols[10] = FGSymbol::Decision;
	symbols[11] = FGSymbol::Decision;
	symbols[12] = FGSymbol::StartEnd;
	symbols[13] = FGSymbol::StartEnd;
	symbols[14] = FGSymbol::StartEnd;
	symbols[15] = FGSymbol::StartEnd;

	std::vector<Process_XTPFlowGraph::SJunctionInfo> junctionInfos;
	junctionInfos.resize(nSizeNode);
	junctionInfos[0] = { 4, 0, 0  , FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[1] = { 5, 0, 0  , FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[2] = { 0, 9, 1  , FGDir::LEFT, FGDir::LEFT, /**/FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::RIGHT, };
	junctionInfos[3] = { 0, 10, 2 , FGDir::LEFT, FGDir::LEFT, /**/FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::RIGHT, };
	junctionInfos[4] = { 0, 11, 3 , FGDir::LEFT, FGDir::LEFT, /**/FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::RIGHT, };
	junctionInfos[5] = { 6,0,0    , FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[6] = { 7,0,0    , FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[7] = { 8,0,0    , FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[8] = { 12,0,0   , FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[9] = { 13,0,0   , FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[10] = { 14,0,0  , FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[11] = { 15,0,0  , FGDir::BOTTOM, FGDir::TOP, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[12] = { 0,0,0   , FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[13] = { 0,0,0   , FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[14] = { 0,0,0   , FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };
	junctionInfos[15] = { 0,0,0   , FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, /**/FGDir::LEFT, FGDir::LEFT, };

	std::vector<FGNode>& nodes = m_nodes;
	nodes.resize(nSizeNode);
	for (long nNode = 0; nNode < nSizeNode; nNode++)
	{
		if (nNode > 11)
			continue;
		nodes[nNode] = Process_XTPFlowGraph::AddNodeWrapped(FGNodeType::NormalNode, pPage, coords[nNode], captions[nNode], junctionInfos[nNode], symbols[nNode]);
	}
	int nNode;
	nNode = 12; 	nodes[nNode] = Process_XTPFlowGraph::AddNodeWrapped(FGNodeType::ImageNode, pPage, coords[nNode], captions[nNode], junctionInfos[nNode], symbols[nNode]);
	nNode = 13; 	nodes[nNode] = Process_XTPFlowGraph::AddNodeWrapped(FGNodeType::ImageNode, pPage, coords[nNode], captions[nNode], junctionInfos[nNode], symbols[nNode]);
	nNode = 14; 	nodes[nNode] = Process_XTPFlowGraph::AddNodeWrapped(FGNodeType::ImageNode, pPage, coords[nNode], captions[nNode], junctionInfos[nNode], symbols[nNode]);
	nNode = 15; 	nodes[nNode] = Process_XTPFlowGraph::AddNodeWrapped(FGNodeType::ImageNode, pPage, coords[nNode], captions[nNode], junctionInfos[nNode], symbols[nNode]);

	nodes[1].m_pNode->SetImageIndex(4);
	nodes[5].m_pNode->SetImageIndex(5);
	nodes[8].m_pNode->SetImageIndex(2);
	nodes[9].m_pNode->SetImageIndex(3);
	nodes[11].m_pNode->SetImageIndex(3);
	nodes[12].m_pNode->SetImageIndex(6);
	nodes[13].m_pNode->SetImageIndex(7);
	nodes[14].m_pNode->SetImageIndex(6);
	nodes[15].m_pNode->SetImageIndex(7);

	///////////////////////////////////////////////////
	//// manage connections
	CXTPFlowGraphConnection* pConnection;
	for (long nNode = 0; nNode < nSizeNode - 1; nNode++)
	{
		int to[3] = { nodes[nNode].m_sJunctionInfo.toNodeNoJunction, nodes[nNode].m_sJunctionInfo.toNodeYes, nodes[nNode].m_sJunctionInfo.toNodeNo };

		for (long cnt = 0; cnt < 3; cnt++)
		{
			if (to[cnt] == 0)
				continue;
			FGDir dirFrom = nodes[nNode].m_sJunctionInfo.dirs[cnt * 2 + 0];
			FGDir dirTo = nodes[nNode].m_sJunctionInfo.dirs[cnt * 2 + 1];

			FGColor color = FGColor::BLACK;
			if (cnt == 2) // toNodeNo
				color = FGColor::ORANGE;
			Process_XTPFlowGraph::ConnectNode(pPage, nodes[nNode], nodes[to[cnt]], color, dirFrom, dirTo, 0);

			CPoint ptLocationYesNo = CPoint(0, 0);
			CSize sizeYesNo = CSize(20, 20);
			ptLocationYesNo = nodes[nNode].m_pNode->GetLocation();
			CSize sizeNode = nodes[nNode].m_pNode->GetSize();
			switch (dirFrom)
			{
			case FGDir::LEFT:
				ptLocationYesNo += CSize(-sizeYesNo.cx, sizeNode.cy / 2);
				break;
			case FGDir::TOP:
				ptLocationYesNo += CSize(sizeNode.cx / 2, -sizeYesNo.cy);
				break;
			case FGDir::RIGHT:
				ptLocationYesNo += CSize(sizeNode.cx, sizeNode.cy / 2);
				break;
			case FGDir::BOTTOM:
				ptLocationYesNo += CSize(sizeNode.cx / 2, sizeNode.cy);
				break;
			default:;
			}

			if (cnt == 1)
				AddYes(&m_wndControl, ptLocationYesNo, sizeYesNo);
			if (cnt == 2)
				AddNo(&m_wndControl, ptLocationYesNo, sizeYesNo);
		}
	}
	//PostMessage(UM_ZOOMFIT, NULL, NULL);
	//UpdateWindow();
	//m_wndControl.ZoomFitToWindow();
}

void CDlgClassificationST::AddYes(CXTPFlowGraphControl* wndControl, CPoint location, CSize size)
{
	// Add
	CXTPFlowGraphNodeCustom* pImageDesktop = wndControl->AddCustomNodeFromToolkitResource(XTP_FLOWGRAPH_SHAPE_IMAGE);
	if (pImageDesktop)
	{
		pImageDesktop->SetLocation(location);
		pImageDesktop->SetSize(size);
	}
	pImageDesktop->SetImageIndex(0);
}
void CDlgClassificationST::AddNo(CXTPFlowGraphControl* wndControl, CPoint location, CSize size)
{
	// Add
	CXTPFlowGraphNodeCustom* pImageDesktop = wndControl->AddCustomNodeFromToolkitResource(XTP_FLOWGRAPH_SHAPE_IMAGE);
	if (pImageDesktop)
	{
		pImageDesktop->SetLocation(location);
		pImageDesktop->SetSize(size);
	}
	pImageDesktop->SetImageIndex(1);
}

void CDlgClassificationST::InitImages()
{
	int count_total = m_wndControl.GetImages()->GetCount();
	CXTPFlowGraphImage* pImage;

	// 0
	pImage = m_wndControl.GetImages()->AddImage(new CXTPFlowGraphImage());
	pImage->LoadFromResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_PNG_FG_YES), _T("PNG"));
	
	// 1
	pImage = m_wndControl.GetImages()->AddImage(new CXTPFlowGraphImage());
	pImage->LoadFromResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_PNG_FG_NO), _T("PNG"));
	
	// 2
	pImage = m_wndControl.GetImages()->AddImage(new CXTPFlowGraphImage());
	pImage->LoadFromResource(AfxGetInstanceHandle(), IDB_BITMAP_SIZE_AND);

	// 3
	pImage = m_wndControl.GetImages()->AddImage(new CXTPFlowGraphImage());
	pImage->LoadFromResource(AfxGetInstanceHandle(), IDB_BITMAP_SIZE_OR);

	// 4
	pImage = m_wndControl.GetImages()->AddImage(new CXTPFlowGraphImage());
	pImage->LoadFromResource(AfxGetInstanceHandle(), IDB_BITMAP_BRIGHT_IMAGE);

	// 5
	pImage = m_wndControl.GetImages()->AddImage(new CXTPFlowGraphImage());
	pImage->LoadFromResource(AfxGetInstanceHandle(), IDB_BITMAP_COMAPCT);

	// 6
	pImage = m_wndControl.GetImages()->AddImage(new CXTPFlowGraphImage());
	pImage->LoadFromResource(AfxGetInstanceHandle(), IDB_BITMAP_NG_TAB_FOLDING);

	// 7
	pImage = m_wndControl.GetImages()->AddImage(new CXTPFlowGraphImage());
	pImage->LoadFromResource(AfxGetInstanceHandle(), IDB_BITMAP_NO_TAB_FOLDING);
}