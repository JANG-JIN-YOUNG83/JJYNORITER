
// MainDlgInspection.cpp : 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "IBCIDlg.h"
//#include "InlineResultCoupler.h"
#include "SurfaceInspect.h"

#include "../Inspection/Accessor.h"

//220817_sie
#include "../SharedModule/VisionHostShare/HostCommSocket.h"	
#include "../SharedModule/VisionHostShare/ResultVision.h"	
#include "DlgAnimationWindow.h"
#include "DlgGraphWindow.h"
#include "Gadget/TimeCheck.h"
#include "../Grab/CISComm.h"
#include "../Grab/GrabExecutor.h"
#include "Gadget/Miscellaneous.h"
#include "DlgDefectCount.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "Path.h"


void CIBCIDlg::ThreadStart_Result()
{
	m_threadControl_Result.Terminate();

	m_threadControl_Result.m_Signal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_threadControl_Result.m_Signal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_threadControl_Result.m_Signal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);

	HANDLE hInspThread = ::AfxBeginThread(ThreadFunc_Result, LPVOID(this))->m_hThread;
	::DuplicateHandle(::GetCurrentProcess(), hInspThread, ::GetCurrentProcess(), &m_threadControl_Result.m_Handle_Thread, DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
}

UINT CIBCIDlg::ThreadFunc_Result(LPVOID pParam)
{
	return ((CIBCIDlg*)pParam)->ThreadFunc_Result();
}

