// CDlgBatchInpection.cpp: 구현 파일
//

#include "stdafx.h"
#include "Inspection.h"
#include "CDlgBatchInpection.h"
#include "afxdialogex.h"
#include "DlgInspectionMain.h"
#include "../Module/Utility/Logger.h"

#include "Gadget/TimeCheck.h"


#define MESSAGE_GRIDLIST (WM_USER + 400)
// CDlgBatchInpection 대화 상자

IMPLEMENT_DYNAMIC(CDlgBatchInpection, CDialogEx)

CDlgBatchInpection::CDlgBatchInpection(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DLG_BATCH_INSPECTIONMODE, pParent)
// , m_mcidStart(1)
// , m_mcidEnd(1000)
    , m_nTheadReset(0)
{
    // m_parent = (CDlgInspectionMain*) pParent;

    m_rowNumber = 0;
    m_cellID = 0;
    m_index = 0;
    m_fGrabSpeed = 300.f;
    m_stopBatchInsp = FALSE;
    //m_ctrl_Grid.SetCallbackFunc(virtualDefectGrid, (LPARAM)this);
    m_threadWaitInspEnd = INVALID_HANDLE_VALUE;
}

CDlgBatchInpection::~CDlgBatchInpection()
{
    ThreadStop_BatchInspection();
    if (m_threadWaitInspEnd != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_threadWaitInspEnd);
    }
}

void CDlgBatchInpection::SetDlg(CDlgInspectionMain* pParent)
{
    m_parent = (CDlgInspectionMain*)pParent;
}
void CDlgBatchInpection::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_BATCH, m_listBatchCtrl);
    DDX_Control(pDX, IDC_CHECK_EXCELSAVE, m_chk_ExcelSave);
    DDX_Control(pDX, IDC_EDIT_GRAP_SPEED, m_editGrapSpeed);
    DDX_Control(pDX, IDC_CHECK_EXCELDEFECTPOSADD, m_chk_DefectPosAdd);
}


BEGIN_MESSAGE_MAP(CDlgBatchInpection, CDialogEx)
ON_BN_CLICKED(IDC_BUTTON_ADD, &CDlgBatchInpection::OnBnClickedButtonAdd)
ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CDlgBatchInpection::OnBnClickedButtonClear)
ON_BN_CLICKED(IDC_BUTTON_START, &CDlgBatchInpection::OnBnClickedButtonStart)
ON_BN_CLICKED(IDC_BUTTON_STOP, &CDlgBatchInpection::OnBnClickedButtonStop)
ON_WM_TIMER()
ON_NOTIFY(NM_DBLCLK, IDC_LIST_BATCH, &CDlgBatchInpection::OnNMDblclkListBatch)
ON_BN_CLICKED(IDC_BUTTON_NEXT, &CDlgBatchInpection::OnBnClickedButtonNext)
ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, &CDlgBatchInpection::OnBnClickedButtonPrevious)
ON_BN_CLICKED(IDC_BUTTON_RESET, &CDlgBatchInpection::OnBnClickedButtonReset)
ON_BN_CLICKED(IDC_BUTTON_INSPECTION, &CDlgBatchInpection::OnBnClickedButtonInspection)
ON_BN_CLICKED(IDC_BUTTON_EXCELSAVEPATH, &CDlgBatchInpection::OnBnClickedButtonExcelsavepath)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_BATCH, &CDlgBatchInpection::OnNMCustomdrawListBatch)
END_MESSAGE_MAP()


// CDlgBatchInpection 메시지 처리기

void CDlgBatchInpection::OnBnClickedButtonAdd()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    const TCHAR szFilters[] = _T("Image files (*.bmp, *.png, *.jpg) | *.bmp;*.png;*.jpg|");

    CFileDialog dlg(TRUE, _T(""), _T(""),
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, szFilters);

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
            //IPVM::Image_8u_C1 image;
            //image.LoadFrom(strFullFilename);
            //m_parent->m_vecMultiCellImage.push_back(image);
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
    {
        return;
    }
}

void CDlgBatchInpection::GetValue(std::vector<long>& vecCellID)
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


void CDlgBatchInpection::OnBnClickedButtonClear()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_listBatchCtrl.DeleteAllItems();
    //m_listBatchCtrl.SetTextBkColor(RGB(255,255,255));
    m_multiInlinePath.clear();    
    m_parent->m_vecMultiCellImage.clear();
    KillTimer(0);
    m_rowNumber = 0;
    m_index = 0;
    m_parent->m_nExcelRow = 0;
}

void CDlgBatchInpection::OnBnClickedButtonStart()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    GetGrapSpeed();
    BOOL bcheck = m_chk_ExcelSave.GetCheck();
    
    if (bcheck == TRUE)
    {
        m_parent->ExcelRejectInput();
    }
    // Grap Speed 없을 때 타이머로 동작
    // Grap Speed 입력 했을 때는 Thread로 동작
   /* if (m_fGrabSpeed > 0)
    {
        ::SetEvent(m_threadControl_BatchSimulation.m_Signal_Start);
    }*/

    //else
    //{
    if (m_multiInlinePath.size() <= 1)
        return;
    SetTimer(0, 200, NULL);
    ControlEnable(FALSE);

    //}
    
    //SetEvent(m_threadControl_BatchInspection.m_Signal_Start);
}

