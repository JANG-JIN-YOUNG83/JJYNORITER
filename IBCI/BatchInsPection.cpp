// BatchInsPection.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "BatchInsPection.h"
#include "afxdialogex.h"
#include "IBCIDlg.h"

#include <sstream>
#include "DlgDefectMap.h"
#include "../SharedModule/VisionHostShare/ResultVision.h"
#include "../Inspection/Accessor.h"
// CBatchInsPection 대화 상자

IMPLEMENT_DYNAMIC(CBatchInsPection, CDialogEx)

CBatchInsPection::CBatchInsPection(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_BATCH_INSPECTION, pParent)
	//, m_mcidStart(1)
	//, m_mcidEnd(1000)
    , m_loggerSequence(_T("C:\\intekplus\\IBCI\\Log\\"), _T("BatchSequence.txt"))
    , m_loggerResult(_T("C:\\intekplus\\IBCI\\Log\\"), _T("BatchResult.txt"))
{
	//m_cellID = 100;
    m_fGrabSpeed = 300.f;
}

CBatchInsPection::~CBatchInsPection()
{
    m_parent->m_bBatchView = FALSE;
}

void CBatchInsPection::SetDlg(CIBCIDlg* pParent)
{
	m_parent = (CIBCIDlg*)pParent;
}

void CBatchInsPection::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BATCH, m_listBatchCtrl);
	//DDX_Text(pDX, IDC_EDIT_MCID_START, m_mcidStart);
	//DDX_Text(pDX, IDC_EDIT_MCID_END, m_mcidEnd);
	//DDX_Control(pDX, IDC_BUTTON_STOP, m_buttonStop);
}


BEGIN_MESSAGE_MAP(CBatchInsPection, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CBatchInsPection::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_START, &CBatchInsPection::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CBatchInsPection::OnBnClickedButtonStop)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_LIST_BATCH, &CBatchInsPection::OnNMClickListBatch)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CBatchInsPection::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_CELLID, &CBatchInsPection::OnBnClickedButtonMakeCellid)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CBatchInsPection::OnBnClickedButtonReset)

ON_WM_LBUTTONDBLCLK()
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_BATCH, &CBatchInsPection::OnDblclkListBatch)
ON_BN_CLICKED(IDC_BUTTON_NEXT, &CBatchInsPection::OnClickedButtonNext)
    ON_BN_CLICKED(IDC_BUTTON_PREV, &CBatchInsPection::OnClickedButtonPrev)
ON_BN_CLICKED(IDC_BUTTON_INSP, &CBatchInsPection::OnClickedButtonInsp)
END_MESSAGE_MAP()


// CBatchInsPection 메시지 처리기

void CBatchInsPection::OnBnClickedButtonAdd()
{
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
		POSITION pos;
		pos = dlg.GetStartPosition();
		CString strFullFilename;
		m_listBatchCtrl.DeleteAllItems();
		while (pos)
		{
			strFullFilename = dlg.GetNextPathName(pos);
			m_multiInlinePath.push_back(strFullFilename);
		}
        if (m_multiInlinePath.size() % 2 == 1)
        {
            MessageBox(_T("Please put the images in pairs(Bright, Dark)"));
            m_multiInlinePath.clear();
            return;
        }
        for (int i = 0; i < m_multiInlinePath.size(); i++)
        {
            CString str = m_multiInlinePath[i].Mid(m_multiInlinePath[i].ReverseFind('\\') + 1, m_multiInlinePath[i].ReverseFind('.'));
            m_listBatchCtrl.InsertItem(i + 1, str);

            CString strID = m_multiInlinePath[i].Mid(m_multiInlinePath[i].ReverseFind(_T('_')) + 1, 3);
            m_listBatchCtrl.SetItemText(i, 2, strID);

            if (m_multiInlinePath[i].Find(_T("BRIGHT")) != -1)
            {
                m_listBatchCtrl.SetItemText(i, 1, _T("BRIGHT"));
            }
            else if (m_multiInlinePath[i].Find(_T("DARK")) != -1)
            {
                m_listBatchCtrl.SetItemText(i, 1, _T("DARK"));
            }
        }
	}
	else
		return;
}


void CBatchInsPection::OnBnClickedButtonStart()
{
    if (m_multiInlinePath.size() <= 1)
        return;
	SetTimer(0, 100, NULL);
    ControlEnable(FALSE);
}


