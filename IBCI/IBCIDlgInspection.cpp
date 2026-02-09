
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
#include "DlgAnimationWindow.h"
#include "DlgGraphWindow.h"
#include "Gadget/TimeCheck.h"
#include "InlineResultCoupler.h"
#include "../Grab/CISComm.h"
#include "CellImageSaver.h"
#include "CISIViewerProperty.h"
#include "InlineResultUiUpdater.h"
#include "../Grab/GrabExecutor.h"
#include "Gadget/Miscellaneous.h"

#include "DlgViewCellImage.h"
#include "ViewCellImageUpdater.h"
#include "..\SharedModule\VisionHostShare\ResultVision.h"
#include <pdh.h>
#include <windows.h>
#pragma comment(lib, "pdh.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class CCPUUsage
{
public:
    CCPUUsage()
        : query(NULL)
        , counter(NULL)
    {
        // 성능 카운터 초기화
        PdhOpenQuery(NULL, NULL, &query);
        PdhAddCounter(query, TEXT("\\Processor(_Total)\\% Processor Time"), NULL, &counter);
        PdhCollectQueryData(query);
    }

    ~CCPUUsage()
    {
        PdhCloseQuery(query);
    }

    double GetCPUUsage()
    {
        PDH_FMT_COUNTERVALUE counterVal;

        // 성능 데이터를 수집
        PdhCollectQueryData(query);

        // 성능 데이터를 가져와서 포맷팅
        PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterVal);

        return counterVal.doubleValue;
    }

private:
    PDH_HQUERY query;
    PDH_HCOUNTER counter;
};
void CIBCIDlg::ThreadStart_InlineInspection()
{
	for (long i = 0; i < m_threadControl_InlineInspection.size(); i++)
	{
		auto &control = m_threadControl_InlineInspection[i];

		control.Terminate();

		control.m_Signal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
		control.m_Signal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
		control.m_Signal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);

        if(g_systemParameter.machineType == MachineType::UC1_mismatch)
        {
            HANDLE hInspThread = ::AfxBeginThread(ThreadFunc_InlineInspection_UC1, LPVOID(this))->m_hThread;
            ::DuplicateHandle(::GetCurrentProcess(), hInspThread, ::GetCurrentProcess(), &control.m_Handle_Thread,
                DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
        }
        else
        {
            HANDLE hInspThread = ::AfxBeginThread(ThreadFunc_InlineInspection, LPVOID(this))->m_hThread;
            ::DuplicateHandle(::GetCurrentProcess(), hInspThread, ::GetCurrentProcess(), &control.m_Handle_Thread,
                DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
        }

	}

    m_threadControl_InlineInspectionResultUpdate.m_Signal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_threadControl_InlineInspectionResultUpdate.m_Signal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_threadControl_InlineInspectionResultUpdate.m_Signal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);

    HANDLE hInspResUpdateThread = ::AfxBeginThread(ThreadFunc_InlineInspectionResultUpdate, LPVOID(this))->m_hThread;
    ::DuplicateHandle(::GetCurrentProcess(), hInspResUpdateThread, ::GetCurrentProcess(), &m_threadControl_InlineInspectionResultUpdate.m_Handle_Thread, DUPLICATE_SAME_ACCESS,
        FALSE,
        DUPLICATE_SAME_ACCESS);
}

void CIBCIDlg::ThreadStop_InlineInspeciton()
{
    for (long i = 0; i < m_threadControl_InlineInspection.size(); i++)
    {
        auto& control = m_threadControl_InlineInspection[i];
        control.Terminate();
    }

    m_threadControl_InlineInspectionResultUpdate.Terminate();
}

void CIBCIDlg::CallbackOnCellImageDispatched(void* callbackData, InspectionInputData& inputData)
{
	return ((CIBCIDlg*)callbackData)->CallbackOnCellImageDispatched(inputData);
}

void CIBCIDlg::CallbackOnCellImageDispatched(InspectionInputData& inputData)
{
	////cell id
	//if (inputData.m_isUp)
	//	m_cell_track[0].push_data(inputData.m_cellID, inputData.m_imagePos, _T(""));

	//220817_sie
	IPVM::TimeCheck tcCompare;
	bool bError_Bright = false, bError_Dark = false;
	float fCompare = tcCompare.Elapsed_ms();

	//if (g_systemParameter.nPC_ID == 2)
	//{
	//	CallbackOnCellImageDispatched_Surface(inputData);
	//}
	//else
	{
		CallbackOnCellImageDispatched_Dimension(inputData);
	}
}

void CIBCIDlg::CallbackOnCellImageDispatched_Dimension(InspectionInputData& inputData)
{
	const long threadCount = long(m_InspectionCellGroup.size());
	const DWORD handleCount = threadCount + 1;

	std::vector<HANDLE> vecSubHandles(handleCount);
	HANDLE* subHandles = &vecSubHandles[0];
	subHandles[0] = m_eventCancelWaitingInspection;

	for (long idx = 1; idx < handleCount; idx++)
	{
		subHandles[idx] = m_threadControl_InlineInspection[idx - 1].m_Signal_Ready;
	}

	DWORD dwSubRet = ::WaitForMultipleObjects(handleCount, subHandles, FALSE, INFINITE);

	const long threadIndex = dwSubRet - WAIT_OBJECT_0 - 1;
	if (threadIndex < 0) 
		return;

	//if (!inputData.m_isUp)
	//{
	//	m_inlineResultCoupler->SetDownCellID(inputData.m_cellID);
	//	if (Share::GenerationVersion::SplitterRemoved == g_systemParameter.nGenerationVersion)
	//		m_inlineResultCoupler->SetDownCellID_Surface(inputData.m_cellID);
	//}

	auto& inspectionCell = m_InspectionCellGroup[threadIndex];
    
    if(m_cisComm->GetExecutorType() == ExecutorType::Euresys)
    {
        inspectionCell.m_cellResult.m_imageSavePath = inputData.m_imageSavePath;
        inspectionCell.m_inspectionImageFull = inputData.m_imageFull;
        inspectionCell.SetInspectionSplitImage(inputData.m_imageBright, inputData.m_imageDark);
        inspectionCell.m_nSide = inputData.m_nSide;
        //{{ 검사항목, 검사스펙 및 Parameter
        auto& inspectionRoot = Inspection::Accessor::GetRoot();
        inspectionCell.CopyDataBase(inspectionRoot.GetInspectionCell(inputData.m_isUp));
        CInspectionMeasure* mismatch = (CInspectionMeasure*)inspectionRoot.GetProcessingItem(
            true, ProcItemCategory::measure); // mismatch 스펙은 상부에만 있고 따로 취급해야 함
        if (mismatch != nullptr)
        {
            inspectionCell.GetProcessingItem(ProcItemCategory::measure)->CopyDataBase(mismatch);
        }
        //카메라 별로, 각각의 px per um 값으로 초기화, dx, dy값은 레시피 등 공간 만들고 적용하기
        if (inspectionCell.m_nSide == 0)
        {
            double dx = 19.;
            double dy = 19.;
            inspectionCell.SetPixelToUmX(dx);
            inspectionCell.SetPixelToUmY(dy);
        }
        else if (inspectionCell.m_nSide == 1)
        {
            double dx = 19.;
            double dy = 19.;
            inspectionCell.SetPixelToUmX(dx);
            inspectionCell.SetPixelToUmY(dy);
        }
        else if (inspectionCell.m_nSide == 2)
        {
            double dx = 19.;
            double dy = 19.;
            inspectionCell.SetPixelToUmX(dx);
            inspectionCell.SetPixelToUmY(dy);
        }
        else
        {
            double dx = 19.;
            double dy = 19.;
            inspectionCell.SetPixelToUmX(dx);
            inspectionCell.SetPixelToUmY(dy);
        }
        //inspectionCell.m_invaild = inputData.m_invalid;
        //inspectionCell.m_cellTriggerCnt = inputData.m_CellTriggerCount;
        inspectionCell.m_cellID = inputData.m_cellID;
        inspectionCell.m_imagePosition = inputData.m_imagePos;
        inspectionCell.m_grabTime_ms = inputData.m_grabTime_ms;
        inspectionCell.m_inputTime = inputData.m_inputTime;
        inspectionCell.SetUpDown(inputData.m_isUp);
        inspectionCell.m_nAnodeCathode = g_systemParameter.nAnodeCathode;
        //inspectionCell.m_nInsulateType = g_systemParameter.nInsulateType;

        inspectionCell.Initialize();
        inspectionCell.SetStartInspectionTime(inputData.m_inspectioinStartTime_ms);
    }
    else
    {
        inspectionCell.m_cellResult.m_imageSavePath = inputData.m_imageSavePath;
        inspectionCell.m_inspectionImageFull = inputData.m_imageFull;
        inspectionCell.SetInspectionSplitImage(inputData.m_imageBright, inputData.m_imageDark);

	    //{{ 검사항목, 검사스펙 및 Parameter
        auto& inspectionRoot = Inspection::Accessor::GetRoot();
        inspectionCell.CopyDataBase(inspectionRoot.GetInspectionCell(inputData.m_isUp));
        CInspectionMeasure* mismatch = (CInspectionMeasure*)inspectionRoot.GetProcessingItem(
            true, ProcItemCategory::measure); // mismatch 스펙은 상부에만 있고 따로 취급해야 함
        if (mismatch != nullptr)
        {
            inspectionCell.GetProcessingItem(ProcItemCategory::measure)->CopyDataBase(mismatch);
        }
        //}}

        //inspectionCell.m_invaild = inputData.m_invalid;
        //inspectionCell.m_cellTriggerCnt = inputData.m_CellTriggerCount;
        inspectionCell.m_cellID = inputData.m_cellID;
        inspectionCell.m_imagePosition = inputData.m_imagePos;
        inspectionCell.m_grabTime_ms = inputData.m_grabTime_ms;
        inspectionCell.m_inputTime = inputData.m_inputTime;
        inspectionCell.SetUpDown(inputData.m_isUp);
        inspectionCell.m_nAnodeCathode = g_systemParameter.nAnodeCathode;
        //inspectionCell.m_nInsulateType = g_systemParameter.nInsulateType;

        inspectionCell.Initialize();
        inspectionCell.SetStartInspectionTime(inputData.m_inspectioinStartTime_ms);
	}

    // Live모드일 때 검사스레드 동작안하도록 하기 위함(Check 필요)
    if (m_bInline)
    {
        SetEvent(m_threadControl_InlineInspection[threadIndex].m_Signal_Start);
    }
}