void CDlgBatchInpection::OnBnClickedButtonStop()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    KillTimer(0);
    m_stopBatchInsp = TRUE;
    ControlEnable(TRUE);
}

void CDlgBatchInpection::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (nIDEvent == 0)
    {
        //KillTimer(0);
        CString strID;
       
        //for (long idx = 0; idx < m_multiInlinePath.size(); idx++)
        
        if (m_index < m_multiInlinePath.size())
        {
            
            {    
                IPVM::Image_8u_C1 cellImage, brightImage, darkImage;
                cellImage.LoadFrom(m_multiInlinePath[m_index]);
                //cellImage = image;

                brightImage.LoadFrom(m_multiInlinePath[m_index]);
                darkImage.LoadFrom(m_multiInlinePath[m_index+1]);

                m_parent->m_setupImage.m_imageBright[0] = m_parent->m_setupImage.m_imageBright[1] = brightImage;
                m_parent->m_setupImage.m_imageDark[0] = m_parent->m_setupImage.m_imageDark[1] = darkImage;

                TCHAR programpath[_MAX_PATH];
                GetCurrentDirectory(_MAX_PATH, programpath);
                CString ImageFolderName = _T("\\DataSearchImage.jpg");
                CString strImagePath = programpath + ImageFolderName;


                CString str = _T("D:\\DataSearchImage.jpg");
                BOOL bRet;
                CFileFind find;
                bRet = find.FindFile(strImagePath);
                if (bRet == FALSE)
                {
                    darkImage.SaveJpeg(strImagePath);
                }
                m_parent->UpdateEditInspectionItem();
                m_parent->OnBnClickedButtonTestAll();
                //m_parent->ShowBatchInspRejectROI();
                CString strResult;

                if (m_parent->m_bReject == TRUE)
                {
                    //m_listBatchCtrl.SetTextBkColor(RGB(255,0,0));
                    strResult = _T("REJECT");
                }
                else
                {
                    // 여기서 블랍의 정보에 대해 결과 값을 갖고 와서 써야함...
                   // m_listBatchCtrl.SetTextBkColor(RGB(0,255,0));
                    strResult = _T("PASS");

                    if (m_parent->m_nRejectType == II_RESULT_VALUE::INVALID)
                    {
                        strResult = _T("INVALID");
                    }
                }
                
                m_listBatchCtrl.SetItemText(m_index, 3, strResult);
                m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
                for (long nIndex = 0; nIndex < m_listBatchCtrl.GetItemCount(); nIndex++)
                {
                    m_listBatchCtrl.SetItemState(nIndex, ~LVIS_SELECTED, LVIS_SELECTED);
                }
                //항목 선택
                m_listBatchCtrl.SetItemState(m_index, LVIS_SELECTED, LVIS_SELECTED);
                m_listBatchCtrl.SetItemState(m_index + 1, LVIS_SELECTED, LVIS_SELECTED);

                //                //기존 선택된 항목 삭제
                //POSITION pos = m_listBatchCtrl.GetFirstSelectedItemPosition();
                //while (pos)
                //{
                //    int nItem = m_listBatchCtrl.GetNextSelectedItem(pos);
                //    m_listBatchCtrl.SetItemState(m_index, ~LVIS_SELECTED, LVIS_SELECTED);
                //}
                //선택한 항목으로 이동
                m_listBatchCtrl.EnsureVisible(m_index, TRUE);
                //m_listBatchCtrl.EnsureVisible(m_index+1, TRUE);
                m_listBatchCtrl.SetFocus();
                
                m_index += 2;
                
                if (m_chk_ExcelSave.GetCheck() == TRUE)
                {
                    m_parent->ExcelRejectDataInput();
                    m_parent->ExcelFileSave();
                    
                }

                //for (int i = 0; i < 100000; i++);
                
            }
            if (m_index == m_multiInlinePath.size())
            {
                KillTimer(0);
                MessageBox(_T("Batch Inspection Finished"));
                ControlEnable(TRUE);
                CString strPath;
                GetDlgItemText(IDC_EDIT_EXCELSAVEPATH, strPath);
                if (m_chk_ExcelSave.GetCheck() == TRUE)
                {
                    m_parent->ExcelFileSave(strPath);
                }
                ::DeleteFile(_T("D:\\iBCI\\Result\\temp.xlsx"));
            }

        }
    }

    CDialogEx::OnTimer(nIDEvent);
}

