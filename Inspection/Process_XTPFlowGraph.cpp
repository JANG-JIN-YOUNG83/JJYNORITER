#include "stdafx.h"
#include "Process_XTPFlowGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const long FG_MARGIN_X = 120;
const long FG_MARGIN_Y = 80;
const long FG_GAP_X = 240;
const long FG_GAP_Y = 120;

//-----------------------------------------------------------------------
// Summary:
//     Return the ElbowPoint of the fake Node that has a size of zero.
//-----------------------------------------------------------------------
CXTPFlowGraphConnectionPoint* Process_XTPFlowGraph::SetNodePoint(CXTPFlowGraphPage* pPage, int x, int y)
{
    CXTPFlowGraphNodeCustom* pNode = pPage->GetControl()->AddCustomNodeFromToolkitResource(XTP_FLOWGRAPH_SHAPE_RECTANGLE_ROUNDED);
    pNode->SetLocation(CPoint(x, y));
    pNode->SetSize(CSize(0, 0));

    return pNode->GetConnectionPoints()->GetAt(0);
}

/////////////////////////////////////////////////////////////////////////////
// CFlowGraphSampleApp message handlerss
Process_XTPFlowGraph::CNodeWrapped Process_XTPFlowGraph::AddNodeWrapped(PXTP_NodeType type, CXTPFlowGraphPage* pPage, CPoint coord, CString caption, SJunctionInfo junctionInfo, FGSymbol symbol, COLORREF color)
{
    if (type == PXTP_NodeType::CustomNode)
    {
        Process_XTPFlowGraph::CNodeWrapped nodeWrapped;
        nodeWrapped.m_pNode = Process_XTPFlowGraph::AddCustomFlowGraphShape(*pPage->GetControl(), symbol, coord, caption);
        nodeWrapped.m_sJunctionInfo = junctionInfo;

        nodeWrapped.bHasConnectionPoints = TRUE;
        return nodeWrapped;
    }

    CString space = _T(" ");
    std::swap(space, caption);
    caption.AppendFormat(_T("%s"), space);
    LPCTSTR bufStr = caption.GetBuffer(0);
    int nCount = 4 - count(bufStr, bufStr + caption.GetLength(), _T('\n'));
    while (nCount)
    {
        if (nCount <= 0)
            break;
        caption.AppendFormat(_T("\n"));
        nCount--;
    }
    caption.Replace(_T("\n"), _T("\n\r "));
    space.ReleaseBuffer();
    caption.ReleaseBuffer();

    // color
    if (caption.Find(_T("시작")) != -1 || caption.Find(_T("종료")) != -1)
        color = static_cast<COLORREF>(FGColor::Custom_Pink1);
     
    CSize shapeSize = CPoint(200, 88);

    CPoint pos_center;
    pos_center.x = (coord.y - 1) * FG_GAP_X + FG_MARGIN_X;
    pos_center.y = (coord.x - 1) * FG_GAP_Y + FG_MARGIN_Y;

    CPoint pos_locate;
    pos_locate.x = pos_center.x - shapeSize.cx / 2;
    pos_locate.y = pos_center.y - shapeSize.cy / 2;

    Process_XTPFlowGraph::CNodeWrapped nodeWrapped;
    switch (type)
    {
    case PXTP_NodeType::NormalNode:
        nodeWrapped.m_pNode = pPage->GetNodes()->AddNode(new CXTPFlowGraphNode());
        break;
    case PXTP_NodeType::ImageNode:
        nodeWrapped.m_pNode = pPage->GetControl()->AddCustomNodeFromToolkitResource(XTP_FLOWGRAPH_SHAPE_IMAGE);
        break;
    default:;
    }
    nodeWrapped.m_sJunctionInfo = junctionInfo;

    if (nodeWrapped.m_pNode)
    {
        nodeWrapped.m_pNode->SetLocation(pos_locate);
        nodeWrapped.m_pNode->SetSize(shapeSize);
        nodeWrapped.m_pNode->SetCaption(caption);
        nodeWrapped.m_pNode->SetColor(color);
    }
    Add4wayPoints(pPage, nodeWrapped);

    return nodeWrapped;
}