UINT CIBCIDlg::ThreadFunc_InlineInspection(LPVOID pParam)
{
	CIBCIDlg *pMain = (CIBCIDlg *)pParam;

	return pMain->ThreadFunc_InlineInspection();
}

UINT CIBCIDlg::ThreadFunc_InlineInspection()
{
	static CCriticalSection cs;

	cs.Lock();
	static long nLatestThreadID = 0;
	const long nThreadID = nLatestThreadID++;

	auto& control = m_threadControl_InlineInspection[nThreadID];
	auto& inspectionCell = m_InspectionCellGroup[nThreadID];
    cs.Unlock();

	::SetEvent(control.m_Signal_Ready);

	HANDLE pHandles[] = { control.m_Signal_Kill, control.m_Signal_Start };
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

			m_loggerSequence.Log(0, _T("[Inspection] Start %s, Cell ID : %d"), (LPCTSTR)strUp, inspectionCell.m_cellID);
            TRACE2("================================================================= Thread ID : %d, Result Size : %d\r\n", nThreadID, m_vecReceiveData.size());

            if (m_nCellID_finalcalc == 0 && m_nCellID_sync == 0 && m_nCellID_calc == 0)
            {
                m_nCellID_finalcalc = inspectionCell.m_cellID;
            }
            else
            {
                if (inspectionCell.m_cellID == 0)
                {
                    if (m_nCellID_sync == 63)
                    {
                        m_nCellID_finalcalc = m_nCellID_calc + 1;
                        TRACE("calc63_1");
                    }
                    else if (m_nCellID_sync == 62)
                    {
                        m_nCellID_finalcalc = m_nCellID_calc + 2;
                        TRACE("calc63_2");
                    }
                }
                else
                {
                    m_nCellID_finalcalc = m_nCellID_calc + inspectionCell.m_cellID - m_nCellID_sync;
                }
            }

            CString str;
            str.Format(_T("_______________________________________cell ID = %d\n"), m_nCellID_finalcalc);
            TRACE(str);
            m_nCellID_sync = inspectionCell.m_cellID;
            m_nCellID_calc = m_nCellID_finalcalc;
            // ---- DisplayMismatchData ----
            inspectionCell.m_cellID = m_nCellID_finalcalc;

            SYSTEMTIME CurTime;
            GetLocalTime(&CurTime);

			bool isInspectionOK = false;
			if (true)
			{
				//220817_sie
				IPVM::TimeCheck tcC;
				bool bError_Bright = false, bError_Dark = false;
				//bError_Bright = m_InspImageValueCompare.Set(inspectionCell.m_inspectionImageBright, inspectionCell.m_isUp, true, 5);
				//bError_Dark = m_InspImageValueCompare.Set(inspectionCell.m_inspectionImageDark, inspectionCell.m_isUp, false, 5);
				float fC = tcC.Elapsed_ms();

				InspTime.Reset();
				//isInspectionOK = inspectionCell.DoInspection();
				insptime = InspTime.Elapsed_ms();
                //===
                inspectionCell.m_inspectionImageBright;
                if (::GetKeyState(VK_SHIFT) < 0 && ::GetKeyState(VK_CONTROL) < 0)
                {
  
                    static long number = 0;
                    CString path;

                    path.Format(_T("c:\\aaaa\\birght%d.bmp"), number);
                    inspectionCell.m_inspectionImageBright.SaveBmp(path);

                    path.Format(_T("c:\\aaaa\\dark%d.bmp"), number);
                    inspectionCell.m_inspectionImageDark.SaveBmp(path);
                    number++;
                }
              
                if (g_systemParameter.GetImageSaveOption() == 2 || g_systemParameter.GetImageSaveOption() == 1)
                {
                     m_cellImageSaver->PushCell(inspectionCell.m_inspectionImageBright, 0, inspectionCell.m_cellID);
                     m_cellImageSaver->PushCell(inspectionCell.m_inspectionImageDark, 1, inspectionCell.m_cellID);
                }
                else if (g_systemParameter.GetImageSaveOption() == 3 && inspectionCell.m_resultValue == II_RESULT_VALUE::REJECT)
                {
                    m_cellImageSaver->PushCell(inspectionCell.m_inspectionImageBright, 0, inspectionCell.m_cellID);
                    m_cellImageSaver->PushCell(inspectionCell.m_inspectionImageDark, 1, inspectionCell.m_cellID);
                }
                

                long nSizeDivideVertical = (long)inspectionCell.m_ISICellAllDefect.m_vvMeasureDefect.size();
                auto& vecMeasureDefect = inspectionCell.m_ISICellAllDefect.m_vvMeasureDefect;

                // surface
                CInspectionCell* unit = &inspectionCell;

                CString strRows, strDirectPath;
                CString strCsvFolderPath, strCsvNoCellIDPath;
                if (!g_systemParameter.pathResultDataCsv.IsEmpty())
                {
                    CString lotId = _T("LOT");
                    if (!unit->m_ISICellAllDefect.strLotID.IsEmpty())
                        lotId.Format(_T("%s"), unit->m_ISICellAllDefect.strLotID);
                    CString topBack;
                    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK"); 

                    //strDirectPath.Format(
                    //    _T("%s\\%s\\%s\\%010d"), g_systemParameter.pathResultDataCsv, topBack, lotId, unit->m_cellID);
                    strDirectPath.Format(_T("%s\\%04d_%02d\\%02d\\%02d\\%s\\Data\\%010d"),
                        g_systemParameter.pathResultDataCsv, CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour,
                        topBack, unit->m_cellID);

                    strCsvFolderPath.Format(_T("%s\\%04d_%02d\\%02d\\%02d\\%s\\Data\\%010d"),
                        g_systemParameter.pathResultDataCsv, CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour,
                        topBack, unit->m_cellID);

                    strCsvNoCellIDPath.Format(_T("%s\\%04d_%02d\\%02d\\%02d\\%s\\Data"),
                        g_systemParameter.pathResultDataCsv, CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour,
                        topBack);
                    IPVM::CreateDirectories(strCsvNoCellIDPath);
                }
                else
                {
                    CString lotId = _T("LOT");
                    if (!unit->m_ISICellAllDefect.strLotID.IsEmpty())
                        lotId.Format(_T("%s"), unit->m_ISICellAllDefect.strLotID);
                    CString topBack;
                    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");
                    strDirectPath.Format(_T("D:\\RESULT\\%s\\%s\\%010d"), topBack, lotId, unit->m_cellID);

                    strCsvFolderPath.Format(_T("D:\\result\\%04d_%02d\\%02d\\%02d\\%s\\Data\\%010d"),
                        CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour,
                        topBack, unit->m_cellID);

                    strCsvNoCellIDPath.Format(_T("D:\\result\\%04d_%02d\\%02d\\%02d\\%s\\Data"),
                        CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour,
                        topBack);
                    IPVM::CreateDirectories(strCsvNoCellIDPath);
                }
                for (long n = 0; n < unit->m_ISICellAllDefect.m_vecCandidate.size(); n++)
                {
                    IPVM::Rect rtDraw = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel;

                    IPVM::Rect rect = rtDraw;

                    int OriginRect_Width = rect.Width();
                    int OriginRect_Height = rect.Height();
                    int Offset_X = 0;
                    int Offset_Y = 0;
                    if (100 > max(rect.Width(), rect.Height()))
                    {
                        Offset_X = 50;
                        Offset_Y = 50;
                    }
                    else if (rect.Width() > rect.Height())
                    {
                        int Diff = rect.Width() - rect.Height();
                        int Diff_half = Diff / 2;
                        Offset_Y = Diff_half;
                    }
                    else
                    {
                        int Diff = rect.Height() - rect.Width();
                        int Diff_half = Diff / 2;
                        Offset_X = Diff_half;
                    }
                    rect.InflateRect(Offset_X, Offset_Y, Offset_X, Offset_Y);
                    rect &= IPVM::Rect(unit->m_inspectionImageBright);

                    int offset_left = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_left - rect.m_left;
                    int offset_top = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_top - rect.m_top;
                    int offset_right = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_right - rect.m_right;
                    int offset_bottom = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_bottom - rect.m_bottom;

                    IPVM::Image_8u_C1 CropImage_Bright(rect.Width(), rect.Height());
                    IPVM::Image_8u_C1 CropImage_Dark(rect.Width(), rect.Height());

                    IPVM::Rect OriginRect(CropImage_Bright);
                    OriginRect.DeflateRect(offset_left, offset_top, -offset_right, -offset_bottom);

                    IPVM::Image_8u_C1& OriginImage_Bright = unit->m_inspectionImageBright;
                    IPVM::Image_8u_C1& OriginImage_Dark = unit->m_inspectionImageDark;
                    IPVM::ImageProcessing::Copy(OriginImage_Bright, rect, IPVM::Rect(CropImage_Bright), CropImage_Bright);
                    IPVM::ImageProcessing::Copy(OriginImage_Dark, rect, IPVM::Rect(CropImage_Dark), CropImage_Dark);

                    if ((II_RESULT_VALUE)unit->m_ISICellAllDefect.m_vecISIResultCode[n] == II_RESULT_VALUE::PASS
                        && (II_RESULT_VALUE)unit->m_resultValue != II_RESULT_VALUE::INVALID)
                    {
                        continue;
                    }


                    IPVM::CreateDirectories(strDirectPath);
                    SPathCropImages sPathCropImages = SaveCropImages(unit, n, OriginImage_Bright, OriginImage_Dark, CropImage_Bright, CropImage_Dark, strDirectPath);
                    CISIViewerProperty propertyView = MakeISIViewerProperty(unit, n, CropImage_Bright, CropImage_Dark, rtDraw, sPathCropImages);

                    strRows.AppendFormat(_T("%s\n"), MakeCsvRowString(n, propertyView, unit));
                }
               
                if (!strRows.IsEmpty() && !strDirectPath.IsEmpty())
                {
                    CString strCsvFile;
                    strCsvFile.Format(_T("%s.csv12"), strDirectPath);
                    SaveCsvSurface(unit, strRows, strCsvFile);
                }

                if (g_systemParameter.bMeasureSave == TRUE)
                {
                    CString strCsvFile;
                    strCsvFile.Format(_T("%s_measure.csv"), strCsvFolderPath);
                    SaveCsvMeasure(unit, nSizeDivideVertical, vecMeasureDefect, strCsvFile);
                }
                
                //m_inlineResultUiUpdater->Trigger();

                if (nThreadID % 10 == 0)
                {
                    if (g_systemParameter.bDisplayShow == TRUE)
                    {
                        if (g_systemParameter.bEdgeViewSampling)
                            m_nSamplingCnt++;
                        UpdateEdgeView((CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign));
                    }
                    
                }
                
                //UpdateUi();
                
                // mismatch
                if (HostSocketConnectCheck())
                {
                    Share::ResultVision Resultdata;
                    if (inspectionCell.m_resultValue != II_RESULT_VALUE::INVALID)
                        SendHostResultDataInfo(Resultdata, &inspectionCell);
                    // mismatch
                    // ResultMismatchData(nSizeDivideVertical, vecMeasureDefect, _T("test1234"), inspectionCell.m_cellID);
                    ResultMismatchData(
                        nSizeDivideVertical, vecMeasureDefect, _T("test1234"), inspectionCell.m_cellID, Resultdata);
                }
                else
                {
                    ResultMismatchData(nSizeDivideVertical, vecMeasureDefect, _T("test1234"), inspectionCell.m_cellID);
                }
                
                //DisplayMismatchData(nSizeDivideVertical, vecMeasureDefect, _T("test1234"), inspectionCell.m_cellID);

                

				CString strUp = _T("Down");
				//m_InspectionCount = inspectionCell.m_InspectionCount;
				if (inspectionCell.m_cellResult.isUp)
				{
					strUp = _T("Up");
				}

				//m_loggerResult.Log(0, _T("Push_back Cell %s [%d]"), (LPCTSTR)strUp, inspectionCell.m_cellResult.cellID);
				//연속 타임아웃 카운트 

                // NG 일때만 Display 하는 거
                if (m_pViewCellImage->IsWindowVisible() && m_pViewCellImage->m_nModeUpdate == ENUM_VIEW_MODE_CELL_IMAGE::UPDATE_NG_ONLY)
                {
                    m_viewCellImageUpdater->SetGrayMode(m_pViewCellImage->m_nModeGray);
                    m_viewCellImageUpdater->Push(inspectionCell.m_inspectionImageBright, inspectionCell.m_inspectionImageDark, inspectionCell.m_cellID, unit->m_ISICellAllDefect);
                }                        
			}

			m_loggerSequence.Log(0, _T("[Inspection] End %s, Cell ID : %d"), (LPCTSTR)strUp, inspectionCell.m_cellID);

			if (isInspectionOK)
			{
				m_loggerSequence.Log(0, _T("[Inspection] Success %s, Cell ID, Thread ID : %d, %d"), (LPCTSTR)strUp, inspectionCell.m_cellID, nThreadID);
			}
			else
			{
				m_loggerSequence.Log(0, _T("[Inspection] Fail %s, Cell ID, Thread ID : %d, %d"), (LPCTSTR)strUp, inspectionCell.m_cellID, nThreadID);
			}
			//{{ calculation time
			m_loggerTime.Log(0, _T(",Inspection,%s,%d,%.2f,ms(only insp : %.2f)"), (LPCTSTR)strUp, inspectionCell.m_cellID, timeCheck.Elapsed_ms(), insptime);
			//}} calculation time

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

UINT CIBCIDlg::ThreadFunc_InlineInspection_UC1(LPVOID pParam)
{
    CIBCIDlg* pMain = (CIBCIDlg*)pParam;

    return pMain->ThreadFunc_InlineInspection_UC1();
}

UINT CIBCIDlg::ThreadFunc_InlineInspection_UC1()
{
    static CCriticalSection cs;

    cs.Lock();
    static long nLatestThreadID = 0;
    const long nThreadID = nLatestThreadID++;

    auto& control = m_threadControl_InlineInspection[nThreadID];
    auto& inspectionCell = m_InspectionCellGroup[nThreadID];
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

            m_loggerSequence.Log(0, _T("[Inspection] Start %s, Cell ID : %d"), (LPCTSTR)strUp, inspectionCell.m_cellID);
            TRACE2("================================================================= Thread ID : %d, Result Size : "
                   "%d\r\n",
                nThreadID, m_vecReceiveData.size());

            if (m_nCellID_finalcalc == 0 && m_nCellID_sync == 0 && m_nCellID_calc == 0)
            {
                m_nCellID_finalcalc = inspectionCell.m_cellID;
            }
            else
            {
                if (inspectionCell.m_cellID == 0)
                {
                    if (m_nCellID_sync == 63)
                    {
                        m_nCellID_finalcalc = m_nCellID_calc + 1;
                        TRACE("calc63_1");
                    }
                    else if (m_nCellID_sync == 62)
                    {
                        m_nCellID_finalcalc = m_nCellID_calc + 2;
                        TRACE("calc63_2");
                    }
                }
                else
                {
                    m_nCellID_finalcalc = m_nCellID_calc + inspectionCell.m_cellID - m_nCellID_sync;
                }
            }

            CString str;
            str.Format(_T("_______________________________________cell ID = %d\n"), m_nCellID_finalcalc);
            TRACE(str);
            m_nCellID_sync = inspectionCell.m_cellID;
            m_nCellID_calc = m_nCellID_finalcalc;
            // ---- DisplayMismatchData ----
            // cell id 시간으로 하기
            SYSTEMTIME time;
            GetLocalTime(&time);
            long nTEmp = time.wHour * 10000000 + time.wMinute * 100000 + time.wSecond * 1000 + time.wMilliseconds;
            //, time.wMinute, time.wSecond, 
            //inspectionCell.m_cellID = nTEmp;
            inspectionCell.m_cellID = m_nTestCellID;
            CString topBack;
            topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK"); 
            if(topBack == _T("TOP"))
            {
            	if (m_nReceiveCellIndex > m_nCellID_finalcalc)
            	{
            	     continue;

            	}
            }      
            m_nTestCellID++;
            if (m_nTestCellID > 2147483640)
            {
                m_nTestCellID = 0;
            }
            //m_nCellID_finalcalc;
            BOOL SaveNG = FALSE ;
            BOOL SaveNGCrater{};
			bool isInspectionOK = false;
            CCPUUsage cpuUsage;
            double usage = cpuUsage.GetCPUUsage();
            CString strResult = _T("");
            if (usage < 90)
			{
				//220817_sie
				IPVM::TimeCheck tcC;
				bool bError_Bright = false, bError_Dark = false;
				//bError_Bright = m_InspImageValueCompare.Set(inspectionCell.m_inspectionImageBright, inspectionCell.m_isUp, true, 5);
				//bError_Dark = m_InspImageValueCompare.Set(inspectionCell.m_inspectionImageDark, inspectionCell.m_isUp, false, 5);
				float fC = tcC.Elapsed_ms();

				InspTime.Reset();
				//isInspectionOK = inspectionCell.DoInspection();

                if (inspectionCell.m_resultValue == II_RESULT_VALUE::INVALID)
                {
                    m_bCheckInvalid = TRUE; 
                    m_nCheckPass = 0;
                }
                if (m_bCheckInvalid == TRUE)
                {
                    m_nCheckPass++;
                    if (m_nCheckPass>4)
                    {
                        m_bCheckInvalid = FALSE;
                    }
                }
				insptime = InspTime.Elapsed_ms();
                SaveNG = false;
                SaveNGCrater = false;
                //===
                inspectionCell.m_inspectionImageBright;
                
                //auto* unit = (CInspectionCellAlign*)inspectionCell;
                //if (!m_bCheckInvalid)
                //    SendHostResultDataInfo(&inspectionCell);
                

                long nSizeDivideVertical = (long)inspectionCell.m_ISICellAllDefect.m_vvMeasureDefect.size();
                auto& vecMeasureDefect = inspectionCell.m_ISICellAllDefect.m_vvMeasureDefect;

                // surface
                CInspectionCell* unit = &inspectionCell;

                CString strRows, strDirectPath;
                if (!g_systemParameter.pathResultDataCsv.IsEmpty())
                {
                    CString lotId = _T("LOT");
                    if (!unit->m_ISICellAllDefect.strLotID.IsEmpty())
                        lotId.Format(_T("%s"), unit->m_ISICellAllDefect.strLotID);
                    CString topBack;
                    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK"); 
                    strDirectPath.Format(_T("%s\\%04d\\%02d\\%02d\\%s\\%s\\\\%010d"), g_systemParameter.pathResultDataCsv,
                        time.wYear, time.wMonth, time.wDay, topBack, lotId, unit->m_cellID);
                }
                else
                {
                    CString lotId = _T("LOT");
                    if (!unit->m_ISICellAllDefect.strLotID.IsEmpty())
                        lotId.Format(_T("%s"), unit->m_ISICellAllDefect.strLotID);
                    CString topBack;
                    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");
                    strDirectPath.Format(_T("%s\\%04d\\%02d\\%02d\\%s\\%s\\\\%010d"), g_systemParameter.pathResultDataCsv,
                        time.wYear, time.wMonth, time.wDay, topBack, lotId, unit->m_cellID);
                }
                BOOL bResult[100] = {
                    FALSE,
                };
                for (long n = 0; n < unit->m_ISICellAllDefect.m_vecCandidate.size(); n++)
                {
                    IPVM::Rect rtDraw = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel;

                    IPVM::Rect rect = rtDraw;

                    int OriginRect_Width = rect.Width();
                    int OriginRect_Height = rect.Height();
                    int Offset_X = 0;
                    int Offset_Y = 0;
                    if (100 > max(rect.Width(), rect.Height()))
                    {
                        Offset_X = 50;
                        Offset_Y = 50;
                    }
                    else if (rect.Width() > rect.Height())
                    {
                        int Diff = rect.Width() - rect.Height();
                        int Diff_half = Diff / 2;
                        Offset_Y = Diff_half;
                    }
                    else
                    {
                        int Diff = rect.Height() - rect.Width();
                        int Diff_half = Diff / 2;
                        Offset_X = Diff_half;
                    }
                    rect.InflateRect(Offset_X, Offset_Y, Offset_X, Offset_Y);
                    rect &= IPVM::Rect(unit->m_inspectionImageBright);

                    int offset_left = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_left - rect.m_left;
                    int offset_top = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_top - rect.m_top;
                    int offset_right = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_right - rect.m_right;
                    int offset_bottom = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_bottom - rect.m_bottom;

                    IPVM::Image_8u_C1 CropImage_Bright(rect.Width(), rect.Height());
                    IPVM::Image_8u_C1 CropImage_Dark(rect.Width(), rect.Height());

                    IPVM::Rect OriginRect(CropImage_Bright);
                    OriginRect.DeflateRect(offset_left, offset_top, -offset_right, -offset_bottom);

                    IPVM::Image_8u_C1& OriginImage_Bright = unit->m_inspectionImageBright;
                    IPVM::Image_8u_C1& OriginImage_Dark = unit->m_inspectionImageDark;
                    IPVM::ImageProcessing::Copy(OriginImage_Bright, rect, IPVM::Rect(CropImage_Bright), CropImage_Bright);
                    IPVM::ImageProcessing::Copy(OriginImage_Dark, rect, IPVM::Rect(CropImage_Dark), CropImage_Dark);
                    
                    if ((II_RESULT_VALUE)unit->m_ISICellAllDefect.m_vecISIResultCode[n] == II_RESULT_VALUE::PASS)
                    {
                        continue;
                    }
                    if (unit->m_ISICellAllDefect.m_vecCandidate.size()<20)
                    {
                        if(!m_bCheckInvalid)
                            SaveNG = true;
                        //AfxMessageBox(_T("1"));
                        bResult[unit->m_ISICellAllDefect.m_vecISIDefectType[n]] = TRUE;
                    }
                    
                    IPVM::CreateDirectories(strDirectPath);
                    SPathCropImages sPathCropImages = SaveCropImages(unit, n, OriginImage_Bright, OriginImage_Dark, CropImage_Bright, CropImage_Dark, strDirectPath);
                    CISIViewerProperty propertyView = MakeISIViewerProperty(unit, n, CropImage_Bright, CropImage_Dark, rtDraw, sPathCropImages);

                    strRows.AppendFormat(_T("%s\n"), MakeCsvRowString(n, propertyView, unit));
                   

                }
                for (long n = 0; n < 100; n++)
                {
                    if (bResult[n] == TRUE)
                    {
                        CString strDefectName = GetDefectName((DefectType)n);
                        strResult += _T("_");
                        strResult += strDefectName;
                        if (_T("Stain_Pinhole") == strDefectName)
                        {
                            SaveNGCrater = true;
                        }
                    }
                }
                if (!strRows.IsEmpty() && !strDirectPath.IsEmpty())
                {
                    CString strCsvFile;
                    strCsvFile.Format(_T("%s.csv12"), strDirectPath);
                    SaveCsvSurface(unit, strRows, strCsvFile);
                }

                

                //
                ////m_inlineResultUiUpdater->Trigger();

                 if (nThreadID % 10 == 0)
                {
                    if (g_systemParameter.bDisplayShow == TRUE)
                    {
                        UpdateEdgeView((CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign));
                    }
                }

                Share::ResultVision Resultdata;


                //UpdateUi();
                //if (!m_bCheckInvalid)
                SendHostResultDataInfo(Resultdata, &inspectionCell);
                // mismatch
                // ResultMismatchData(nSizeDivideVertical, vecMeasureDefect, _T("test1234"), inspectionCell.m_cellID);
                ResultMismatchData(  nSizeDivideVertical, vecMeasureDefect, _T("test1234"), inspectionCell.m_cellID, Resultdata);
                SaveCsvMeasureNotMsa(nSizeDivideVertical, vecMeasureDefect, _T("test1234"), unit->m_cellID);
                //DisplayMismatchData(nSizeDivideVertical, vecMeasureDefect, _T("test1234"), inspectionCell.m_cellID);
 
                Resultdata.Clear();

				CString strUp = _T("Down");
				//m_InspectionCount = inspectionCell.m_InspectionCount;
				if (inspectionCell.m_cellResult.isUp)
				{
					strUp = _T("Up");
				}

				//m_loggerResult.Log(0, _T("Push_back Cell %s [%d]"), (LPCTSTR)strUp, inspectionCell.m_cellResult.cellID);
				//연속 타임아웃 카운트 

                // NG 일때만 Display 하는 거
                
                if (m_pViewCellImage->IsWindowVisible() && m_pViewCellImage->m_nModeUpdate == ENUM_VIEW_MODE_CELL_IMAGE::UPDATE_NG_ONLY)
                {
                    m_viewCellImageUpdater->SetGrayMode(m_pViewCellImage->m_nModeGray);
                    m_viewCellImageUpdater->Push(inspectionCell.m_inspectionImageBright, inspectionCell.m_inspectionImageDark, inspectionCell.m_cellID, unit->m_ISICellAllDefect);
                }                        
			}
            
            if (SaveNG && isInspectionOK && g_systemParameter.GetImageSaveOption() != 2
                && g_systemParameter.GetImageSaveOption() != 1)
            {
                
               // m_cellImageSaver->PushCell(inspectionCell.m_inspectionImageFull, 0, inspectionCell.m_cellID);

                m_cellImageSaver->PushCell(
                   inspectionCell.m_inspectionImageBright, 0, inspectionCell.m_cellID, strResult);
               m_cellImageSaver->PushCell(inspectionCell.m_inspectionImageDark, 1, inspectionCell.m_cellID, strResult);
            }

            if (g_systemParameter.GetImageSaveOption() == 2 || g_systemParameter.GetImageSaveOption() == 1)
            {
                //m_cellImageSaver->PushCell(inspectionCell.m_inspectionImageBright, 0, inspectionCell.m_cellID);
                SaveNGCrater = FALSE; 

                if (SaveNGCrater && isInspectionOK)
                {
                    m_cellImageSaver->PushCell(
                        inspectionCell.m_inspectionImageFull, 0, inspectionCell.m_cellID, strResult, true);
                    /*m_cellImageSaver->PushCell(inspectionCell.m_inspectionImageBright, 0, inspectionCell.m_cellID);
                    m_cellImageSaver->PushCell(inspectionCell.m_inspectionImageDark, 1, inspectionCell.m_cellID);*/
                }

                long nTempResize = g_systemParameter.m_nImageResize;
                if (nTempResize < 1)
                    nTempResize = 1;
                if (nTempResize > 10)
                    nTempResize = 10;
                inspectionCell.m_inspectionImageBrightResize.Free();
                inspectionCell.m_inspectionImageDarkResize.Free();
                inspectionCell.m_inspectionImageBrightResize.Create(
                    inspectionCell.m_inspectionImageBright.GetSizeX() / nTempResize,
                    inspectionCell.m_inspectionImageBright.GetSizeY() / nTempResize);
                inspectionCell.m_inspectionImageDarkResize.Create(
                    inspectionCell.m_inspectionImageDark.GetSizeX() / nTempResize,
                    inspectionCell.m_inspectionImageDark.GetSizeY() / nTempResize);
                if (nTempResize!=1)
                {
                    IPVM::ImageProcessing::Resize_Linear(
                        inspectionCell.m_inspectionImageBright, inspectionCell.m_inspectionImageBrightResize);
                    IPVM::ImageProcessing::Resize_Linear(
                        inspectionCell.m_inspectionImageDark, inspectionCell.m_inspectionImageDarkResize);
                }
                
                m_cellImageSaver->PushCell(
                    inspectionCell.m_inspectionImageBrightResize, 0, inspectionCell.m_cellID, strResult);
                m_cellImageSaver->PushCell(
                    inspectionCell.m_inspectionImageDarkResize, 1, inspectionCell.m_cellID, strResult);

            }
			m_loggerSequence.Log(0, _T("[Inspection] End %s, Cell ID : %d"), (LPCTSTR)strUp, inspectionCell.m_cellID);

			if (isInspectionOK)
			{
				m_loggerSequence.Log(0, _T("[Inspection] Success %s, Cell ID, Thread ID : %d, %d"), (LPCTSTR)strUp, inspectionCell.m_cellID, nThreadID);
			}
			else
			{
				m_loggerSequence.Log(0, _T("[Inspection] Fail %s, Cell ID, Thread ID : %d, %d"), (LPCTSTR)strUp, inspectionCell.m_cellID, nThreadID);
			}
			//{{ calculation time
			m_loggerTime.Log(0, _T(",Inspection,%s,%d,%.2f,ms(only insp : %.2f)"), (LPCTSTR)strUp, inspectionCell.m_cellID, timeCheck.Elapsed_ms(), insptime);
			//}} calculation time

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

UINT CIBCIDlg::ThreadFunc_InlineInspectionResultUpdate(LPVOID pParam)
{
    CIBCIDlg* pMain = (CIBCIDlg*)pParam;

    return pMain->ThreadFunc_InlineInspectionResultUpdate();
}
UINT CIBCIDlg::ThreadFunc_InlineInspectionResultUpdate()
{
    auto& control = m_threadControl_InlineInspectionResultUpdate;

    HANDLE pHandles[] = {control.m_Signal_Kill, control.m_Signal_Start};
    const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);

    while (1)
    {
        DWORD dwRet = WaitForMultipleObjects(nSignalCount, pHandles, FALSE, 200);
        if (WAIT_TIMEOUT == dwRet)
        {
            //TRACE1("================================================================= Thread Result Dispaly, Result Display Size : %d\r\n", m_vecReceiveData.size());
            std::vector<CISIViewerProperty> vecReceiveData;
            {
                CSingleLock lock(&m_csPropertyView, TRUE);
                //swap(m_vecReceiveData, vecReceiveData);
                vecReceiveData = std::move(m_vecReceiveData);
                lock.Unlock();
            }
            if (g_systemParameter.bDisplayShow == TRUE)
            {
                UpdateUiData(vecReceiveData);
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

void CIBCIDlg::ResultMismatchData(
    long nSizeDivideVertical, std::vector<std::vector<CIDISpecInfo>> m_vvMeasureDefect, CString strLotID, long nCellID)
{
    //InitializeResMismatchData(nSizeDivideVertical, true);
    //InitializeResMismatchData(nSizeDivideVertical);

    ResultMismatch stResAllMismatch; //Missmatch 변수 통합 Data로 빼야하나?
    std::vector<ResultLaneData> resvecLaneUp;
    std::vector<ResultSpec> resvecLaneSpec;
    float fFullLength = 0.f;
    stResAllMismatch.m_strLotID = strLotID;
    stResAllMismatch.m_nCellIndex = nCellID;
    for (int nVerticalIdx = 0; nVerticalIdx < nSizeDivideVertical; nVerticalIdx++)
    {
        auto& vecMeasureDefect = m_vvMeasureDefect[nVerticalIdx];

        long nSizeMeasureItem = (long)vecMeasureDefect.size();

#if INSPECTION_MACHINE_UC1
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_LeftFoil)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_LeftCoating)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_RightCoating)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_MiddleFoil)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_RightFoil)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_FullLength)
            {
                fFullLength = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
        }
#else
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_NonCoating_1)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_Coating_1)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_Coating_2)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_MiddleFoil)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_NonCoating_2)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_FullLength)
            {
                fFullLength = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
        }