void CBatchInsPection::OnBnClickedButtonStop()
{
	KillTimer(0);
    ControlEnable(TRUE);
}


void CBatchInsPection::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0)
	{
        FILETIME timeCreate;
        FILETIME timeAccess;
        FILETIME timeWrite;

        HANDLE hFile = NULL;

        SYSTEMTIME stCreateTime;
        hFile = CreateFile(m_multiInlinePath[m_index], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);
        BOOL bRet = GetFileTime(hFile, &timeCreate, &timeAccess, &timeWrite);

        bRet = FileTimeToSystemTime(&timeCreate, &stCreateTime);

        //CString strYear;
        //CString strMonth;
        //CString strDay;
        //std::vector<CString> vecFilePath;
        //StringTokenDivideToCString(strPath, '\\', vecFilePath);

        //strYear = vecFilePath[vecFilePath.size() -4];
        //strMonth = vecFilePath[vecFilePath.size() - 3];
        //strDay = vecFilePath[vecFilePath.size() - 2];

        CString strTopBack = m_multiInlinePath[m_index];
        strTopBack.MakeUpper();

        long nCheckTOP = strTopBack.Find(_T("TOP"));
        long nCheckBACK = strTopBack.Find(_T("BACK"));
        BOOL bDirection = g_systemParameter.bDirection;
#if INSPECTION_MACHINE_UC1
        if (nCheckTOP == -1 && nCheckBACK == -1)
        {
            bDirection = g_systemParameter.bDirection;
        }
        else if (nCheckTOP != -1)
        {
            bDirection = FALSE;
        }
        else if (nCheckBACK != -1)
        {
            bDirection = TRUE;
        }
#endif

        CString strCellID;

#if INSPECTION_MACHINE_UC1
        strCellID = m_multiInlinePath[m_index].Mid(
            m_multiInlinePath[m_index].ReverseFind('\\') + 1, m_multiInlinePath[m_index].GetLength());
        strCellID = strCellID.Mid(0, strCellID.Find('_'));
#else
        strCellID = m_multiInlinePath[m_index].Mid(
            m_multiInlinePath[m_index].ReverseFind('_') + 1, m_multiInlinePath[m_index].GetLength());
        strCellID = strCellID.Mid(0, strCellID.ReverseFind('.'));
#endif // INSPECTION_MACHINE_UC1

        long nCellID = m_index/2;


        //m_parent->Inspection(m_multiInlinePath[m_index], m_multiInlinePath[m_index + 1], nCellID, !g_systemParameter.bDirection); 
        m_parent->Inspection(m_multiInlinePath[m_index], m_multiInlinePath[m_index + 1], nCellID, bDirection); 
		
		CString strResult;
        CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);

        if (unit->m_resultValue == II_RESULT_VALUE::REJECT)
        {
            strResult = _T("REJECT");
            m_listBatchCtrl.SetItemText(m_index, 3, strResult);
            m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
        }
        else if (unit->m_resultValue == II_RESULT_VALUE::PASS)
        {
            strResult = _T("PASS");
            m_listBatchCtrl.SetItemText(m_index, 3, strResult);
            m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
        }
        else
        {
            strResult = _T("INVALID");
            m_listBatchCtrl.SetItemText(m_index, 3, strResult);
            m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
        }

        for (long nIndex = 0; nIndex < m_listBatchCtrl.GetItemCount(); nIndex++)
        {
            m_listBatchCtrl.SetItemState(nIndex, ~LVIS_SELECTED, LVIS_SELECTED);
        }
        //항목 선택
        m_listBatchCtrl.SetItemState(m_index, LVIS_SELECTED, LVIS_SELECTED);
        m_listBatchCtrl.SetItemState(m_index + 1, LVIS_SELECTED, LVIS_SELECTED);
        m_listBatchCtrl.EnsureVisible(m_index, TRUE);
        m_listBatchCtrl.SetFocus();

        m_index+=2;
		/*(path index)*/
		//CString cell = _T("1000");
		//CString result = _T("REJECT");

		if (m_rowNumber <= m_multiInlinePath.size())
		{
			//m_listBatchCtrl.SetItemText(m_rowNumber, 1, cell);
			//m_listBatchCtrl.SetItemText(m_rowNumber, 2, result);
			m_rowNumber++;
		}

		if (m_index >= m_multiInlinePath.size())
		{
			m_index = 0;
			KillTimer(0);
            MessageBox(_T("Batch Inspection Finished"));
            ControlEnable(TRUE);
		}
	}

    if (nIDEvent == 10)
    {
        long nCnt = 0;

        m_parent->SendHostData(m_vecResultVision[m_nResultCheckCnt]);

        m_nResultCheckCnt++;

        if (m_vecResultVision.size() == m_nResultCheckCnt)
        {
            m_nResultCheckCnt = 0;
            KillTimer(10);
        }
    }
	CDialogEx::OnTimer(nIDEvent);
}