BOOL CDlgBatchInpection::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.


    m_listBatchCtrl.InsertColumn(1, _T("File Name"), NULL, 350, NULL);
    m_listBatchCtrl.InsertColumn(2, _T("Type"), NULL, 50, NULL);
    m_listBatchCtrl.InsertColumn(3, _T("ID"), NULL, 50, NULL);
    m_listBatchCtrl.InsertColumn(4, _T("Result"), NULL, 50, NULL);

    m_index = 0;
    m_rowNumber = 0;

    CString strPath;
    strPath.Format(_T("D:\\iBCI\\Result"));
    SetDlgItemText(IDC_EDIT_EXCELSAVEPATH, strPath);  

    strPath.Format(_T("%.1f"), m_fGrabSpeed);
    SetDlgItemText(IDC_EDIT_GRAP_SPEED, strPath);

    Utility::Logger loggerSequence(_T("C:\\intekplus\\IBCI\\Log\\"), _T("BatchSequence.txt"));
    Utility::Logger loggerResult(_T("C:\\intekplus\\IBCI\\Log\\"), _T("BatchResult.txt"));


    // Initialize
    m_threadControl_BatchInspection.resize(20);
    m_batchInspectionCellGroup.reserve(20);
    for (long idx = 0; idx < 20; idx++)
    {
        m_batchInspectionCellGroup.emplace_back(loggerSequence, loggerResult);
    }
    
    m_threadWaitInspEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
    //ThreadStart_BatchInspection();


    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgBatchInpection::OnNMDblclkListBatch(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

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


        m_parent->GetDlgItem(IDC_BUTTON_IMAGESAVE)->SetWindowTextW(strImagePath_Bright);


        IPVM::Image_8u_C1 showImage;
        int nCnt = 1;
        long nWidth = 0;
        nCnt = m_parent->GetDlgItemInt(IDC_EDIT_AREAINDEX);

        IPVM::Image_8u_C1 image_Bright;
        IPVM::Image_8u_C1 image_Dark;
        image_Bright.LoadFrom(strImagePath_Bright);
        image_Dark.LoadFrom(strImagePath_Dark);
        int nImageWidth = 0, nImageHeight = 0;

  //      //핀홀비전 영상 Resize Start

  //      strImagePath_Bright.Replace(_T("_BRIGHT_"), _T("BRIGHT_RESIZE_"));
  //      strImagePath_Dark.Replace(_T("_DARK_"), _T("DARK_RESIZE_"));

		//IPVM::Image_8u_C1 resizeBrightImage, resizeDarkImage;

  //      resizeBrightImage.Create(image_Bright.GetSizeX(), image_Bright.GetSizeY() / 5);
  //      resizeDarkImage.Create(image_Dark.GetSizeX(), image_Dark.GetSizeY() / 5);

  //      long nMergeCount = 5;

  //      for (long nPosY = 0; nPosY < resizeBrightImage.GetSizeY(); nPosY++)
  //      {
  //          BYTE* pbyLineBright = resizeBrightImage.GetMem(0, nPosY);
  //          BYTE* pbyLineDark = resizeDarkImage.GetMem(0, nPosY);

  //          for (long nPosX = 0; nPosX < image_Bright.GetSizeX(); nPosX++)
  //          {
  //              long nSumBright = 0;
  //              long nSumDark = 0;
  //              long nAvg = 0;
  //              long nRepeat = (image_Bright.GetSizeY() - nPosY > nMergeCount) ? 5 : image_Bright.GetSizeY() - nPosY;

  //              for (long nIndex = 0; nIndex < nRepeat; nIndex++)
  //              {
  //                  nSumBright += *image_Bright.GetMem(nPosX, (nPosY * nMergeCount) + nIndex);
  //                  nSumDark += *image_Dark.GetMem(nPosX, (nPosY * nMergeCount) + nIndex);
  //              }

  //              nAvg = nSumBright / (float)nMergeCount;

  //              pbyLineBright[nPosX] = nAvg;

  //              nAvg = nSumDark / (float)nMergeCount;

  //              pbyLineDark[nPosX] = nAvg;
  //          }
  //      }

  //      resizeBrightImage.SavePng(strImagePath_Bright);
  //      resizeDarkImage.SavePng(strImagePath_Dark);
  //      //핀홀비전 Resize End

        //strImagePath_Bright.Replace(_T("_OS_"), _T("_DS_"));
        //strImagePath_Dark.Replace(_T("_OS_"), _T("_DS_"));
 
        //IPVM::Image_8u_C1 image_Bright_DS;
        //IPVM::Image_8u_C1 image_Dark_DS;
        //image_Bright_DS.LoadFrom(strImagePath_Bright);
        //image_Dark_DS.LoadFrom(strImagePath_Dark);

        //IPVM::Image_8u_C1 image_Bright_Merge;
        //IPVM::Image_8u_C1 image_Dark_Merge;

        //image_Bright_Merge.Create(image_Bright.GetSizeX() * 2, image_Bright.GetSizeY());
        //image_Dark_Merge.Create(image_Dark.GetSizeX() * 2, image_Dark.GetSizeY());

        //IPVM::Rect copyRectSrc(image_Bright);
        //IPVM::Rect copyRectDst(image_Bright);
        //copyRectDst.MoveToX(image_Bright.GetSizeX());
        //IPVM::ImageProcessing::Copy(image_Bright, copyRectSrc, copyRectSrc, image_Bright_Merge);
        //IPVM::ImageProcessing::Copy(image_Bright_DS, copyRectSrc, copyRectDst, image_Bright_Merge);

        //IPVM::ImageProcessing::Copy(image_Dark, copyRectSrc, copyRectSrc, image_Dark_Merge);
        //IPVM::ImageProcessing::Copy(image_Dark_DS, copyRectSrc, copyRectDst, image_Dark_Merge);

        //strImagePath_Bright.Replace(_T("_DS_"), _T(""));
        //strImagePath_Dark.Replace(_T("_DS_"), _T(""));
      
        //image_Bright_Merge.SaveBmp(strImagePath_Bright);
        //image_Dark_Merge.SaveBmp(strImagePath_Dark);


        m_parent->m_setupImage.m_imageBright[0] = image_Bright;
        m_parent->m_setupImage.m_imageDark[0] = image_Dark;
        m_parent->UpdateEditInspectionItem(nCnt);
        m_parent->ManualTestAll();
        m_parent->ShowBatchInspRejectROI();
        CString strResult;
        if (m_parent->m_bReject == TRUE)
        {
            strResult = _T("REJECT");
        }
        else
        {
            // 여기서 블랍의 정보에 대해 결과 값을 갖고 와서 써야함...
            strResult = _T("PASS");
            if (m_parent->m_nRejectType == II_RESULT_VALUE::INVALID)
            {
                strResult = _T("INVALID");
            }
        }
        m_listBatchCtrl.SetItemText(m_nSelect, 3, strResult);
        m_listBatchCtrl.SetItemText(m_nSelect + 1, 3, strResult);
        //m_listBatchCtrl.SetTextBkColor(RGB(250, 250, 0));

        m_parent->GetDlgItem(IDC_BUTTON_SHOW_BRIGHT_IMAGE)->EnableWindow(true);
        m_parent->GetDlgItem(IDC_BUTTON_SHOW_DARK_IMAGE)->EnableWindow(true);

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

    }
    *pResult = 0;
}
void CDlgBatchInpection::UpadateList()
{
    for (long nIndex = 0; nIndex < m_listBatchCtrl.GetItemCount(); nIndex++)
    {

    }
   // m_listBatchCtrl.SetItemCount
}
void CDlgBatchInpection::OnBnClickedButtonNext()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_multiInlinePath.size() <= 1)
        return;

    m_nSelect = m_listBatchCtrl.GetSelectionMark();

    m_listBatchCtrl.SetTextBkColor(RGB(0, 0, 0));
    if (m_nSelect == -1)
    {
        m_nSelect = 0;
    }
    else
    {
        m_nSelect+=2;
    }
    if (m_nSelect >= m_multiInlinePath.size())
    {
        m_nSelect -= 2;
    }
     if (m_multiInlinePath[m_nSelect].Find(_T("DARK")) != -1)
    {
        m_nSelect = m_nSelect - 1;
    }
    CString strImagePath_bright;
    CString strImagePath_dark;
    strImagePath_bright = m_multiInlinePath[m_nSelect];
    strImagePath_dark = m_multiInlinePath[m_nSelect + 1];
    m_parent->GetDlgItem(IDC_BUTTON_IMAGESAVE)->SetWindowTextW(strImagePath_bright);

    IPVM::Image_8u_C1 image_bright;
    IPVM::Image_8u_C1 image_dark;
    image_bright.LoadFrom(strImagePath_bright);
    image_dark.LoadFrom(strImagePath_dark);

    m_parent->m_setupImage.m_imageBright[0] = image_bright;
    m_parent->m_setupImage.m_imageDark[0] = image_dark;
    m_parent->UpdateEditInspectionItem();
    m_parent->OnBnClickedButtonTestAll();
    //m_parent->ShowBatchInspRejectROI();
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
    m_listBatchCtrl.SetItemState(m_nSelect+1, LVIS_SELECTED, LVIS_SELECTED);
    //선택한 항목으로 이동
    m_listBatchCtrl.EnsureVisible(m_nSelect, TRUE);
    m_listBatchCtrl.SetFocus();

    CString strResult;
    if (m_parent->m_bReject == TRUE)
    {
        strResult = _T("REJECT");
    }
    else
    {
        // 여기서 블랍의 정보에 대해 결과 값을 갖고 와서 써야함...
        strResult = _T("PASS");
        if (m_parent->m_nRejectType == II_RESULT_VALUE::INVALID)
        {
            strResult = _T("INVALID");
        }
    }

    m_listBatchCtrl.SetHotItem(m_nSelect);
    m_listBatchCtrl.SetItemText(m_nSelect, 3, strResult);
    m_listBatchCtrl.SetItemText(m_nSelect + 1, 3, strResult);

    ////특정위치 하이라이트
    //m_listBatchCtrl.SetSelectionMark(m_nSelect);
    //m_listBatchCtrl.EnsureVisible(m_nSelect, TRUE); //스크롤
    //m_listBatchCtrl.SetItemState(m_listBatchCtrl.GetSelectionMark(), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    //m_listBatchCtrl.SetFocus();




    //m_listBatchCtrl.SetTextBkColor(RGB(250, 250, 0));
}