UINT CIBCIDlg::ThreadFunc_Result()
{
	::SetEvent(m_threadControl_Result.m_Signal_Ready);

	HANDLE pHandles[] = { m_threadControl_Result.m_Signal_Kill, m_threadControl_Result.m_Signal_Start };
	const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);

	IPVM::Image_8u_C1 upImageSmallBright, downImageSmallBright, upImageSmallDark, downImageSmallDark;

	std::vector<DefectType> vecTotalNG;
	while (1)
	{
        DWORD dwRet = ::WaitForMultipleObjects(nSignalCount, pHandles, FALSE, INFINITE);
        
        if ((WAIT_OBJECT_0) == dwRet)
        {
            break;
        }
        else if ((WAIT_OBJECT_0 + 1) == dwRet)
        {
            CString topBack;

            topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");

            ResultMismatch stResAllMismatch;
            Share::ResultVision shResultVision;
            {
                CSingleLock lock(&m_csResult, TRUE);
                stResAllMismatch = m_vecMismatchData.front();
                m_vecMismatchData.pop_front();
                //if (topBack == _T("BACK"))
                //{
                //    m_vecResultVisionData.clear();
                //    shResultVision.Clear();
                //}
                //else
                {
                    shResultVision = m_vecResultVisionData.front();
                    m_vecResultVisionData.erase(m_vecResultVisionData.begin());
                }
            }

            if (stResAllMismatch.m_vecResUpLane.size() == stResAllMismatch.m_vecResDownLane.size())
            {
                if (stResAllMismatch.m_vecResUpLane.size() == 2)
                {
                    float fMismatch;
                    fMismatch = fabs(stResAllMismatch.m_vecResUpLane[0].fLaneWidth
                        - stResAllMismatch.m_vecResDownLane[0].fLaneWidth);
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);
                    fMismatch = fabs(stResAllMismatch.m_vecResUpLane[1].fLaneWidth
                        - stResAllMismatch.m_vecResDownLane[1].fLaneWidth);
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);
                }
                else if (stResAllMismatch.m_vecResUpLane.size() == 3)
                {
                    float fMismatch;
                    fMismatch = fabs(stResAllMismatch.m_vecResUpLane[0].fLaneWidth
                        - stResAllMismatch.m_vecResDownLane[0].fLaneWidth);
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);
                    fMismatch = fabs(
                        (stResAllMismatch.m_vecResUpLane[0].fLaneWidth + stResAllMismatch.m_vecResUpLane[1].fLaneWidth)
                        - (stResAllMismatch.m_vecResDownLane[0].fLaneWidth
                            + stResAllMismatch.m_vecResDownLane[1].fLaneWidth));
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);

                    fMismatch = fabs(stResAllMismatch.m_vecResUpLane[2].fLaneWidth
                        - stResAllMismatch.m_vecResDownLane[2].fLaneWidth);
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);
                }
                else if (stResAllMismatch.m_vecResUpLane.size() == 4)
                {
                    float fMismatch;
                    fMismatch = fabs(stResAllMismatch.m_vecResUpLane[0].fLaneWidth
                        - stResAllMismatch.m_vecResDownLane[0].fLaneWidth);
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);
                    fMismatch = fabs(
                        (stResAllMismatch.m_vecResUpLane[0].fLaneWidth + stResAllMismatch.m_vecResUpLane[1].fLaneWidth)
                        - (stResAllMismatch.m_vecResDownLane[0].fLaneWidth
                            + stResAllMismatch.m_vecResDownLane[1].fLaneWidth));
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);

                    fMismatch = fabs(
                        (stResAllMismatch.m_vecResUpLane[3].fLaneWidth + stResAllMismatch.m_vecResUpLane[2].fLaneWidth)
                        - (stResAllMismatch.m_vecResDownLane[3].fLaneWidth
                            + stResAllMismatch.m_vecResDownLane[2].fLaneWidth));
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);

                    fMismatch = fabs(stResAllMismatch.m_vecResUpLane[3].fLaneWidth
                        - stResAllMismatch.m_vecResDownLane[3].fLaneWidth);
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);
                }
                else if (stResAllMismatch.m_vecResUpLane.size() == 5)
                {
                    float fMismatch;
                    fMismatch = fabs(stResAllMismatch.m_vecResUpLane[0].fLaneWidth
                        - stResAllMismatch.m_vecResDownLane[0].fLaneWidth);
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);
                    fMismatch = fabs(
                        (stResAllMismatch.m_vecResUpLane[0].fLaneWidth + stResAllMismatch.m_vecResUpLane[1].fLaneWidth)
                        - (stResAllMismatch.m_vecResDownLane[0].fLaneWidth
                            + stResAllMismatch.m_vecResDownLane[1].fLaneWidth));
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);

                    fMismatch = fabs(
                        (stResAllMismatch.m_vecResUpLane[3].fLaneWidth + stResAllMismatch.m_vecResUpLane[2].fLaneWidth)
                        - (stResAllMismatch.m_vecResDownLane[3].fLaneWidth
                            + stResAllMismatch.m_vecResDownLane[2].fLaneWidth));
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);

                    fMismatch = fabs(stResAllMismatch.m_vecResUpLane[3].fLaneWidth
                        - stResAllMismatch.m_vecResDownLane[3].fLaneWidth);
                    stResAllMismatch.m_vecfDiffWidth.push_back(fMismatch);
                }
                // mismatch 저장
                if (g_systemParameter.bMismatchSave == TRUE)
                {
                    // Measure와 같은 폴더에 저장?
                    // 현재 검사 시간 or 검사할 이미지의 원본 시간
                    SYSTEMTIME CurTime;
                    GetLocalTime(&CurTime);
                    m_strInspTime;
                    CString strYear = m_strInspTime.Left(4);
                    CString strMonth = m_strInspTime.Mid(4, 2);
                    CString strDay = m_strInspTime.Mid(6, 2);
                    CString strHour = m_strInspTime.Mid(8, 2);
                    CString strMinute = m_strInspTime.Mid(10, 2);
                    CString strSecond = m_strInspTime.Mid(12, 2);

                    CString strCsvFolderPath, strCsvNoCellIDPath;
                    if (!g_systemParameter.pathResultDataCsv.IsEmpty())
                    {
                        CString lotId = _T("LOT");
                        if (!stResAllMismatch.m_strLotID.IsEmpty())
                            lotId.Format(_T("%s"), stResAllMismatch.m_strLotID);
                        CString topBack;
                        topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");

                        //strCsvFolderPath.Format(_T("%s\\%04d_%02d\\%02d\\%02d\\%s\\Data\\%10d"),
                        //    g_systemParameter.pathResultDataCsv, CurTime.wYear, CurTime.wMonth, CurTime.wDay,
                        //    CurTime.wHour, topBack, stResAllMismatch.m_nCellIndex);


                        strCsvFolderPath.Format(_T("%s\\%s\\%s\\Data\\%09d"),
                            g_systemParameter.pathResultDataCsv, m_strFolderTime, topBack,
                            stResAllMismatch.m_nCellIndex);

                        strCsvNoCellIDPath.Format(_T("%s\\%04d_%02d\\%02d\\%02d\\%s\\Data"),
                            g_systemParameter.pathResultDataCsv, CurTime.wYear, CurTime.wMonth, CurTime.wDay,
                            CurTime.wHour, topBack);

                        strCsvNoCellIDPath.Format(_T("%s\\%s\\%s\\Data"), g_systemParameter.pathResultDataCsv, m_strFolderTime, topBack);

                        IPVM::CreateDirectories(strCsvNoCellIDPath);
                    }
                    else
                    {
                        CString lotId = _T("LOT");
                        if (!stResAllMismatch.m_strLotID.IsEmpty())
                            lotId.Format(_T("%s"), stResAllMismatch.m_strLotID);
                        CString topBack;
                        topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");

                        strCsvFolderPath.Format(_T("D:\\result\\%04d_%02d\\%02d\\%02d\\%s\\Data\\%10d"), CurTime.wYear,
                            CurTime.wMonth, CurTime.wDay, CurTime.wHour, topBack, stResAllMismatch.m_nCellIndex);

                        strCsvNoCellIDPath.Format(_T("D:\\result\\%04d_%02d\\%02d\\%02d\\%s\\Data"), CurTime.wYear,
                            CurTime.wMonth, CurTime.wDay, CurTime.wHour, topBack);
                        IPVM::CreateDirectories(strCsvNoCellIDPath);
                    }

                    CString strMismatchPath;
                    strMismatchPath.Format(_T("%s_mismatch.csv"), strCsvFolderPath);
                    SaveCsvMismatch(stResAllMismatch, strMismatchPath);
                    
                }
                //if (topBack == _T("TOP"))
                {
                    shResultVision.m_vecMismatch.resize(stResAllMismatch.m_vecfDiffWidth.size());
                    for (long nMismatch = 0; nMismatch < shResultVision.m_vecMismatch.size(); nMismatch++)
                    {
                        shResultVision.m_vecMismatch[nMismatch].m_dValue = stResAllMismatch.m_vecfDiffWidth[nMismatch];

                        /*double fRand = 0.0f;
                        {
                            fRand = ((rand() % 100));
                            fRand = fRand / 100;
                        }
                        shResultVision.m_vecMismatch[nMismatch].m_dValue
                            = stResAllMismatch.m_vecfDiffWidth[nMismatch] + fRand;*/
                    }
                    
                    //shResultVision.m_vecMismatch[1].m_dValue = stResAllMismatch.m_vecfDiffWidth[1];
                    //shResultVision.m_vecMismatch[2].m_dValue = stResAllMismatch.m_vecfDiffWidth[2];
                    //shResultVision.m_vecMismatch[3].m_dValue = stResAllMismatch.m_vecfDiffWidth[3];

                    SendHostData(shResultVision);
                   // m_dlgBatch->m_vecResultVision.push_back(shResultVision);
                }


                shResultVision.Clear();
            }
            m_stResAllMismatchData = stResAllMismatch;


            
            //SetAnimationParameter
            if (m_pDlgAnimationWindow && MachineType::ES_rollpress_pinhole != g_systemParameter.machineType)
            {
                if (g_systemParameter.bDisplayShow == TRUE)
                {
                    m_pDlgAnimationWindow->SetAnimationParameter(stResAllMismatch, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        g_systemParameter.nLaneCount, g_systemParameter.bOSDS, g_systemParameter.bRollerOS);
                }
            }

            if (m_pDlgGraphWindow)
            {
                if (g_systemParameter.bDisplayShow == TRUE)
                {
                    if (stResAllMismatch.m_vecfDiffWidth.size() == 2)
                    {
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(TRUE, DefectType::Dimension_Mismatch1,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[0], true, _T("OK"));

                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(TRUE, DefectType::Dimension_Mismatch2,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[1], true, _T("OK"));
                    }
                    if (stResAllMismatch.m_vecfDiffWidth.size() == 3)
                    {
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(TRUE, DefectType::Dimension_Mismatch1,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[0], true, _T("OK"));

                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(TRUE, DefectType::Dimension_Mismatch2,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[1], true, _T("OK"));
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[2]->SetData(TRUE, DefectType::Dimension_Mismatch3,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[2], true, _T("OK"));
                    }
                    if (stResAllMismatch.m_vecfDiffWidth.size() == 4)
                    {
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(TRUE, DefectType::Dimension_Mismatch1,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[0], true, _T("OK"));

                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(TRUE, DefectType::Dimension_Mismatch2,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[1], true, _T("OK"));
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[2]->SetData(TRUE, DefectType::Dimension_Mismatch3,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[2], true, _T("OK"));

                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[3]->SetData(TRUE, DefectType::Dimension_Mismatch4,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[3], true, _T("OK"));
                    }
                    if (stResAllMismatch.m_vecfDiffWidth.size() == 5)
                    {
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(TRUE, DefectType::Dimension_Mismatch1,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[0], true, _T("OK"));

                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(TRUE, DefectType::Dimension_Mismatch2,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[1], true, _T("OK"));
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[2]->SetData(TRUE, DefectType::Dimension_Mismatch3,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[2], true, _T("OK"));

                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[3]->SetData(TRUE, DefectType::Dimension_Mismatch4,
                            stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth[3], true, _T("OK"));
                    }
                }

#if INSPECTION_MACHINE_UC1
#else
                for (long nCount = 0; nCount < m_dlgDefectCount->m_vecMismatchData.size(); nCount++)
                {
                    if (m_dlgDefectCount->m_vecMismatchData[nCount].m_strName
                        == GetDefectName(DefectType::Width_NonCoating_1))
                    {
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fTopValue
                            = stResAllMismatch.m_vecResUpLane[0].fLaneWidth;
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fBottomValue
                            = stResAllMismatch.m_vecResDownLane[0].fLaneWidth;
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fMismatchValue
                            = stResAllMismatch.m_vecfDiffWidth[0];
                    }
                    if (m_dlgDefectCount->m_vecMismatchData[nCount].m_strName
                        == GetDefectName(DefectType::Width_Coating_1))
                    {
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fTopValue
                            = stResAllMismatch.m_vecResUpLane[1].fLaneWidth;
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fBottomValue
                            = stResAllMismatch.m_vecResDownLane[1].fLaneWidth;
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fMismatchValue
                            = stResAllMismatch.m_vecfDiffWidth[1];
                    }

                    if (stResAllMismatch.m_vecResUpLane.size() == 4
                        && m_dlgDefectCount->m_vecMismatchData[nCount].m_strName
                            == GetDefectName(DefectType::Width_Coating_2))
                    {
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fTopValue
                            = stResAllMismatch.m_vecResUpLane[2].fLaneWidth;
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fBottomValue
                            = stResAllMismatch.m_vecResDownLane[2].fLaneWidth;
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fMismatchValue
                            = stResAllMismatch.m_vecfDiffWidth[2];
                    }

                    if (m_dlgDefectCount->m_vecMismatchData[nCount].m_strName
                        == GetDefectName(DefectType::Width_NonCoating_2))
                    {
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fTopValue
                            = stResAllMismatch.m_vecResUpLane[(stResAllMismatch.m_vecResUpLane.size() == 4) ? 3 : 2]
                                  .fLaneWidth;
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fBottomValue
                            = stResAllMismatch.m_vecResDownLane[(stResAllMismatch.m_vecResUpLane.size() == 4) ? 3 : 2]
                                  .fLaneWidth;
                        m_dlgDefectCount->m_vecMismatchData[nCount].m_fMismatchValue
                            = stResAllMismatch.m_vecfDiffWidth[(stResAllMismatch.m_vecResUpLane.size() == 4) ? 3 : 2];
                    }
                }
                m_dlgDefectCount->DefectMismatchDraw();
#endif // INSPECTION_MACHINE_UC1

                
                // ExportMismatchGraphData(stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth);

                m_vecCellIndex.push_back(stResAllMismatch.m_nCellIndex);
                m_vecvecDiffWidth.push_back(stResAllMismatch.m_vecfDiffWidth);
                // inline으로 돌릴 때
                if (m_vecCellIndex.size() >= 68 && m_vecvecDiffWidth.size() >= 68)
                {
                    m_vecCellIndex.erase(m_vecCellIndex.begin());
                    m_vecvecDiffWidth.erase(m_vecvecDiffWidth.begin());

                    CString filePath;
                    //filePath = _T("C:\\Intekplus\\IBCI\\backup_UI\\MismatchGraphData.csv");
                    filePath.Format(_T("%s\\MismatchGraphData.csv"), Path::GetBackupUIFolder());
                    CFileFind find;
                    if (find.FindFile(filePath))
                    {
                        DeleteFile(filePath);
                    }
                    for (long n = 0; n < m_vecCellIndex.size(); n++)
                    {
                        ExportMismatchGraphData(m_vecCellIndex[n], m_vecvecDiffWidth[n]);
                    }
                }
                else
                {
                    ExportMismatchGraphData(stResAllMismatch.m_nCellIndex, stResAllMismatch.m_vecfDiffWidth);
                }
                // Animation SetData
            }
            // ----[end] DisplayMismatchData ----

            ::SetEvent(m_threadControl_Result.m_Signal_Ready);
		}
	}
    return 0;
}