BOOL CBatchInsPection::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_listBatchCtrl.InsertColumn(1, _T("File Name"), NULL, 430, NULL);
    m_listBatchCtrl.InsertColumn(2, _T("Type"), NULL, 50, NULL);
	m_listBatchCtrl.InsertColumn(3, _T("ID"), NULL, 50, NULL);
	m_listBatchCtrl.InsertColumn(4, _T("Result"), NULL, 50, NULL);

	m_index = 0;
	m_rowNumber = 0;
    m_threadWaitInspEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CBatchInsPection::OnNMClickListBatch(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int idx = pNMItemActivate->iItem;

	m_index = idx;
	/*if (m_mcidStart.IsEmpty())
	{
		SetDlgItemInt(IDC_EDIT_MCID_START, idx);
		m_mcidStart.Insert(idx, IDC_EDIT_MCID_START);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_MCID_END, idx);
		m_mcidEnd.Insert(idx, IDC_EDIT_MCID_END);
	}*/
	*pResult = 0;
}

void CBatchInsPection::GetValue(std::vector<long>& vecCellID)
{
	std::ostringstream oss;
	for (int i = 0; i < vecCellID.size(); ++i)
	{
		if (i > 0)
		{
			oss << ", ";
		}
		oss << vecCellID[i];
	}
	std::string cellID = oss.str();

	m_listBatchCtrl.SetItemText(m_rowNumber, 1, CString(cellID.c_str()));
}


void CBatchInsPection::OnBnClickedButtonClear()
{
	m_listBatchCtrl.DeleteAllItems();
	m_multiInlinePath.clear();
	KillTimer(0);
	m_rowNumber = 0;

    if (m_parent->m_dlgDefectmap->m_pDlgDefectTimerView)
    {
        m_parent->UdateDefectROIViewClear();
    }
}


void CBatchInsPection::OnBnClickedButtonMakeCellid()
{
	//m_mcidStart.gets
}


void CBatchInsPection::OnBnClickedButtonReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_index = 0;
    for (long nCnt = 0; nCnt < m_listBatchCtrl.GetItemCount(); nCnt++)
    {
        m_listBatchCtrl.SetItemText(nCnt, 3, _T(""));
        m_listBatchCtrl.SetItemText(nCnt + 1, 3, _T(""));
    }

    if (m_parent->m_dlgDefectmap->m_pDlgDefectTimerView != nullptr)
    {
        m_parent->UdateDefectROIViewClear();
    }
}

void CBatchInsPection::OnDblclkListBatch(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    m_nSelect = pNMItemActivate->iItem;
    if (pNMItemActivate->iItem != -1)
    {
        //m_listBatchCtrl.SetTextBkColor(RGB(255, 255, 255));
        if (m_multiInlinePath[m_nSelect].Find(_T("DARK")) != -1)
        {
            m_nSelect = m_nSelect - 1;
        }
        m_listBatchCtrl.SetItemText(m_nSelect, 3, _T(""));
        m_listBatchCtrl.SetItemText(m_nSelect + 1, 3, _T(""));
        CString strImagePath_Bright;
        CString strImagePath_Dark;
        strImagePath_Bright = m_multiInlinePath[m_nSelect];
        strImagePath_Dark = m_multiInlinePath[m_nSelect + 1];

		m_parent->Inspection(strImagePath_Bright, strImagePath_Dark, m_index * 1, !g_systemParameter.bDirection);

        CString strResult;
        CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);

        if (unit->m_resultValue == II_RESULT_VALUE::REJECT)
        {
            strResult = _T("REJECT");
            m_listBatchCtrl.SetItemText(m_index, 3, strResult);
            m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
        }
        else
        {
            strResult = _T("PASS");
            m_listBatchCtrl.SetItemText(m_index, 3, strResult);
            m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
        }

		m_listBatchCtrl.SetItemText(m_nSelect, 3, strResult);
        m_listBatchCtrl.SetItemText(m_nSelect + 1, 3, strResult);

    }
    *pResult = 0;
}