void CDlgBatchInpection::OnBnClickedButtonPrevious()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_multiInlinePath.size() <= 1)
        return;
    m_nSelect = m_listBatchCtrl.GetSelectionMark();
    m_listBatchCtrl.SetTextBkColor(RGB(0, 0, 0));
    if (m_nSelect == -1 || m_nSelect == 0)
    {
        m_nSelect = 0;
    }
    else
    {
        m_nSelect-=2;
    }

    if (m_multiInlinePath[m_nSelect].Find(_T("DARK")) != -1)
    {
        m_nSelect = m_nSelect - 1;
    }
    CString strImagePath_bright;
    CString strImagePath_dark;
    strImagePath_bright = m_multiInlinePath[m_nSelect];
    strImagePath_dark = m_multiInlinePath[m_nSelect + 1];
    m_parent->GetDlgItem(IDC_BUTTON_IMAGESAVE)->SetWindowTextW(strImagePath_bright);

    IPVM::Image_8u_C1 image_bright;
    IPVM::Image_8u_C1 image_dark;
    image_bright.LoadFrom(strImagePath_bright);
    image_dark.LoadFrom(strImagePath_dark);

    m_parent->m_setupImage.m_imageBright[0] = image_bright;
    m_parent->m_setupImage.m_imageDark[0] = image_dark;

    m_parent->UpdateEditInspectionItem();
    m_parent->OnBnClickedButtonTestAll();
    //m_parent->ShowBatchInspRejectROI();
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

    CString strResult;
    if (m_parent->m_bReject == TRUE)
    {
        strResult = _T("REJECT");
    }
    else
    {
        // 여기서 블랍의 정보에 대해 결과 값을 갖고 와서 써야함...
        strResult = _T("PASS");
        if (m_parent->m_nRejectType == II_RESULT_VALUE::INVALID)
        {
            strResult = _T("INVALID");
        }
    }

    
    m_listBatchCtrl.SetItemText(m_nSelect, 3, strResult);
    m_listBatchCtrl.SetItemText(m_nSelect + 1, 3, strResult);
}