/////////////////////////////////////////////////////////////////////////////
// CFlowGraphSampleApp message handlerss
CXTPFlowGraphNode* Process_XTPFlowGraph::AddCustomFlowGraphShape(CXTPFlowGraphControl& m_wndControl, FGSymbol symbol, CPoint coord, CString caption)
{
    UINT uResShapeID = XTP_FLOWGRAPH_SHAPE_RECTANGLE_ROUNDED;
    COLORREF color = static_cast<COLORREF>(PXTP_Color::WHITE);
    CSize shapeSize = CPoint(180, 60);

    switch (symbol)
    {
    case FGSymbol::StartEnd: // RectRounded
    {
        uResShapeID = XTP_FLOWGRAPH_SHAPE_ELLIPSE;
        color = static_cast<COLORREF>(FGColor::Custom_Pink1);
    }
    break;
    case FGSymbol::Decision: // Rhombus
    {
        uResShapeID = XTP_FLOWGRAPH_SHAPE_RHOMBUS;
        color = static_cast<COLORREF>(FGColor::Custom_Yellow1);
        shapeSize = CPoint(200, 84);
    }
    break;
    case FGSymbol::Process_Simple: // Rectangle Slight
    {
        uResShapeID = XTP_FLOWGRAPH_SHAPE_RECTANGLE_ROUNDED;
        color = static_cast<COLORREF>(FGColor::Custom_Blue1);
    }
    break;
    case FGSymbol::Process_Complex: // Rectangle Dark
    {
        uResShapeID = XTP_FLOWGRAPH_SHAPE_RECTANGLE_ROUNDED;
        color = static_cast<COLORREF>(FGColor::Custom_Blue2);
    }
    break;
    case FGSymbol::InputOutput: // Parallelogram
    {
        uResShapeID = XTP_FLOWGRAPH_SHAPE_PARALLELOGRAM;
        color = static_cast<COLORREF>(FGColor::Custom_Green1);
    }
    break;
    default:; break;
    }

    CPoint pos_center;
    pos_center.x = (coord.y - 1) * FG_GAP_X + FG_MARGIN_X;
    pos_center.y = (coord.x - 1) * FG_GAP_Y + FG_MARGIN_Y;

    CPoint pos_locate;
    pos_locate.x = pos_center.x - shapeSize.cx / 2;
    pos_locate.y = pos_center.y - shapeSize.cy / 2;

    CXTPFlowGraphNodeCustom* pNode = m_wndControl.AddCustomNodeFromToolkitResource(uResShapeID);

    if (pNode)
    {
        pNode->SetLocation(pos_locate);
        pNode->SetSize(shapeSize);
        pNode->SetCaption(caption);
        pNode->SetColor(color);
    }

    return pNode;
}