#endif // INSPECTION_MACHINE_UC1

        
    }
   // if (g_systemParameter.machineType != MachineType::UC1_mismatch)
    {
        ResultMismatch stRes;
        stRes.m_strLotID = strLotID;
        stRes.m_vecResUpLane = resvecLaneUp;
        stRes.m_vecResDownLane = resvecLaneUp;
        stRes.m_vecResSpec = resvecLaneSpec;
        stRes.m_fTopFullLength = fFullLength;
        stRes.m_fBackFullLength = fFullLength;
        stRes.m_nCellIndex = nCellID;
        m_inlineResultCoupler->Push1(stRes);

        // OnSend
        if (g_systemParameter.bUseServer == 1)
        {
            CString strSend;
            strSend.Format(_T("MISSMATCH,%s,"), stRes.m_strLotID);
            strSend.AppendFormat(_T("%d,"), stRes.m_nCellIndex);
            strSend.AppendFormat(_T("%d,"), g_systemParameter.bDirection);
            strSend.AppendFormat(_T("%d,"), resvecLaneUp.size());

            for (int i = 0; i < resvecLaneUp.size(); i++)
                strSend.AppendFormat(_T("%f,"), resvecLaneUp[i].fLaneWidth);

            strSend.AppendFormat(_T("%f,"), fFullLength);

            if (g_systemParameter.bServer == 0) //client
            {
                OnSend2(strSend);
            }
            else if (g_systemParameter.bServer == 1) // server
            {
                OnSend2_S(strSend);
            }
        }
        // ----[end] DisplayMismatchData ----
    }

}