void CDlgBatchInpection::OnBnClickedButtonReset()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    m_index = 0;
    m_parent->m_nExcelRow = 0;
    for (long nCnt = 0; nCnt < m_listBatchCtrl.GetItemCount(); nCnt++)
    {
        m_listBatchCtrl.SetItemText(nCnt, 3, _T(""));
        m_listBatchCtrl.SetItemText(nCnt + 1, 3, _T(""));
    }
    
     m_listBatchCtrl.SetTextBkColor(RGB(0, 0, 0));
}

void CDlgBatchInpection::OnBnClickedButtonInspection()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

    IPVM::Image_8u_C1 brightImage, darkImage;
    //cellImage.LoadFrom(m_multiInlinePath[m_index]);
    //cellImage = image;

    brightImage.LoadFrom(m_multiInlinePath[nSel]);
    darkImage.LoadFrom(m_multiInlinePath[nSel + 1]);

    m_parent->m_setupImage.m_imageBright[0] = m_parent->m_setupImage.m_imageBright[1] = brightImage;
    m_parent->m_setupImage.m_imageDark[0] = m_parent->m_setupImage.m_imageDark[1] = darkImage;

    m_parent->UpdateEditInspectionItem();
    m_parent->OnBnClickedButtonTestAll();
    //m_parent->ShowBatchInspRejectROI();
    CString strResult;

    
    if (m_parent->m_bReject == TRUE)
    {
        strResult = _T("REJECT");
    }
    else
    {
        // 여기서 블랍의 정보에 대해 결과 값을 갖고 와서 써야함...
        strResult = _T("PASS");
        if (m_parent->m_nRejectType == II_RESULT_VALUE::INVALID)
        {
            strResult = _T("INVALID");
        }
    }

    m_listBatchCtrl.SetItemText(nSel, 3, strResult);
    m_listBatchCtrl.SetItemText(nSel + 1, 3, strResult);
}

void CDlgBatchInpection::OnBnClickedButtonExcelsavepath()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    BROWSEINFO BrInfo;
	TCHAR szBuffer[512];                                      // 경로저장 버퍼 

	::ZeroMemory(&BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, 512);

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.lpszTitle = _T("파일이 저장될 폴더를 선택하세요");
	BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);
	::SHGetPathFromIDList(pItemIdList, szBuffer);               // 파일경로 읽어오기
    //CFileDialog SaveDlg(FALSE, _T("xlsx"), _T(""), OFN_HIDEREADONLY, _T("xlsx Files (*..xlsx)|*..xlsx||"));

    //if (SaveDlg.DoModal() != IDOK)
    //{
    //    return;
    //}

    //CString strExcelSaveFilePath = SaveDlg.GetPathName();
        // 경로를 가져와 사용할 경우, Edit Control 에 값 저장
	CString str;
    //str.Format(_T("%s"), strExcelSaveFilePath);
    str.Format(_T("%s"), szBuffer);
	SetDlgItemText(IDC_EDIT_EXCELSAVEPATH, str);  
}