//-----------------------------------------------------------------------
// Summary:
//     Create a connection between two nodes.
//     Return the CXTPFlowGraphConnection* closest to the start point for using SetCaption().
//-----------------------------------------------------------------------
CXTPFlowGraphConnection* Process_XTPFlowGraph::ConnectNode(CXTPFlowGraphPage* pPage, CNodeWrapped nodeGiver, CNodeWrapped nodeTaker,
    PXTP_Color color, PXTP_Direction dir_pt_from, PXTP_Direction dir_pt_to, long margin, CString caption)
{
    CXTPFlowGraphNode* pNodeFrom = nodeGiver.m_pNode;
    CXTPFlowGraphNode* pNodeTo = nodeTaker.m_pNode;

    int pt_index_from = 0;
    int pt_index_to = 0;
    CXTPFlowGraphConnectionPoint* pPt_from = nullptr;
    CXTPFlowGraphConnectionPoint* pPt_to = nullptr;

    // find Pt : PXTP_Direction에 따라 가장 direction 방향에 있는 point 구하기
    // NodeFrom, NodeTo에서 같은 연산하므로 어레이로 2개 묶어서 loop
    CXTPFlowGraphConnectionPoint** pPtResults[2] = { &pPt_from , &pPt_to };
    vector< CXTPFlowGraphConnectionPoint*> vvCandidates[2];
    if (!nodeGiver.bHasConnectionPoints)
        vvCandidates[0] = nodeGiver.m_elbowPoints;
    else
    {
        CXTPFlowGraphConnectionPoints* pPoints = nodeGiver.m_pNode->GetConnectionPoints();
        long nCountPoint = pPoints->GetCount();
        for (long i = 0; i < nCountPoint; i++)
        {
            CXTPFlowGraphConnectionPoint* pPoint = pPoints->GetAt(i);
            vvCandidates[0].push_back(pPoint);
        }
    }
    if (!nodeTaker.bHasConnectionPoints)
        vvCandidates[1] = nodeTaker.m_elbowPoints;
    else
    {
        CXTPFlowGraphConnectionPoints* pPoints = nodeTaker.m_pNode->GetConnectionPoints();
        long nCountPoint = pPoints->GetCount();
        for (long i = 0; i < nCountPoint; i++)
        {
            CXTPFlowGraphConnectionPoint* pPoint = pPoints->GetAt(i);
            vvCandidates[1].push_back(pPoint);
        }
    }
    PXTP_Direction dir_pt[2] = { dir_pt_from , dir_pt_to };
    for (long from_to = 0; from_to < 2; from_to++)
    {
        switch (dir_pt[from_to])
        {
        case PXTP_Direction::LEFT:
        {
            long min_max_from = LONG_MAX;
            for (long i = 0; i < vvCandidates[from_to].size(); i++)
            {
                CXTPFlowGraphConnectionPoint* pPoint = vvCandidates[from_to][i];
                CXTPFlowGraphNode* pNodePoint = pPoint->GetNode();
                int pos = 0;
                if (pNodePoint->GetSize() == CSize(0, 0))
                    pos = pNodePoint->GetLocation().x;
                else
                    pos = pPoint->GetRect().CenterPoint().x;
                if (pos < min_max_from)
                {
                    min_max_from = pos;
                    *pPtResults[from_to] = pPoint;
                }
            }
            break;
        }
        case PXTP_Direction::RIGHT:
        {
            long min_max_from = LONG_MIN;
            for (long i = 0; i < vvCandidates[from_to].size(); i++)
            {
                CXTPFlowGraphConnectionPoint* pPoint = vvCandidates[from_to][i];
                CXTPFlowGraphNode* pNodePoint = pPoint->GetNode();
                long pos = 0;
                if (pNodePoint->GetSize() == CSize(0, 0))
                    pos = pNodePoint->GetLocation().x;
                else
                    pos = pPoint->GetRect().CenterPoint().x;
                if (pos > min_max_from)
                {
                    min_max_from = pos;
                    *pPtResults[from_to] = pPoint;
                }
            }
            break;
        }
        case PXTP_Direction::TOP:
        {
            long min_max_from = LONG_MAX;
            for (long i = 0; i < vvCandidates[from_to].size(); i++)
            {
                CXTPFlowGraphConnectionPoint* pPoint = vvCandidates[from_to][i];
                CXTPFlowGraphNode* pNodePoint = pPoint->GetNode();
                long pos = 0;
                if (pNodePoint->GetSize() == CSize(0, 0))
                    pos = pNodePoint->GetLocation().y;
                else
                    pos = pPoint->GetRect().CenterPoint().y;
                if (pos < min_max_from)
                {
                    min_max_from = pos;
                    *pPtResults[from_to] = pPoint;
                }
            }
            break;
        }
        case PXTP_Direction::BOTTOM:
        {
            long min_max_from = LONG_MIN;
            for (long i = 0; i < vvCandidates[from_to].size(); i++)
            {
                CXTPFlowGraphConnectionPoint* pPoint = vvCandidates[from_to][i];
                CXTPFlowGraphNode* pNodePoint = pPoint->GetNode();
                long pos = 0;
                if (pNodePoint->GetSize() == CSize(0, 0))
                    pos = pNodePoint->GetLocation().y;
                else
                    pos = pPoint->GetRect().CenterPoint().y;
                if (pos > min_max_from)
                {
                    min_max_from = pos;
                    *pPtResults[from_to] = pPoint;
                }
            }
            break;
        }

        default:;
        }
    }

    if (pPt_from == nullptr)
    {
        //AfxMessageBox(_T("an error occurs, the node has no connection point. {C7A98282-ECD3-413E-A4D3-0903527762CC}"));
        return nullptr;
    }

    vector<CXTPFlowGraphConnectionPoint*> ElbowPoints;
    ElbowPoints.push_back(pPt_from);
    ElbowPoints.push_back(pPt_to);

    CPoint pt_from;
    CPoint pt_to;

    if (pPt_from->GetNode()->GetSize() == CSize(0, 0))
        pt_from = pPt_from->GetNode()->GetLocation();
    else
        pt_from = pPt_from->GetRect().CenterPoint() + pNodeFrom->GetLocation();
    if (pPt_to->GetNode()->GetSize() == CSize(0, 0))
        pt_to = pPt_to->GetNode()->GetLocation();
    else
        pt_to = pPt_to->GetRect().CenterPoint() + pNodeTo->GetLocation();

    AddElbowPoints(pPage, ElbowPoints, pt_from, pt_to, dir_pt_from, dir_pt_to, margin);

     // start Block 컨넥션을 반환해야 하므로 역방향, 반환하여 캡션 사용
    CXTPFlowGraphConnection* pConnection = nullptr;
    for (int i = ElbowPoints.size() - 1; i > 0; i--)
    {
        pConnection = pPage->GetConnections()->AddConnection(new CXTPFlowGraphConnection());
        pConnection->SetOutputPoint(ElbowPoints[i - 1]);
        pConnection->SetInputPoint(ElbowPoints[i]);
        if (i == ElbowPoints.size() - 1)
            pConnection->SetStyle(xtpFlowGraphConnectorStraightArrow);
        else
            pConnection->SetStyle(xtpFlowGraphConnectorStraight);
        pConnection->SetColor(static_cast<COLORREF>(color));
        if (color == FGColor::RED)
            pConnection->SetWidth(5);
    }

    return pConnection;
}