void CBatchInsPection::OnClickedButtonNext()
{
    if (m_multiInlinePath.size() <= 1)
        return;
    //m_listBatchCtrl.SetTextBkColor(RGB(0, 0, 0));
    m_nSelect = m_listBatchCtrl.GetSelectionMark();
    if (m_nSelect == -1)
    {
        m_nSelect = 0;
    }
    else
    {
        m_nSelect += 2;
    }
    if (m_nSelect >= m_multiInlinePath.size())
    {
        m_nSelect -= 2;
    }
    if (m_multiInlinePath[m_nSelect].Find(_T("DARK")) != -1)
    {
        m_nSelect = m_nSelect - 1;
    }
    CString strImagePath_Bright;
    CString strImagePath_Dark;
    strImagePath_Bright = m_multiInlinePath[m_nSelect];
    strImagePath_Dark = m_multiInlinePath[m_nSelect + 1];

    m_listBatchCtrl.SetSelectionMark(m_nSelect);

    //기존 선택된 항목 삭제
    POSITION pos = m_listBatchCtrl.GetFirstSelectedItemPosition();
    while (pos)
    {
        int nItem = m_listBatchCtrl.GetNextSelectedItem(pos);
        m_listBatchCtrl.SetItemState(nItem, ~LVIS_SELECTED, LVIS_SELECTED);
    }

    //항목 선택
    m_listBatchCtrl.SetItemState(m_nSelect, LVIS_SELECTED, LVIS_SELECTED);
    m_listBatchCtrl.SetItemState(m_nSelect + 1, LVIS_SELECTED, LVIS_SELECTED);
    //선택한 항목으로 이동
    m_listBatchCtrl.EnsureVisible(m_nSelect, TRUE);
    m_listBatchCtrl.SetFocus();

	m_parent->Inspection(strImagePath_Bright, strImagePath_Dark, m_index * 1, !g_systemParameter.bDirection);

    CString strResult;
    CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);

    if (unit->m_resultValue == II_RESULT_VALUE::REJECT)
    {
        strResult = _T("REJECT");
        m_listBatchCtrl.SetItemText(m_index, 3, strResult);
        m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
    }
    else
    {
        strResult = _T("PASS");
        m_listBatchCtrl.SetItemText(m_index, 3, strResult);
        m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
    }

    m_listBatchCtrl.SetHotItem(m_nSelect);
    m_listBatchCtrl.SetItemText(m_nSelect, 3, strResult);
    m_listBatchCtrl.SetItemText(m_nSelect + 1, 3, strResult);
}

void CBatchInsPection::OnClickedButtonPrev()
{
    if (m_multiInlinePath.size() <= 1)
        return;

    m_nSelect = m_listBatchCtrl.GetSelectionMark();
    //m_listBatchCtrl.SetTextBkColor(RGB(0, 0, 0));
    if (m_nSelect == -1 || m_nSelect == 0)
    {
        m_nSelect = 0;
    }
    else
    {
        m_nSelect -= 2;
    }

    if (m_multiInlinePath[m_nSelect].Find(_T("DARK")) != -1)
    {
        m_nSelect = m_nSelect - 1;
    }
    CString strImagePath_Bright;
    CString strImagePath_Dark;
    strImagePath_Bright = m_multiInlinePath[m_nSelect];
    strImagePath_Dark = m_multiInlinePath[m_nSelect + 1];

    m_listBatchCtrl.SetSelectionMark(m_nSelect);

    //기존 선택된 항목 삭제
    POSITION pos = m_listBatchCtrl.GetFirstSelectedItemPosition();
    while (pos)
    {
        int nItem = m_listBatchCtrl.GetNextSelectedItem(pos);
        m_listBatchCtrl.SetItemState(nItem, ~LVIS_SELECTED, LVIS_SELECTED);
    }

    //항목 선택
    m_listBatchCtrl.SetItemState(m_nSelect, LVIS_SELECTED, LVIS_SELECTED);
    m_listBatchCtrl.SetItemState(m_nSelect + 1, LVIS_SELECTED, LVIS_SELECTED);
    //선택한 항목으로 이동
    m_listBatchCtrl.EnsureVisible(m_nSelect, TRUE);
    m_listBatchCtrl.SetFocus();

	m_parent->Inspection(strImagePath_Bright, strImagePath_Dark, m_index * 1, !g_systemParameter.bDirection);

    CString strResult;
    CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);

    if (unit->m_resultValue == II_RESULT_VALUE::REJECT)
    {
        strResult = _T("REJECT");
        m_listBatchCtrl.SetItemText(m_index, 3, strResult);
        m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
    }
    else
    {
        strResult = _T("PASS");
        m_listBatchCtrl.SetItemText(m_index, 3, strResult);
        m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
    }

    m_listBatchCtrl.SetHotItem(m_nSelect);
    m_listBatchCtrl.SetItemText(m_nSelect, 3, strResult);
    m_listBatchCtrl.SetItemText(m_nSelect + 1, 3, strResult);
}

