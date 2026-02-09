// FlowChart Standard
// 우선 ppt등으로 그린 후 UI 코딩
// NODE, LINE 배치
//      1. 최대한 겹치는 라인 없도록 배치
// 라인겹침 허용 조건
//      1. 같은 방향
//      2. 겹쳐진 후 분기 없음(같은 포인트로 input해야함)
// NODE indexing order
//      0, 1, 2
//      3, 4, 5
// 


#pragma once
#include <vector>

using namespace std;

#define FGColor       Process_XTPFlowGraph::PXTP_Color
#define FGDir       Process_XTPFlowGraph::PXTP_Direction
#define FGPCoord    Process_XTPFlowGraph::PXTP_PosCoord
#define FGSymbol     Process_XTPFlowGraph::PXTP_Symbol
#define FGNodeType     Process_XTPFlowGraph::PXTP_NodeType
#define FGNode     Process_XTPFlowGraph::CNodeWrapped

namespace Process_XTPFlowGraph
{
    enum class PXTP_Symbol
    {
        StartEnd,        // RectRounded
        Decision,        // Rhombus
        Process_Simple,  // Rectangle
        Process_Complex, // Rectangle Dark
        InputOutput,     // Parallelogram
    };
    enum class PXTP_Color
    {
        BLACK = RGB(0, 0, 0),
        WHITE = RGB(255, 255, 255),  //
        RED = RGB(255, 0, 0),
        GREEN = RGB(0, 255, 0),
        BLUE = RGB(0, 0, 255),  //
        YELLOW = RGB(255, 255, 0),
        MAGENTA = RGB(0, 255, 255),
        CYAN = RGB(255, 0, 255),  //
        ORANGE = RGB(255, 165, 0),

        Custom_Pink1 = RGB(255, 191, 201),
        Custom_Yellow1 = RGB(255, 255, 138),
        Custom_Green1 = RGB(139, 239, 145),
        Custom_Blue1 = RGB(108, 214, 255), // light blue
        Custom_Blue2 = RGB(88, 134, 255), // dark blue
    };
    enum class PXTP_Direction
    {
        LEFT,
        TOP,
        RIGHT,
        BOTTOM,
    };
    enum class PXTP_PosCoord
    {
        Quadrant1,
        Quadrant2,
        Quadrant3,
        Quadrant4,

        X_Axis_left,
        X_Axis_right,
        Y_Axis_up,
        Y_Axis_down,

        Origin,
    };
    enum class PXTP_NodeType
    {
        NormalNode,
        CustomNode,
        ImageNode,
    };

    struct SJunctionInfo
    {
        int toNodeNoJunction;
        int toNodeYes;
        int toNodeNo;

        FGDir dirs[6]; // NoJunction_from, NoJunction_to, yes_from, yes_to, no_from, no_to;
    };

    class CNodeWrapped
    {
    public:
        CNodeWrapped()
            :bHasConnectionPoints(false)
        {
        };
        CNodeWrapped(CXTPFlowGraphNode* pNode, SJunctionInfo  sJunctionInfo)
        {
            m_pNode = pNode;
            m_sJunctionInfo = sJunctionInfo;
            bHasConnectionPoints = false;
        }
        virtual ~CNodeWrapped() {};

    public:
        CXTPFlowGraphNode* m_pNode;
        SJunctionInfo m_sJunctionInfo;
        bool bHasConnectionPoints;

        vector<CXTPFlowGraphConnectionPoint*> m_elbowPoints;
    };

    CXTPFlowGraphNode* AddCustomFlowGraphShape(CXTPFlowGraphControl& m_wndControl, FGSymbol symbol, CPoint coord, CString caption);
    CNodeWrapped AddNodeWrapped(PXTP_NodeType type, CXTPFlowGraphPage* pPage, CPoint coord, CString caption, SJunctionInfo junctionInfo, FGSymbol symbol, COLORREF color = RGB(255, 255, 255));

    CXTPFlowGraphConnection* ConnectNode(CXTPFlowGraphPage* pPage, CNodeWrapped nodeGiver, CNodeWrapped nodeTaker, PXTP_Color color, PXTP_Direction from, PXTP_Direction to, long margin, CString caption = _T(""));
    CXTPFlowGraphConnectionPoint* SetNodePoint(CXTPFlowGraphPage* pPage, int x, int y);
    bool                          AddElbowPoints(CXTPFlowGraphPage* pPage, vector<CXTPFlowGraphConnectionPoint*>& ElbowPoints, CPoint pt_from, CPoint pt_to, PXTP_Direction pt_pos_from, PXTP_Direction pt_pos_to, long margin);
    bool                          Add4wayPoints(CXTPFlowGraphPage* pPage, CNodeWrapped& node);

};