bool Process_XTPFlowGraph::Add4wayPoints(CXTPFlowGraphPage* pPage, CNodeWrapped& nodeWrapped)
{
    CXTPFlowGraphNode* pNode = nodeWrapped.m_pNode;

    CPoint ptCenter = pNode->GetLocation() + pNode->GetSize();
    CPoint ptTop = ptCenter - CSize(pNode->GetSize().cx / 2, pNode->GetSize().cy);
    CPoint ptBottom = ptCenter - CSize(pNode->GetSize().cx / 2, 0);
    CPoint ptLeft = ptCenter - CSize(pNode->GetSize().cx, pNode->GetSize().cy / 2);
    CPoint ptRight = ptCenter - CSize(0, pNode->GetSize().cy / 2);

    nodeWrapped.m_elbowPoints.push_back(Process_XTPFlowGraph::SetNodePoint(pPage, ptTop.x, ptTop.y));
    nodeWrapped.m_elbowPoints.push_back(Process_XTPFlowGraph::SetNodePoint(pPage, ptBottom.x, ptBottom.y));
    nodeWrapped.m_elbowPoints.push_back(Process_XTPFlowGraph::SetNodePoint(pPage, ptLeft.x, ptLeft.y));
    nodeWrapped.m_elbowPoints.push_back(Process_XTPFlowGraph::SetNodePoint(pPage, ptRight.x, ptRight.y));

    return true;
}