void CBatchInsPection::OnClickedButtonInsp()
{
    if (m_multiInlinePath.size() <= 1)
        return;
    long nSel = m_listBatchCtrl.GetSelectionMark();

    if (nSel == -1)
    {
        nSel = 0;
    }

    if (m_multiInlinePath[nSel].Find(_T("DARK")) != -1)
    {
        nSel = nSel - 1;
    }
    CString strImagePath_Bright;
    CString strImagePath_Dark;
    strImagePath_Bright = m_multiInlinePath[nSel];
    strImagePath_Dark = m_multiInlinePath[nSel + 1];

    m_parent->Inspection(strImagePath_Bright, strImagePath_Dark, m_index * 1, !g_systemParameter.bDirection);

    CString strResult;
    CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);

    if (unit->m_resultValue == II_RESULT_VALUE::REJECT)
    {
        strResult = _T("REJECT");
        m_listBatchCtrl.SetItemText(m_index, 3, strResult);
        m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
    }
    else
    {
        strResult = _T("PASS");
        m_listBatchCtrl.SetItemText(m_index, 3, strResult);
        m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
    }

    m_listBatchCtrl.SetItemText(nSel, 3, strResult);
    m_listBatchCtrl.SetItemText(nSel + 1, 3, strResult);
}

void CBatchInsPection::ImageLoad()
{
    m_vecImageBright.resize(m_multiInlinePath.size() / 2);
    m_vecImageDark.resize(m_multiInlinePath.size() / 2);

    std::vector<CString> vecBright;
    std::vector<CString> veccDark;
    for (long nidx = 0; nidx < m_multiInlinePath.size(); nidx++)
    {
        long nDivid = nidx % 2;

        if (nDivid == 0)
        {
            vecBright.push_back(m_multiInlinePath[nidx]);
        }
        else
        {
            veccDark.push_back(m_multiInlinePath[nidx]);
        }
    }
    for (long nImage = 0; nImage < m_vecImageBright.size(); nImage++)
    {
        m_vecImageBright[nImage].LoadFrom(vecBright[nImage]);
        m_vecImageDark[nImage].LoadFrom(veccDark[nImage]);
    }
}

UINT CBatchInsPection::ThreadFunc_BatchInspection(LPVOID pParam)
{
    CBatchInsPection* pDlg = (CBatchInsPection*)pParam;
    return pDlg->ThreadFunc_BatchInspection();
}