void CDlgBatchInpection::OnNMCustomdrawListBatch(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVCUSTOMDRAW* pNMCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
    *pResult = CDRF_DODEFAULT;
    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.

    if (CDDS_PREPAINT == pNMCD->nmcd.dwDrawStage)
    {
        *pResult = (LRESULT)CDRF_NOTIFYITEMDRAW;
        return;
    }
    else if (CDDS_ITEMPREPAINT == pNMCD->nmcd.dwDrawStage)
    {
        // This is the prepaint stage for an item. Here's where we set the
        // item's text color. Our return value will tell Windows to draw the
        // item itself, but it will use the new color we set here.
        // We'll cycle the colors through red, green, and light blue.

        COLORREF crText = RGB(0, 0, 0);
        COLORREF crBkgnd = RGB(255, 255, 255);

      

        // Store the color back in the NMLVCUSTOMDRAW struct.
        pNMCD->clrText = crText;
        pNMCD->clrTextBk = crBkgnd;
        
        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;

        //TRACE2("================ notify custom index = %d, update row = %d \r\n", m_index, pNMCD->nmcd.dwItemSpec);
    }

    *pResult = 0;
}

void CDlgBatchInpection::GetGrapSpeed()
{
    CString strGrapSpeed;
    GetDlgItemText(IDC_EDIT_GRAP_SPEED, strGrapSpeed);
    if (!strGrapSpeed.IsEmpty())
    {
        m_fGrabSpeed = _ttof(strGrapSpeed);
    }
}

void CDlgBatchInpection::ThreadStart_BatchInspection()
{
    for (long i = 0; i < m_threadControl_BatchInspection.size(); i++)
    {
        auto& control = m_threadControl_BatchInspection[i];

        control.Terminate();

        control.m_Signal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
        control.m_Signal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
        control.m_Signal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);

        HANDLE hInspThread = ::AfxBeginThread(ThreadFunc_BatchInspection, LPVOID(this))->m_hThread;
        ::DuplicateHandle(::GetCurrentProcess(), hInspThread, ::GetCurrentProcess(), &control.m_Handle_Thread,
            DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
    }

    m_threadControl_BatchSimulation.m_Signal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_threadControl_BatchSimulation.m_Signal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_threadControl_BatchSimulation.m_Signal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);

    HANDLE hInspResUpdateThread = ::AfxBeginThread(ThreadFunc_BatchSimulation, LPVOID(this))->m_hThread;
    ::DuplicateHandle(::GetCurrentProcess(), hInspResUpdateThread, ::GetCurrentProcess(),
        &m_threadControl_BatchSimulation.m_Handle_Thread, DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);

    ResetEvent(m_threadWaitInspEnd);
}

void CDlgBatchInpection::ThreadStop_BatchInspection()
{
    // 스레드 컨트롤 삭제
    for (long i = 0; i < m_threadControl_BatchInspection.size(); i++)
    {
        auto& control = m_threadControl_BatchInspection[i];
        control.Terminate();
    }

    m_threadControl_BatchSimulation.Terminate();
}

UINT CDlgBatchInpection::ThreadFunc_BatchInspection(LPVOID pParam)
{
    CDlgBatchInpection* pDlg = (CDlgBatchInpection*)pParam;
    return pDlg->ThreadFunc_BatchInspection();
}

UINT CDlgBatchInpection::ThreadFunc_BatchInspection()
{
    static CCriticalSection cs;
    cs.Lock();
    static long nLatestThreadID = 0;
    const long nThreadID = nLatestThreadID++;

    auto& control = m_threadControl_BatchInspection[nThreadID];
    auto& inspectionCell = m_batchInspectionCellGroup[nThreadID];
    cs.Unlock();

    ::SetEvent(control.m_Signal_Ready);

    HANDLE pHandles[] = {control.m_Signal_Kill, control.m_Signal_Start};
    const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);

    while (1)
    {
        DWORD dwRet = WaitForMultipleObjects(nSignalCount, pHandles, FALSE, INFINITE);
        if ((WAIT_OBJECT_0 + 1) == dwRet)
        {
            IPVM::TimeCheck timeCheck, InspTime;
            timeCheck.Reset();
            float insptime = 0.;
            CString strUp = _T("Down");
            if (inspectionCell.m_isUp)
            {
                strUp = _T("Up");
            }

            bool isInspectionOK = false;

            InspTime.Reset();
            //isInspectionOK = inspectionCell.DoInspection();
            m_parent->m_setupImage.m_imageBright[0] = m_parent->m_setupImage.m_imageBright[1] = inspectionCell.m_inspectionImageBright;
            m_parent->m_setupImage.m_imageDark[0] = m_parent->m_setupImage.m_imageDark[1] = inspectionCell.m_inspectionImageDark;
            m_parent->SendMessage(UWM_UPDATE_EDIT_INSPECTIONITEM);
            insptime = InspTime.Elapsed_ms();

            CString strResult = _T("");
            if (m_parent->m_bReject == TRUE)
            {
                strResult = _T("REJECT");
            }
            else
            {
                strResult = _T("PASS");
                if (m_parent->m_nRejectType == II_RESULT_VALUE::INVALID)
                {
                    strResult = _T("INVALID");
                }
            }
            
            m_listBatchCtrl.SetItemText(inspectionCell.m_cellID, 3, strResult);
            m_listBatchCtrl.SetItemText(inspectionCell.m_cellID + 1, 3, strResult);

            for (long nIndex = 0; nIndex < m_listBatchCtrl.GetItemCount(); nIndex++)
            {
                m_listBatchCtrl.SetItemState(nIndex, ~LVIS_SELECTED, LVIS_SELECTED);
            }

            //항목 선택
            m_listBatchCtrl.SetItemState(inspectionCell.m_cellID, LVIS_SELECTED, LVIS_SELECTED);
            m_listBatchCtrl.SetItemState(inspectionCell.m_cellID + 1, LVIS_SELECTED, LVIS_SELECTED);

            //선택한 항목으로 이동
            m_listBatchCtrl.EnsureVisible(inspectionCell.m_cellID, TRUE);
            m_listBatchCtrl.SetFocus();

            if (m_chk_ExcelSave.GetCheck() == TRUE)
            {
                m_parent->ExcelRejectDataInput();
                m_parent->ExcelFileSave();
            }
            
            ::SetEvent(control.m_Signal_Ready);
        }
        else if (WAIT_OBJECT_0 == dwRet)
        {
            SetEvent(control.m_Handle_Thread);
            break;
        }
    }

    return 0;
}