void CIBCIDlg::DisplayMismatchData(long nSizeDivideVertical, std::vector<std::vector<CIDISpecInfo>> m_vvMeasureDefect
                                   , CString strLotID, long nCellID)
{
    
    //InitializeResMismatchData(nSizeDivideVertical, true);
    //InitializeResMismatchData(nSizeDivideVertical);

    ResultMismatch stResAllMismatch; //Missmatch 변수 통합 Data로 빼야하나?
    std::vector<ResultLaneData> resvecLaneUp;
    std::vector<ResultSpec> resvecLaneSpec;
    float fFullLength = 0.f;
    stResAllMismatch.m_strLotID = strLotID;
    stResAllMismatch.m_nCellIndex = nCellID;
    for (int nVerticalIdx = 0; nVerticalIdx < nSizeDivideVertical; nVerticalIdx++)
    {
        auto& vecMeasureDefect = m_vvMeasureDefect[nVerticalIdx];
        

        long nSizeMeasureItem = (long)vecMeasureDefect.size();

#if INSPECTION_MACHINE_UC1
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_LeftFoil)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_LeftCoating)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_RightCoating)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_RightFoil)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_FullLength)
            {
                fFullLength = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
        }
#else
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_NonCoating_1)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_Coating_1)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_Coating_2)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_NonCoating_2)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_FullLength)
            {
                fFullLength = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
        }
