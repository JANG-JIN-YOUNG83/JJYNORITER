// CDlgDefectMapUpdate.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgDefectMapUpdate.h"
#include "afxdialogex.h"
#include "DlgDefectMap.h"


// CDlgDefectMapUpdate 대화 상자

IMPLEMENT_DYNAMIC(CDlgDefectMapUpdate, CDialogEx)

CDlgDefectMapUpdate::CDlgDefectMapUpdate(CWnd* pParent)
	: CDialogEx(IDD_DIALOG_DEFECTMAP_UPDATE, pParent)
{
    m_pParent = (CDlgDefectMap*)pParent;
}

CDlgDefectMapUpdate::~CDlgDefectMapUpdate()
{
}

void CDlgDefectMapUpdate::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS1, m_progCtrlDefectmap);
}


BEGIN_MESSAGE_MAP(CDlgDefectMapUpdate, CDialogEx)
ON_BN_CLICKED(IDCANCEL, &CDlgDefectMapUpdate::OnBnClickedCancel)
ON_BN_CLICKED(IDRESTART, &CDlgDefectMapUpdate::OnBnClickedRestart)
ON_BN_CLICKED(IDQUIT, &CDlgDefectMapUpdate::OnBnClickedQuit)
END_MESSAGE_MAP()


// CDlgDefectMapUpdate 메시지 처리기



BOOL CDlgDefectMapUpdate::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_progCtrlDefectmap.ModifyStyle(0, PBS_MARQUEE);
    m_progCtrlDefectmap.SetMarquee(TRUE, 30);   
    /*m_pParent->m_ThreadWorkType = THREAD_RUNNING;
    m_pParent->StartUIUpdateThread();*/
    //::SetEvent(m_pParent->m_threadControl_UpdateMap.m_Signal_Start);
    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgDefectMapUpdate::EndUIUpdate()
{
    m_progCtrlDefectmap.ModifyStyle(PBS_MARQUEE, 0);
    m_progCtrlDefectmap.SetRange(0, 100);
    m_progCtrlDefectmap.SetPos(100);
    this->ShowWindow(SW_HIDE);
}

void CDlgDefectMapUpdate::StopUIUpdate()
{
    m_progCtrlDefectmap.ModifyStyle(PBS_MARQUEE, 0);
    m_progCtrlDefectmap.SetRange(0, 100);
    m_progCtrlDefectmap.SetPos(0);
}

void CDlgDefectMapUpdate::OnBnClickedCancel()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_pParent->m_bPauseDefectMapUpdate = TRUE;
    ::SetEvent(m_pParent->m_threadControl_UpdateMap.m_Signal_Ready);
    StopUIUpdate();
    //m_pParent->PauseUIUpdateThread();
}

void CDlgDefectMapUpdate::OnBnClickedRestart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    ::SetEvent(m_pParent->m_threadControl_UpdateMap.m_Signal_Start);
    m_progCtrlDefectmap.ModifyStyle(0, PBS_MARQUEE);
    m_progCtrlDefectmap.SetMarquee(TRUE, 30);
    m_pParent->m_bPauseDefectMapUpdate = FALSE;
    //m_pParent->StartUIUpdateThread();

}


void CDlgDefectMapUpdate::OnBnClickedQuit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_pParent->m_bQuitDefectMapUpdate = TRUE;
    //PostMessage(UWM_DEFECTMAP_UPDATE_QUIT);
    ::SetEvent(m_pParent->m_threadControl_UpdateMap.m_Signal_Kill);
    //m_pParent->TerminateUI();
    EndUIUpdate();
    
}