UINT CDlgBatchInpection::ThreadFunc_BatchSimulation(LPVOID pParam)
{
    CDlgBatchInpection* pDlg = (CDlgBatchInpection*)pParam;
    return pDlg->ThreadFunc_BatchSimulation();
}

UINT CDlgBatchInpection::ThreadFunc_BatchSimulation()
{
    auto& control = m_threadControl_BatchSimulation;
    HANDLE pHandles[] = {control.m_Signal_Kill, control.m_Signal_Start};
    const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);

    const long threadCount = long(m_batchInspectionCellGroup.size());
    //const DWORD handleCount = threadCount + 1;
    const DWORD handleCount = threadCount;

    std::vector<HANDLE> vecSubHandles(handleCount);
    HANDLE* subHandles = &vecSubHandles[0];
    //subHandles[0] = m_eventCancelWaitingInspection;

    for (long idx = 0; idx < handleCount; idx++)
    {
        subHandles[idx] = m_threadControl_BatchInspection[idx].m_Signal_Ready;
    }

    while (1)
    {
        DWORD dwRet = WaitForMultipleObjects(nSignalCount, pHandles, FALSE, INFINITE);
        if (WAIT_OBJECT_0 + 1 == dwRet) // signal Start
        {
            m_stopBatchInsp = FALSE;
            CString strID, strLog;

             IPVM::TimeCheck tm;
            float fTime = 0.f;
            if (m_index < (m_multiInlinePath.size() - 1))
            {
                long cntRoof = m_multiInlinePath.size() / 2;
                //for (long idx = 0; idx < cntRoof; idx++)
                long cntRoof20;
                if (cntRoof > 20)
                for (long idx = 0; idx < cntRoof; idx++)
                {


                   
                    tm.Reset();

                    IPVM::Image_8u_C1 cellImage, brightImage, darkImage;
                    cellImage.LoadFrom(m_multiInlinePath[m_index]);
                    brightImage.LoadFrom(m_multiInlinePath[m_index]);
                    darkImage.LoadFrom(m_multiInlinePath[m_index + 1]);

                    DWORD dwSubRet = ::WaitForMultipleObjects(handleCount, subHandles, FALSE, INFINITE);

                    const long threadIndex = dwSubRet - WAIT_OBJECT_0;
                    if (threadIndex < 0)
                    {
                        return -1;
                    }

                    auto& inspectionCell = m_batchInspectionCellGroup[threadIndex];

                    inspectionCell.m_isUp = true; //Top/Back 인지 선택하는거 만들어서 넣어도 됨
                    inspectionCell.m_cellResult.m_imageSavePath = m_multiInlinePath[m_index];
                    inspectionCell.m_inspectionImageBright = brightImage;
                    inspectionCell.m_inspectionImageDark = darkImage;
                    inspectionCell.m_cellID = m_index;

                    fTime = tm.Elapsed_ms();
                    strLog.Format(_T("===[RoofNum = %d][Image Open Time = %.3f ms][nThreadID = %d] \r\n"), m_index,
                        fTime, threadIndex);
                    TRACE(strLog);

                    SetEvent(m_threadControl_BatchInspection[threadIndex].m_Signal_Start);
                    m_index += 2;

                    if (m_index == m_multiInlinePath.size())
                    {
                        CString strPath;
                        GetDlgItemText(IDC_EDIT_EXCELSAVEPATH, strPath);
                        if (m_chk_ExcelSave.GetCheck() == TRUE)
                        {
                            m_parent->ExcelFileSave(strPath);
                        }
                        ::DeleteFile(_T("D:\\iBCI\\Result\\temp.xlsx"));
                    }

                    //여기서 입력한 시간까지 기다려 그리고 다시 돌아
                    //DWORD dwRet = WaitForSingleObject(m_threadWaitInspEnd, m_fGrabSpeed);
                }

                if (m_stopBatchInsp)
                {
                    break;
                }
            }
        }
        else if (WAIT_OBJECT_0 == dwRet)
        {
            SetEvent(control.m_Handle_Thread);
            break;
        }
    }
    return 0;
}