UINT CBatchInsPection::ThreadFunc_BatchInspection()
{
    static CCriticalSection cs;
    cs.Lock();
    static long nLatestThreadID = 0;
    const long nThreadID = nLatestThreadID++;

    auto& control = m_vecThreadControl[nThreadID];
    auto& inspectionCell = m_vecInspectionCell[nThreadID];
    cs.Unlock();

    ::SetEvent(control.m_Signal_Ready);

    HANDLE pHandles[] = {control.m_Signal_Kill, control.m_Signal_Start};
    const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);

    while (1)
    {
        DWORD dwRet = WaitForMultipleObjects(nSignalCount, pHandles, FALSE, INFINITE);

        if ((WAIT_OBJECT_0 + 1) == dwRet)
        {
            //inspectionCell = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);
            auto& inspectionRoot = Inspection::Accessor::GetRoot();
            inspectionCell.CopyDataBase(inspectionRoot.GetInspectionCell((bool)true));
            inspectionCell.m_cellID = nThreadID;
            inspectionCell.m_isUp = true;
            //m_parent->Inspection();
            inspectionCell.SetInspectionSplitImage(m_vecImageBright[nThreadID], m_vecImageBright[nThreadID]);
            inspectionCell.ResetResult();

            //auto* align = (CInspectionCellAlign*)inspectionCell.GetProcessingItem(ProcItemCategory::cellAlign);
            //align->DoInspection();
            //CCriticalSection cs;
            //cs.Lock();
            inspectionCell.DoInspection();
            Share::ResultVision ResultData;
            //SendHostResultDataInfo(ResultData, unit);

            std::vector<IPVM::Image_8u_C1> vecCropImg_Bright;
            std::vector<IPVM::Image_8u_C1> vecCropImg_Dark;
            m_parent->GetDefectCropImage(m_vecImageBright[nThreadID], m_vecImageDark[nThreadID], vecCropImg_Bright,
                vecCropImg_Dark, &inspectionCell);

            long nSizeDivideVertical = (long)inspectionCell.m_ISICellAllDefect.m_vvMeasureDefect.size();
            auto& vecMeasureDefect = inspectionCell.m_ISICellAllDefect.m_vvMeasureDefect;
            m_parent->SendHostResultDataInfo(ResultData, vecCropImg_Bright, vecCropImg_Dark, &inspectionCell);
            m_parent->ResultMismatchData(
                nSizeDivideVertical, vecMeasureDefect, _T("test1234"), inspectionCell.m_cellID, ResultData);

            CString strResult;

            if (inspectionCell.m_resultValue == II_RESULT_VALUE::REJECT)
            {
                strResult = _T("REJECT");
                m_listBatchCtrl.SetItemText(nThreadID * 2, 3, strResult);
                m_listBatchCtrl.SetItemText((nThreadID * 2) + 1, 3, strResult);
            }
            else if (inspectionCell.m_resultValue == II_RESULT_VALUE::PASS)
            {
                strResult = _T("PASS");
                m_listBatchCtrl.SetItemText(nThreadID * 2, 3, strResult);
                m_listBatchCtrl.SetItemText((nThreadID * 2) + 1, 3, strResult);
            }
            else
            {
                strResult = _T("INVALID");
                m_listBatchCtrl.SetItemText(nThreadID * 2, 3, strResult);
                m_listBatchCtrl.SetItemText((nThreadID * 2) + 1, 3, strResult);
            }
            for (long nIndex = 0; nIndex < m_listBatchCtrl.GetItemCount(); nIndex++)
            {
                m_listBatchCtrl.SetItemState(nIndex, ~LVIS_SELECTED, LVIS_SELECTED);
            }

            m_listBatchCtrl.SetItemState(nThreadID * 2, LVIS_SELECTED, LVIS_SELECTED);
            m_listBatchCtrl.SetItemState((nThreadID * 2) + 1, LVIS_SELECTED, LVIS_SELECTED);
            m_listBatchCtrl.EnsureVisible(nThreadID, TRUE);
            m_listBatchCtrl.SetFocus();
            DWORD dwRet = WaitForSingleObject(m_threadWaitInspEnd, m_fGrabSpeed);
            //cs.Unlock();
        }
    }
}