#endif // INSPECTION_MACHINE_UC1

        
    }

    

    if (g_systemParameter.bUseServer == 1)
    {
        ResultMismatch stResAllMismatchView;

        if (g_systemParameter.bServer == 0) //client
        {
            CString strSend;
            CString strData;
            strSend = stResAllMismatch.m_strLotID + _T(",");

            strData.Format(_T("%d,"), stResAllMismatch.m_nCellIndex);
            strSend += strData;
            strData.Format(_T("%d,"), g_systemParameter.bDirection);
            strSend += strData;
            strData.Format(_T("%d,"), resvecLaneUp.size());
            strSend += strData;

            for (int i = 0; i < resvecLaneUp.size(); i++)
            {
                strData.Format(_T("%f,"), resvecLaneUp[i].fLaneWidth);
                strSend += strData;
            }

            strData.Format(_T("%f,"), fFullLength);
            strSend += strData;

            OnSend2(_T("MISSMATCH,") + strSend);
            m_loggerResult.Log(0, strSend);
        }
        else if (g_systemParameter.bServer == 1) // server
        {
            CString strSend;
            CString strData;
            strSend = stResAllMismatch.m_strLotID + _T(",");

            strData.Format(_T("%d,"), stResAllMismatch.m_nCellIndex);
            strSend += strData;
            strData.Format(_T("%d,"), g_systemParameter.bDirection);
            strSend += strData;
            strData.Format(_T("%d,"), resvecLaneUp.size());
            strSend += strData;

            for (int i = 0; i < resvecLaneUp.size(); i++)
            {
                strData.Format(_T("%f,"), resvecLaneUp[i].fLaneWidth);
                strSend += strData;
            }

            strData.Format(_T("%f,"), fFullLength);
            strSend += strData;

            OnSend2_S(_T("MISSMATCH,") + strSend);
            m_loggerResult.Log(0, strSend);

            for (long n = 0; n < m_vecMismatchData.size(); n++)
            {
                if (m_vecMismatchData[n].m_strLotID == stResAllMismatch.m_strLotID)
                {
                    if (m_vecMismatchData[n].m_nCellIndex == stResAllMismatch.m_nCellIndex)
                    {
                        if (g_systemParameter.bDirection == 0) //top
                        {
                            m_vecMismatchData[n].m_vecResUpLane = resvecLaneUp;

                            stResAllMismatch = MismathchData(
                                stResAllMismatch, resvecLaneUp, m_vecMismatchData, resvecLaneSpec, fFullLength, n, 0);
                        }
                        else
                        {
                            m_vecMismatchData[n].m_vecResDownLane = resvecLaneUp;

                            stResAllMismatch = MismathchData(
                                stResAllMismatch, resvecLaneUp, m_vecMismatchData, resvecLaneSpec, fFullLength, n, 1);
                        }

                        if (m_vecMismatchData[n].m_vecResUpLane.size() == m_vecMismatchData[n].m_vecResDownLane.size())
                        {
                            stResAllMismatchView = stResAllMismatch;
                            stResAllMismatchView.m_nCellIndex = stResAllMismatch.m_nCellIndex;
                            if (m_vecMismatchData[n].m_vecResUpLane.size() == 2)
                            {
                                float fMismatch;
                                fMismatch = fabs(m_vecMismatchData[n].m_vecResUpLane[0].fLaneWidth
                                    - m_vecMismatchData[n].m_vecResDownLane[0].fLaneWidth);
                                stResAllMismatchView.m_vecfDiffWidth.push_back(fMismatch);

                                fMismatch = fabs(m_vecMismatchData[n].m_vecResUpLane[1].fLaneWidth
                                    - m_vecMismatchData[n].m_vecResDownLane[1].fLaneWidth);
                                stResAllMismatchView.m_vecfDiffWidth.push_back(fMismatch);
                            }
                            else if (m_vecMismatchData[n].m_vecResUpLane.size() == 3)
                            {
                                float fMismatch;
                                fMismatch = fabs(m_vecMismatchData[n].m_vecResUpLane[0].fLaneWidth
                                    - m_vecMismatchData[n].m_vecResDownLane[0].fLaneWidth);
                                stResAllMismatchView.m_vecfDiffWidth.push_back(fMismatch);

                                fMismatch = fabs((m_vecMismatchData[n].m_vecResUpLane[0].fLaneWidth
                                                     + m_vecMismatchData[n].m_vecResUpLane[1].fLaneWidth)
                                    - (m_vecMismatchData[n].m_vecResDownLane[0].fLaneWidth
                                        + m_vecMismatchData[n].m_vecResDownLane[1].fLaneWidth));
                                stResAllMismatchView.m_vecfDiffWidth.push_back(fMismatch);

                                fMismatch = fabs(m_vecMismatchData[n].m_vecResUpLane[2].fLaneWidth
                                    - m_vecMismatchData[n].m_vecResDownLane[2].fLaneWidth);
                                stResAllMismatchView.m_vecfDiffWidth.push_back(fMismatch);
                            }
                            else if (m_vecMismatchData[n].m_vecResUpLane.size() == 4)
                            {
                                float fMismatch;
                                fMismatch = fabs(m_vecMismatchData[n].m_vecResUpLane[0].fLaneWidth
                                    - m_vecMismatchData[n].m_vecResDownLane[0].fLaneWidth);
                                stResAllMismatchView.m_vecfDiffWidth.push_back(fMismatch);

                                fMismatch = fabs((m_vecMismatchData[n].m_vecResUpLane[0].fLaneWidth
                                                     + m_vecMismatchData[n].m_vecResUpLane[1].fLaneWidth)
                                    - (m_vecMismatchData[n].m_vecResDownLane[0].fLaneWidth
                                        + m_vecMismatchData[n].m_vecResDownLane[1].fLaneWidth));
                                stResAllMismatchView.m_vecfDiffWidth.push_back(fMismatch);

                                fMismatch = fabs((m_vecMismatchData[n].m_vecResUpLane[3].fLaneWidth
                                                     + m_vecMismatchData[n].m_vecResUpLane[2].fLaneWidth)
                                    - (m_vecMismatchData[n].m_vecResDownLane[3].fLaneWidth
                                        + m_vecMismatchData[n].m_vecResDownLane[2].fLaneWidth));
                                stResAllMismatchView.m_vecfDiffWidth.push_back(fMismatch);

                                fMismatch = fabs(m_vecMismatchData[n].m_vecResUpLane[3].fLaneWidth
                                    - m_vecMismatchData[n].m_vecResDownLane[3].fLaneWidth);
                                stResAllMismatchView.m_vecfDiffWidth.push_back(fMismatch);
                            }
                        }

                        //SetAnimationParameter
                        if (m_pDlgAnimationWindow && MachineType::ES_rollpress_pinhole != g_systemParameter.machineType)
                        {
                            m_pDlgAnimationWindow->SetAnimationParameter(
                                stResAllMismatchView, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, g_systemParameter.nLaneCount, g_systemParameter.bOSDS, g_systemParameter.bRollerOS);
                        }

                        if (m_pDlgGraphWindow)
                        {
                            if (stResAllMismatchView.m_vecfDiffWidth.size() == 2)
                            {
                                m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(TRUE,
                                    DefectType::Dimension_Mismatch1, stResAllMismatchView.m_nCellIndex,
                                    stResAllMismatchView.m_vecfDiffWidth[0], true, _T("OK"));

                                m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(TRUE,
                                    DefectType::Dimension_Mismatch2, stResAllMismatchView.m_nCellIndex,
                                    stResAllMismatchView.m_vecfDiffWidth[1], true, _T("OK"));
                            }
                            if (stResAllMismatchView.m_vecfDiffWidth.size() == 3)
                            {
                                m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(TRUE,
                                    DefectType::Dimension_Mismatch1, stResAllMismatchView.m_nCellIndex,
                                    stResAllMismatchView.m_vecfDiffWidth[0], true, _T("OK"));

                                m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(TRUE,
                                    DefectType::Dimension_Mismatch2, stResAllMismatchView.m_nCellIndex,
                                    stResAllMismatchView.m_vecfDiffWidth[1], true, _T("OK"));
                                m_pDlgGraphWindow->m_pvecDlgGraphFrame[2]->SetData(TRUE,
                                    DefectType::Dimension_Mismatch3, stResAllMismatchView.m_nCellIndex,
                                    stResAllMismatchView.m_vecfDiffWidth[2], true, _T("OK"));
                            }
                            if (stResAllMismatchView.m_vecfDiffWidth.size() == 4)
                            {
                                m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(TRUE,
                                    DefectType::Dimension_Mismatch1, stResAllMismatchView.m_nCellIndex,
                                    stResAllMismatchView.m_vecfDiffWidth[0], true, _T("OK"));

                                m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(TRUE,
                                    DefectType::Dimension_Mismatch2, stResAllMismatchView.m_nCellIndex,
                                    stResAllMismatchView.m_vecfDiffWidth[1], true, _T("OK"));
                                m_pDlgGraphWindow->m_pvecDlgGraphFrame[2]->SetData(TRUE,
                                    DefectType::Dimension_Mismatch3, stResAllMismatchView.m_nCellIndex,
                                    stResAllMismatchView.m_vecfDiffWidth[2], true, _T("OK"));

                                m_pDlgGraphWindow->m_pvecDlgGraphFrame[3]->SetData(TRUE,
                                    DefectType::Dimension_Mismatch4, stResAllMismatchView.m_nCellIndex,
                                    stResAllMismatchView.m_vecfDiffWidth[3], true, _T("OK"));
                            }
                            // batch Inspection 돌릴 때
                            ExportMismatchGraphData(stResAllMismatchView.m_nCellIndex, stResAllMismatchView.m_vecfDiffWidth);
                            // Animation SetData
                        }
                    }
                }
            }
        }
        if (g_systemParameter.bNotPair == 1 || g_systemParameter.bRollerTwoSide == 0) 
        {
            if (m_vecMismatchData.size() == 0 && resvecLaneUp.size() > 0)
            {
                stResAllMismatch.m_vecResUpLane = resvecLaneUp;
                stResAllMismatch.m_vecResDownLane = resvecLaneUp;
                stResAllMismatch.m_vecResSpec = resvecLaneSpec;
                stResAllMismatch.m_fTopFullLength = fFullLength;
                stResAllMismatch.m_fBackFullLength = fFullLength;
                stResAllMismatchView = stResAllMismatch;
                for (long n = 0; n < resvecLaneUp.size(); n++)
                {
                    stResAllMismatchView.m_vecfDiffWidth.push_back(0);
                }

                if (m_pDlgAnimationWindow && MachineType::ES_rollpress_pinhole != g_systemParameter.machineType)
                {
                    m_pDlgAnimationWindow->SetAnimationParameter(
                        stResAllMismatchView, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, g_systemParameter.nLaneCount, g_systemParameter.bOSDS, g_systemParameter.bRollerOS);
                }
                if (m_pDlgGraphWindow)
                {
                    if (stResAllMismatchView.m_vecfDiffWidth.size() == 2)
                    {
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(TRUE, DefectType::Dimension_Mismatch1,
                            stResAllMismatchView.m_nCellIndex, stResAllMismatchView.m_vecfDiffWidth[0], true, _T("OK"));

                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(TRUE, DefectType::Dimension_Mismatch2,
                            stResAllMismatchView.m_nCellIndex, stResAllMismatchView.m_vecfDiffWidth[1], true, _T("OK"));
                    }
                    if (stResAllMismatchView.m_vecfDiffWidth.size() == 3)
                    {
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(TRUE, DefectType::Dimension_Mismatch1,
                            stResAllMismatchView.m_nCellIndex, stResAllMismatchView.m_vecfDiffWidth[0], true, _T("OK"));

                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(TRUE, DefectType::Dimension_Mismatch2,
                            stResAllMismatchView.m_nCellIndex, stResAllMismatchView.m_vecfDiffWidth[1], true, _T("OK"));
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[2]->SetData(TRUE, DefectType::Dimension_Mismatch3,
                            stResAllMismatchView.m_nCellIndex, stResAllMismatchView.m_vecfDiffWidth[2], true, _T("OK"));
                    }
                    if (stResAllMismatchView.m_vecfDiffWidth.size() == 4)
                    {
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(TRUE, DefectType::Dimension_Mismatch1,
                            stResAllMismatchView.m_nCellIndex, stResAllMismatchView.m_vecfDiffWidth[0], true, _T("OK"));

                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(TRUE, DefectType::Dimension_Mismatch2,
                            stResAllMismatchView.m_nCellIndex, stResAllMismatchView.m_vecfDiffWidth[1], true, _T("OK"));
                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[2]->SetData(TRUE, DefectType::Dimension_Mismatch3,
                            stResAllMismatchView.m_nCellIndex, stResAllMismatchView.m_vecfDiffWidth[2], true, _T("OK"));

                        m_pDlgGraphWindow->m_pvecDlgGraphFrame[3]->SetData(TRUE, DefectType::Dimension_Mismatch4,
                            stResAllMismatchView.m_nCellIndex, stResAllMismatchView.m_vecfDiffWidth[3], true, _T("OK"));
                    }
                    // batch Inspection 돌릴 때
                    ExportMismatchGraphData(stResAllMismatchView.m_nCellIndex, stResAllMismatchView.m_vecfDiffWidth);
                    // Animation SetData
                }
            }
        }
    }
    // ----[end] DisplayMismatchData ----
}
ResultSpec CIBCIDlg::LaneSpecData(std::vector<CIDISpecInfo> vecMeasureDefectUp, std::vector<ResultSpec> resvecLaneSpec, int nItemIdx)
{
    ResultSpec resSpec;

    resSpec.fMeasureStandard = vecMeasureDefectUp[nItemIdx].m_fMeasureStandard;
    resSpec.fMeasureOffsetUCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUCL;
    resSpec.fMeasureOffsetLCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLCL;
    resSpec.fMeasureOffsetUSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUSL;
    resSpec.fMeasureOffsetLSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLSL;

    return resSpec;
}