//void CDlgBatchInpection::ThreadStart_BatchInspection()
//{
//    // 스레드 컨트롤 생성
//      m_threadControl_BatchInspection.Terminate();
//
//      m_threadControl_BatchInspection.m_Signal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
//      m_threadControl_BatchInspection.m_Signal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
//      m_threadControl_BatchInspection.m_Signal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);
//
//      HANDLE hInspThread = ::AfxBeginThread(ThreadFunc_BatchInspection, LPVOID(this))->m_hThread;
//    ::DuplicateHandle(::GetCurrentProcess(), hInspThread, ::GetCurrentProcess(), &m_threadControl_BatchInspection.m_Handle_Thread, DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
//}

//void CDlgBatchInpection::ThreadStop_BatchInspection()
//{
//    // 스레드 컨트롤 삭제
//    m_threadControl_BatchInspection.Terminate();
//}

//UINT CDlgBatchInpection::ThreadFunc_BatchInspection()
//{
//    auto& control = m_threadControl_BatchInspection;
//    HANDLE pHandles[] = {control.m_Signal_Kill, control.m_Signal_Start};
//    const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);
//
//    while(1)
//    {
//        DWORD dwRet = WaitForMultipleObjects(nSignalCount, pHandles, FALSE, INFINITE);
//        if (WAIT_OBJECT_0 + 1 == dwRet) // signal Start
//        {
//            // OnTimer에서 수행하는 것들
//            m_stopBatchInsp = FALSE;
//            CString strID;
//            if (m_index < m_multiInlinePath.size())
//            {
//                long cntRoof = m_multiInlinePath.size() / 2;
//                for (long idx = 0; idx < cntRoof; idx++)
//                {
//                    //CSingleLock lock(&m_csBatch, TRUE);
//                    ResetEvent(m_threadWaitInspEnd);
//                    if (m_stopBatchInsp)
//                    {
//                        break;
//                    }
//
//                    IPVM::Image_8u_C1 cellImage, brightImage, darkImage;
//                    cellImage.LoadFrom(m_multiInlinePath[m_index]);
//                    brightImage.LoadFrom(m_multiInlinePath[m_index]);
//                    darkImage.LoadFrom(m_multiInlinePath[m_index + 1]);
//
//                    m_parent->m_setupImage.m_imageBright[0] = m_parent->m_setupImage.m_imageBright[1] = brightImage;
//                    m_parent->m_setupImage.m_imageDark[0] = m_parent->m_setupImage.m_imageDark[1] = darkImage;
//
//                    m_parent->PostMessage(UWM_UPDATE_EDIT_INSPECTIONITEM);
//
//                    CString strResult;
//                    //여기서 입력한 시간까지 기다려 그리고 풀려
//                    DWORD dwRet = WaitForSingleObject(m_threadWaitInspEnd, m_fGrabSpeed);
//
//                    if (dwRet == WAIT_OBJECT_0)
//                    {
//                        if (m_parent->m_bReject == TRUE)
//                        {
//                            strResult = _T("REJECT");
//                        }
//                        else
//                        {
//                            strResult = _T("PASS");
//                        }
//                    }
//                    else
//                    {
//                        strResult = _T("TIME OUT");
//                        dwRet = WaitForSingleObject(m_threadWaitInspEnd, INFINITE);
//                    }
//
//                    m_listBatchCtrl.SetItemText(m_index, 3, strResult);
//                    m_listBatchCtrl.SetItemText(m_index + 1, 3, strResult);
//
//                    for (long nIndex = 0; nIndex < m_listBatchCtrl.GetItemCount(); nIndex++)
//                    {
//                        m_listBatchCtrl.SetItemState(nIndex, ~LVIS_SELECTED, LVIS_SELECTED);
//                    }
//
//                    //항목 선택
//                    m_listBatchCtrl.SetItemState(m_index, LVIS_SELECTED, LVIS_SELECTED);
//                    m_listBatchCtrl.SetItemState(m_index + 1, LVIS_SELECTED, LVIS_SELECTED);
//
//                    //선택한 항목으로 이동
//                    m_listBatchCtrl.EnsureVisible(m_index, TRUE);
//                    m_listBatchCtrl.SetFocus();
//
//                    m_index += 2;
//
//                    if (m_chk_ExcelSave.GetCheck() == TRUE)
//                    {
//                        m_parent->ExcelRejectDataInput();
//                        m_parent->ExcelFileSave();
//                    }
//
//                    if (m_index == m_multiInlinePath.size())
//                    {
//                        CString strPath;
//                        GetDlgItemText(IDC_EDIT_EXCELSAVEPATH, strPath);
//                        if (m_chk_ExcelSave.GetCheck() == TRUE)
//                        {
//                            m_parent->ExcelFileSave(strPath);
//                        }
//                        ::DeleteFile(_T("D:\\iBCI\\Result\\temp.xlsx"));
//                    }
//                }
//            }
//        }
//        else if (WAIT_OBJECT_0 == dwRet) // signal Kill
//        {
//            SetEvent(control.m_Handle_Thread);
//            break;
//        }
//    }
//    return 0;
//}



BOOL CDlgBatchInpection::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
        {
            return TRUE;
        }
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgBatchInpection::ControlEnable(BOOL bEnable)
{
    GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_PREVIOUS)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_INSPECTION)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(bEnable);
}