void CBatchInsPection::OnBnClickedButtonHostMessageSend()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    //for (long n = 0; n < m_nLoopCnt; n++)
    //{
    //    SetTimer(10, m_fGrabSpeed, NULL);
    //}
    //

    //.. Host 에서 IQ 이미지 정보 관련하여 데이터 전송 (Camera Info)
    CString topBack;
    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");
    //.. 만약에 양끝을 예외처리 할거라면 이광남 선임에게 이야기 해주고 이것도 수정해야하고
    for (long nLoop = 0; nLoop < m_nLoopCnt; nLoop++)
    {
        for (long n = 0; n < m_vecResultVision.size(); n++)
        {
            m_loggerResult.Log(0, _T("%d Index Push"), (nLoop + 1) * (n + 1));
            m_vecResultVision[n].m_vecCamera.clear();
            if (m_parent->m_bReceiveRequestData == TRUE) // m_parent->m_bReceiveRequestData == TRUE
            {
                m_parent->m_bReceiveRequestData = FALSE;
                Share::CameraInfo cameraInfo;
                for (long nCam = 0; nCam < 2; nCam++) // LG 기준 카메라 정보는 2개
                {
                    if (topBack == _T("TOP"))
                    {
                        cameraInfo.m_nCamera_Location = 0; // TOP
                    }
                    else
                    {
                        cameraInfo.m_nCamera_Location = 1; //BACK
                    }
                    cameraInfo.m_nCamera_Number = nCam + 1;
                    cameraInfo.m_dResolution_X = g_systemParameter.m_dPixelToUmXUp;
                    cameraInfo.m_dResolution_Y = g_systemParameter.m_dPixelToUmY;
                    cameraInfo.m_nCamera_Gain = 1.5;
                    double dExposure;
                    //m_cisComm->GetGrabExcutor(nCam)->GetExposureTime(dExposure, dExposure);
                    cameraInfo.m_nExposure_Time = 0.0f;
                    cameraInfo.m_fAngle = 0.0f;

                    Share::ImageInfo ImageInfo;
                    //align->image
                    cameraInfo.m_vecImageQuality.clear();
                    CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);

                    unit->m_cellID = n;
                    unit->m_isUp = true;
                    //m_->arent->Inspection();
                    IPVM::Image_8u_C1 ImgBright;
                    IPVM::Image_8u_C1 ImgDark;
                    ImgBright.LoadFrom(m_multiInlinePath[n]);
                    ImgDark.LoadFrom(m_multiInlinePath[n + 1]);
                    unit->SetInspectionSplitImage(ImgBright, ImgDark);

                    unit->DoInspection();
                    auto* itemCellAlign = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);
                    if (nCam == 0)
                    {
                        for (long nEdge = 1; nEdge < itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size() - 1;
                             nEdge++)
                        {
                            // 1 lane 인 경우, 1,2 Edge
                            // 2 lane 인 경우, 1,2,3,4 Edge
                            IPVM::Rect rtResult;
                            rtResult = itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge];

                            IPVM::Rect rtCropROI;
                            rtCropROI.m_left = 0;
                            rtCropROI.m_top = 0;
                            rtCropROI.m_bottom = rtResult.Height();
                            rtCropROI.m_right = rtResult.Width();

                            IPVM::Image_8u_C1 CropImage;
                            CropImage.Create(rtResult.Width(),
                                rtResult
                                    .Height()); // 좀 더 크게 보여 주기 위해 어떻게 진행이 되어야 할까? off set 값을 어떻게 계산하여 넣어줄까?
                            IPVM::ImageProcessing::Copy(
                                itemCellAlign->m_inspectionImageDark, rtResult, rtCropROI, CropImage);
                            ImageInfo.m_CropImage_LaneEdge = CropImage;
                            ImageInfo.nEdgeNum = nEdge + 1;
                            if (g_systemParameter.nLaneCount == 1)
                            {
                                ImageInfo.nLaneNum = 1;
                            }
                            else if (g_systemParameter.nLaneCount == 2)
                            {
                                if (nEdge <= itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size())
                                {
                                    ImageInfo.nLaneNum = 1;
                                }
                                else
                                {
                                    ImageInfo.nLaneNum = 2;
                                }
                            }
                            else
                            {
                                ImageInfo.nLaneNum = 2;
                            }

                            ImageInfo.nSizeX = rtResult.Width();
                            ImageInfo.nSizeY = rtResult.Height();
                            ImageInfo.nFocus = 0;
                            //ImageInfo.nGrayLevelValue[0] = 10; // 이거 해줘야 하나?
                            //ImageInfo.nGrayLevelValue[1] = 11;

                            ImageInfo.nGrayLevelValue_Bright
                                = itemCellAlign->m_AlignGrayValue.vecGV_Bright[nEdge]; // 여기에 값을 넣어줘야함
                            ImageInfo.nGrayLevelValue_Dark = itemCellAlign->m_AlignGrayValue.vecGV_Dark[nEdge];

                            itemCellAlign->m_AlignGrayValue.vecGV_Bright;
                            itemCellAlign->m_AlignGrayValue.vecGV_Dark;
                            cameraInfo.m_vecImageQuality.push_back(ImageInfo);
                        }
                    }

                    m_vecResultVision[n].m_vecCamera.push_back(cameraInfo);
                }
            }
            m_parent->SendHostData(m_vecResultVision[n]);

            Sleep(m_fGrabSpeed);
        }
    }
}

void CBatchInsPection::ControlEnable(BOOL bEnable)
{
    GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_PREV)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_INSP)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(bEnable);
}