ResultMismatch CIBCIDlg::MismathchData(ResultMismatch stResAllMismatch, std::vector<ResultLaneData> resvecLaneUp,
    std::deque<ResultMismatch> m_vecMismatchData, std::vector<ResultSpec> resvecLaneSpec, float fFullLength, long n,int check)
{
    if (check==0)
    {
        stResAllMismatch.m_vecResUpLane = resvecLaneUp;
        stResAllMismatch.m_vecResDownLane = m_vecMismatchData[n].m_vecResDownLane;
        stResAllMismatch.m_vecResSpec = resvecLaneSpec;
        stResAllMismatch.m_fTopFullLength = fFullLength;
        stResAllMismatch.m_fBackFullLength = m_vecMismatchData[n].m_fBackFullLength;

        return stResAllMismatch;
    }
    else
    {
        stResAllMismatch.m_vecResDownLane = resvecLaneUp;
        stResAllMismatch.m_vecResUpLane = m_vecMismatchData[n].m_vecResUpLane;
        stResAllMismatch.m_vecResSpec = resvecLaneSpec;
        stResAllMismatch.m_fBackFullLength = fFullLength;
        stResAllMismatch.m_fTopFullLength = m_vecMismatchData[n].m_fTopFullLength;

        return stResAllMismatch;
    }
}