//-----------------------------------------------------------------------
// Summary:
//     XTP drawing function with xtpFlowGraphConnectorElbowArrow is a shit.
//     Find and Add Elbow Points at the list manually.
//     엘보라인이 허용되지 않는 Direction & 위치이면 false(점을 추가하지 않으므로 from-to 직선Line이 된다)
// Parameters:
//     pPage - for XTPFunction
//     listEP - list that has elbow points
//     pt_from - start point's position
//     pt_to - end point's position
//     dir_pt_from - point direction in pNodeFrom
//     dir_pt_to - point direction in pNodeTo
//-----------------------------------------------------------------------
bool Process_XTPFlowGraph::AddElbowPoints(CXTPFlowGraphPage* pPage, vector<CXTPFlowGraphConnectionPoint*>& ElbowPoints, CPoint pt_from, CPoint pt_to, PXTP_Direction dir_pt_from, PXTP_Direction dir_pt_to, long margin)
{
    const long MARGIN_DIRECTION_X = 14;
    const long MARGIN_DIRECTION_Y = 14;

    // pPt_to pos comparing to pPt_from
    PXTP_PosCoord posCoord;
    CPoint pt_diff = pt_to - pt_from;
    if (pt_diff.x == 0 && pt_diff.y == 0)		posCoord = PXTP_PosCoord::Origin;
    else if (pt_diff.x > 0 && pt_diff.y == 0)		posCoord = PXTP_PosCoord::X_Axis_right;
    else if (pt_diff.x < 0 && pt_diff.y == 0)		posCoord = PXTP_PosCoord::X_Axis_left;
    else if (pt_diff.x == 0 && pt_diff.y > 0)		posCoord = PXTP_PosCoord::Y_Axis_up;
    else if (pt_diff.x > 0 && pt_diff.y > 0)		posCoord = PXTP_PosCoord::Quadrant4;
    else if (pt_diff.x < 0 && pt_diff.y  > 0)		posCoord = PXTP_PosCoord::Quadrant3;
    else if (pt_diff.x == 0 && pt_diff.y < 0)		posCoord = PXTP_PosCoord::Y_Axis_down;
    else if (pt_diff.x > 0 && pt_diff.y < 0)		posCoord = PXTP_PosCoord::Quadrant1;
    else if (pt_diff.x < 0 && pt_diff.y < 0)		posCoord = PXTP_PosCoord::Quadrant2;

    if (posCoord == PXTP_PosCoord::Origin)
        return false;

    long x;
    long y;
    switch (dir_pt_from)
    {
    case PXTP_Direction::LEFT:
    {
        switch (dir_pt_to)
        {
        case PXTP_Direction::LEFT:
        {
            if (posCoord == PXTP_PosCoord::X_Axis_left || posCoord == PXTP_PosCoord::X_Axis_right)
                return false;
            x = min(pt_from.x, pt_to.x) - (long)(margin * FG_GAP_X) - MARGIN_DIRECTION_X;
            y = pt_to.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
            y = pt_from.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::RIGHT:
        {
            if (posCoord != PXTP_PosCoord::Quadrant2 && posCoord != PXTP_PosCoord::Quadrant3)
                return false;
            if (pt_from.x - pt_to.x < margin * FG_GAP_X)
                x = (pt_from.x + pt_to.x) / 2;
            else
                x = pt_from.x - (long)(margin * FG_GAP_X) - MARGIN_DIRECTION_X;
            y = pt_to.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
            y = pt_from.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::TOP:
        {
            if (posCoord != PXTP_PosCoord::Quadrant3)
                return false;
            x = pt_to.x;
            y = pt_from.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::BOTTOM:
        {
            if (posCoord != PXTP_PosCoord::Quadrant2)
                return false;
            x = pt_to.x;
            y = pt_from.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        default:;
            break;
        }
    }
    break;
    case PXTP_Direction::RIGHT:
    {
        switch (dir_pt_to)
        {
        case PXTP_Direction::RIGHT:
        {
            if (posCoord == PXTP_PosCoord::X_Axis_left || posCoord == PXTP_PosCoord::X_Axis_right)
                return false;
            x = max(pt_from.x, pt_to.x) + (long)(margin * FG_GAP_X) + MARGIN_DIRECTION_X;
            y = pt_to.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
            y = pt_from.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::LEFT:
        {
            if (posCoord != PXTP_PosCoord::Quadrant4 && posCoord != PXTP_PosCoord::Quadrant1)
                return false;
            if (pt_to.x - pt_from.x < margin * FG_GAP_X)
                x = (pt_from.x + pt_to.x) / 2;
            else
                x = pt_from.x + (long)(margin * FG_GAP_X) + MARGIN_DIRECTION_X;
            x = (pt_from.x + pt_to.x) / 2;
            y = pt_to.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
            y = pt_from.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::TOP:
        {
            if (posCoord != PXTP_PosCoord::Quadrant4)
                return false;
            x = pt_to.x;
            y = pt_from.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::BOTTOM:
        {
            if (posCoord != PXTP_PosCoord::Quadrant1)
                return false;
            x = pt_to.x;
            y = pt_from.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        default:;
            break;
        }
    }
    break;
    case PXTP_Direction::TOP:
    {
        switch (dir_pt_to)
        {
        case PXTP_Direction::TOP:
        {
            if (posCoord == PXTP_PosCoord::Y_Axis_up || posCoord == PXTP_PosCoord::Y_Axis_down)
                return false;
            y = min(pt_from.y, pt_to.y) - (long)(margin * FG_GAP_Y) - MARGIN_DIRECTION_Y;
            x = pt_to.x;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
            x = pt_from.x;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::BOTTOM:
        {
            if (posCoord != PXTP_PosCoord::Quadrant1 && posCoord != PXTP_PosCoord::Quadrant2)
                return false;
            if (pt_from.y - pt_to.y < margin * FG_GAP_Y)
                y = (pt_from.y + pt_to.y) / 2;
            else
                y = pt_from.y - (long)(margin * FG_GAP_Y) - MARGIN_DIRECTION_Y;
            x = pt_to.x;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
            x = pt_from.x;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::LEFT:
        {
            if (posCoord != PXTP_PosCoord::Quadrant1)
                return false;
            x = pt_from.x;
            y = pt_to.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::RIGHT:
        {
            if (posCoord != PXTP_PosCoord::Quadrant2)
                return false;
            x = pt_from.x;
            y = pt_to.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        default:;
            break;
        }
    }
    break;
    case PXTP_Direction::BOTTOM:
    {
        switch (dir_pt_to)
        {
        case PXTP_Direction::BOTTOM:
        {
            if (posCoord == PXTP_PosCoord::Y_Axis_up || posCoord == PXTP_PosCoord::Y_Axis_down)
                return false;
            y = max(pt_from.y, pt_to.y) + (long)(margin * FG_GAP_Y) + MARGIN_DIRECTION_Y;
            x = pt_to.x;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
            x = pt_from.x;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::TOP:
        {
            if (posCoord != PXTP_PosCoord::Quadrant4 && posCoord != PXTP_PosCoord::Quadrant3)
                return false;
            if (pt_to.y - pt_from.y < margin * FG_GAP_Y)
                y = (pt_from.y + pt_to.y) / 2;
            else
                y = pt_from.y + (long)(margin * FG_GAP_Y) + MARGIN_DIRECTION_Y;
            x = pt_to.x;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
            x = pt_from.x;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::LEFT:
        {
            if (posCoord != PXTP_PosCoord::Quadrant4)
                return false;
            x = pt_from.x;
            y = pt_to.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        case PXTP_Direction::RIGHT:
        {
            if (posCoord != PXTP_PosCoord::Quadrant3)
                return false;
            x = pt_from.x;
            y = pt_to.y;
            ElbowPoints.insert(ElbowPoints.begin() + 1, SetNodePoint(pPage, x, y));
        }
        break;
        default:;
            break;
        }
    }
    break;
    default:;
        break;
    }
    return true;
}