ResultLaneData CIBCIDlg::LaneUpData(std::vector<CIDISpecInfo> vecMeasureDefectUp, std::vector<ResultLaneData> resvecLaneUp, int nItemIdx)
{
    ResultLaneData resLaneUp;

    resLaneUp.fLaneWidth = (float)vecMeasureDefectUp[nItemIdx].LENGTH;

    return resLaneUp;
}


void CIBCIDlg::ResultMismatchData(
    long nSizeDivideVertical, std::vector<std::vector<CIDISpecInfo>> m_vvMeasureDefect, CString strLotID, long nCellID,Share::ResultVision resultdata)
{
    //InitializeResMismatchData(nSizeDivideVertical, true);
    //InitializeResMismatchData(nSizeDivideVertical);

    ResultMismatch stResAllMismatch; //Missmatch 변수 통합 Data로 빼야하나?
    std::vector<ResultLaneData> resvecLaneUp;
    std::vector<ResultSpec> resvecLaneSpec;
    float fFullLength = 0.f;
    stResAllMismatch.m_strLotID = strLotID;
    stResAllMismatch.m_nCellIndex = nCellID;
    for (int nVerticalIdx = 0; nVerticalIdx < nSizeDivideVertical; nVerticalIdx++)
    {
        auto& vecMeasureDefect = m_vvMeasureDefect[nVerticalIdx];

        long nSizeMeasureItem = (long)vecMeasureDefect.size();

#if INSPECTION_MACHINE_UC1
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_LeftFoil)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_LeftCoating)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_RightCoating)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_MiddleFoil)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_RightFoil)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_FullLength)
            {
                fFullLength = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
        }
#else
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_NonCoating_1)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_Coating_1)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_Coating_2)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_MiddleFoil)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_NonCoating_2)
            {
                resvecLaneUp.push_back(LaneUpData(vecMeasureDefect, resvecLaneUp, nItemIdx));

                resvecLaneSpec.push_back(LaneSpecData(vecMeasureDefect, resvecLaneSpec, nItemIdx));
            }
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_FullLength)
            {
                fFullLength = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
        }
#endif // INSPECTION_MACHINE_UC1

        
    }
    // if (g_systemParameter.machineType != MachineType::UC1_mismatch)
    {
        ResultMismatch stRes;
        stRes.m_strLotID = strLotID;
        stRes.m_vecResUpLane = resvecLaneUp;
        stRes.m_vecResDownLane = resvecLaneUp;
        stRes.m_vecResSpec = resvecLaneSpec;
        stRes.m_fTopFullLength = fFullLength;
        stRes.m_fBackFullLength = fFullLength;
        stRes.m_nCellIndex = nCellID;
        //m_inlineResultCoupler->Push1(stRes);
        m_inlineResultCoupler->Push3(stRes, resultdata);

        // OnSend
        if (g_systemParameter.bUseServer == 1)
        {
            CString strSend;
            strSend.Format(_T("MISSMATCH,%s,"), stRes.m_strLotID);
            strSend.AppendFormat(_T("%d,"), stRes.m_nCellIndex);
            strSend.AppendFormat(_T("%d,"), g_systemParameter.bDirection);
            strSend.AppendFormat(_T("%d,"), resvecLaneUp.size());

            for (int i = 0; i < resvecLaneUp.size(); i++)
                strSend.AppendFormat(_T("%f,"), resvecLaneUp[i].fLaneWidth);

            strSend.AppendFormat(_T("%f,"), fFullLength);

            if (g_systemParameter.bServer == 0) //client
            {
                OnSend2(strSend);
            }
            else if (g_systemParameter.bServer == 1) // server
            {
                OnSend2_S(strSend);
            }
        }
        // ----[end] DisplayMismatchData ----
    }
}