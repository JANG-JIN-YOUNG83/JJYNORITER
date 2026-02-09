
// MainDlgInline.cpp : 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "IBCIDlg.h"
#include "SystemParameter.h"

#include "DlgVIewGrabImage.h"
#include "DlgVIewCellImage.h"

//#include "InlineResultCoupler.h"
//#include "ViewGrabImageDlg.h"
//#include "ViewImageFocusDlg.h"
#include "CellImageDispatcher.h"
#include "CellImageProducer.h"
#include "CellImageMerger.h"
#include "ViewGrabImageUpdater.h"
#include "ViewCellImageUpdater.h"
//#include "DebugImageSaver.h"
//#include "ViewGrabImageUpdater.h"
//#include "ViewImageFocusUpdater.h"
//#include "ViewMainDlg.h"
//#include "CellImageSaver.h"
//#include "SaveImageQueue.h"
//#include "CDlgSyncgrabWindow.h"

#include "../Grab/CISComm.h"
#include "../Grab/GrabExecutor.h"
#include "../Grab/GrabExecutorContainer.h"
//#include "../SharedModule/VisionHostShare/HostCommSocket.h"
#include "../SyncPci/Syncboard.h"
//#include "../Inspection/Accessor.h"
//#include "../Inspection/InspectionCell.h"
//#include "../Inspection/InspectionConfiguration.h"
//#include "../Inspection/InspectionConfigurationPara.h"
//#include "../Module/CustomControl/CustomStatic.h"

#include "Algorithm/ImageProcessing.h"
#include "Algorithm/ProcessingBuffer.h"
#include "Gadget/TimeCheck.h"
#include "Widget/ImageView.h"

#include "CalMap_Distortion_Impl.h"
#include "CalMap_Distortion.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void CIBCIDlg::CallbackOnFrameMerged(void* callbackData, UINT horizontalSize, UINT verticalSize, DWORD mergeCount, GrabError error)
{
	((CIBCIDlg*)callbackData)->CallbackOnFrameMerged(horizontalSize, verticalSize, mergeCount, error);
}

void CIBCIDlg::CallbackOnFrameMerged(UINT horizontalSize, UINT verticalSize, DWORD mergeCount, GrabError error)
{
	// grabExecutor 사이즈만큼 head,tail 검색해서 Ready된 최소 갯수만큼 복사/ 콜백
	// m_pViewGrabImage visible이면 setImage;
	// grabExecutor의 head, tail proceed.


	auto curUsedFrames = m_cellImageMerger->m_tailFrameIndex - m_cellImageMerger->m_headFrameIndex;
	if (curUsedFrames > m_cellImageMerger->m_maxUsedFrames)
	{
		m_cellImageMerger->m_maxUsedFrames = curUsedFrames;
		m_loggerGrab.Log(0, _T("CellImageMerger Max Used Frames = %d."), m_cellImageMerger->m_maxUsedFrames);
	}

	// 처음 체크
	int64_t headFrameIndex = -1;  // Common
	int64_t tailFrameIndex = INT64_MAX; // Min
	for (long cnt = 0; cnt < m_cellImageMerger->m_sizeExecutor; cnt++)
	{
		GrabExecutor* grabExecutor = m_cellImageMerger->m_grabExecutor[cnt];

		int64_t head = 0;
		int64_t tail = 0;
		grabExecutor->GetUpdatedFrameIndices(head, tail);

		if (headFrameIndex == -1)
		{
			headFrameIndex = head;
		}
		else if (headFrameIndex != -1 && head != headFrameIndex)
		{
			m_loggerGrab.Log(0, _T("CallbackOnFrameMerged ERROR"));
		}
		tailFrameIndex = min(tail, tailFrameIndex);
	}

	if (headFrameIndex >= tailFrameIndex || headFrameIndex == -1)
	{
		return;
	}
    else if (m_cellImageMerger->m_grabExecutor[0]->GetGrabMode() == GrabMode::AREA)
    {
        if (headFrameIndex == 0)
        {
            long margin = g_systemParameter.nCuttingMargin;
            m_cellImageProducers->SetLatestSensorPosition(margin);
        }
        for (long cnt = 0; cnt < m_cellImageMerger->m_sizeExecutor; cnt++)
        {
            GrabExecutor* grabExecutor = m_cellImageMerger->m_grabExecutor[cnt];
            grabExecutor->ProceedHeadFrameIndex(tailFrameIndex - headFrameIndex);
        }

        ::InterlockedExchange((uint64_t*)&m_cellImageMerger->m_tailFrameIndex, tailFrameIndex);

        // ..
      /*  TRACE2("CellImageMerger %d, %d\n", tailFrameIndex, headFrameIndex);

        if (m_pViewGrabImage->IsWindowVisible())
        {
            int cameraIndex = 0;
            const unsigned char* pImage = m_cellImageMerger->GetFrameAddress(mergeCount);
            m_viewGrabImageUpdaters[2]->Push(IPVM::Image_8u_C1(horizontalSize, horizontalSize, verticalSize, pImage));
        }*/

        m_cellImageProducers->Trigger();
        return;
	}

	// copy
	for (long cameraIndex = 0; cameraIndex < m_cellImageMerger->m_sizeExecutor; cameraIndex++)
	{
		for (long frameIndex = headFrameIndex; frameIndex < tailFrameIndex; frameIndex++)
		{
            switch (m_cisComm->GetExecutorType())
            {
                case ExecutorType::Sapera:
                {
                    const auto pImage = m_cisComm->GetGrabExcutor(cameraIndex)->GetFrameAddress(frameIndex);

                    // merge start, todo:2024.03.29 merge buffer를 각 executor에서 사용하도록
                    auto pImageDst = m_cellImageMerger->GetFrameAddress(frameIndex);
                    int widthSrc = m_cisComm->GetGrabExcutor(cameraIndex)->GetHorizontalSize();
                    int heightSrc
                        = m_cisComm->GetGrabExcutor(cameraIndex)->GetVerticalSize() * 2; //todo:2024.03.29 merge option
                    int widthDst = m_cellImageMerger->GetHorizontalSize();
                    int heightDst = m_cellImageMerger->GetVerticalSize();
                    const IPVM::Image_8u_C1 imageSrc(widthSrc, widthSrc, heightSrc, pImage);
                    IPVM::Image_8u_C1 imageDst(widthDst, widthDst, heightDst, pImageDst);

                    int offsetDst = 0; // widthDst > widthSrc
                    if (cameraIndex == 0)
                    {
                        //offsetDst = 14288;
                        offsetDst = m_cisComm->GetGrabExcutor(0)->GetHorizontalSize();
                    }

                    if (m_cisComm->GetGrabExcutor(cameraIndex)->GetSensorImageMirror())
                    {
                        if (cameraIndex == 0)
                        {
                            //offsetDst = 0;
                            offsetDst = m_cisComm->GetGrabExcutor(0)->GetHorizontalSize();
                        }
                        else
                        {
                            //offsetDst = 14288;
                            //offsetDst = m_cisComm->GetGrabExcutor(0)->GetHorizontalSize();
                            offsetDst = 0;
                        }
                    }

                    for (long y = 0; y < heightSrc; y++)
                    {
                        const auto pSrc = imageSrc.GetMem(0, y);
                        auto pDst = imageDst.GetMem(offsetDst, y);
                        memcpy(pDst, pSrc, widthSrc);
                    }
                    // end merge
                    break;
                }
                case ExecutorType::iGrab:
                {
                    const auto pImage = m_cisComm->GetGrabExcutor(cameraIndex)->GetFrameAddress(frameIndex);

                    auto pImageDst = m_cellImageMerger->GetFrameAddress(frameIndex);
                    int widthSrc = m_cisComm->GetGrabExcutor(cameraIndex)->GetHorizontalSize();
                    int heightSrc = m_cisComm->GetGrabExcutor(cameraIndex)->GetVerticalSize();
                    int widthDst = m_cellImageMerger->GetHorizontalSize();
                    int heightDst = m_cellImageMerger->GetVerticalSize();
                    const IPVM::Image_8u_C1 imageSrc(widthSrc, widthSrc, heightSrc, pImage);
                    IPVM::Image_8u_C1 imageDst(widthDst, widthDst, heightDst, pImageDst);
                    int offsetDst = 0; // widthDst > widthSrc
                    if (cameraIndex == 1)
                    {
                        offsetDst = 14688;
                    }
                    if (cameraIndex == 2)
                    {
                        offsetDst = 29376;
                    }

                    if (m_cisComm->GetGrabExcutor(cameraIndex)->GetSensorImageMirror())
                    {
                        if (cameraIndex == 0)
                        {
                            offsetDst = 21600;
                        }
                        else if(cameraIndex == 1)
                        {
                            offsetDst = 6912;
                        }
                        else
                        {
                            offsetDst = 0;
                        }
                    }

                    for (long y = 0; y < heightSrc; y++)
                    {
                        const auto pSrc = imageSrc.GetMem(0, y);
                        auto pDst = imageDst.GetMem(offsetDst, y);
                        memcpy(pDst, pSrc, widthSrc);
                    }
                    break;
                }
                case ExecutorType::Insnex: //jjy
                {
                
					const auto pTopImage = m_cisComm->GetGrabExcutor(cameraIndex)->GetFrameAddress(frameIndex);
                    const auto pBottopImage = m_cisComm->GetGrabExcutor(cameraIndex)->GetSecFrameAddress(frameIndex);
					
					auto pImageTopDst = const_cast<unsigned char*> ( m_cellImageMerger->GetFrameAddress(frameIndex));
                    auto pImageBottomDst = const_cast<unsigned char*> (m_cellImageMerger->GetSecFrameAddress(frameIndex));
                    
					int widthSrc = m_cisComm->GetGrabExcutor(cameraIndex)->GetHorizontalSize();
                    int heightSrc = m_cisComm->GetGrabExcutor(cameraIndex)->GetVerticalSize(); //todo:2024.03.29 merge option
   
					size_t imageSize = widthSrc * heightSrc;
                    memcpy(pImageTopDst, pTopImage, imageSize);
                    memcpy(pImageBottomDst, pBottopImage, imageSize);																   
																							   
																							   
	//               int widthDst = m_cellImageMerger->GetHorizontalSize();
     //               int heightDst = m_cellImageMerger->GetVerticalSize();
     //              
					//const IPVM::Image_8u_C1 imageTopSrc(widthSrc, widthSrc, heightSrc, pTopImage);
     //               IPVM::Image_8u_C1 imageTopDst(widthDst, widthDst, heightDst, pImageTopDst);

					//const IPVM::Image_8u_C1 imageBottomSrc(widthSrc, widthSrc, heightSrc, pBottopImage);
     //               IPVM::Image_8u_C1 imageBottomDst(widthDst, widthDst, heightDst, pImageBottomDst);
				
				 //   int offsetDst = 0; // widthDst > widthSrc
     //                                 
     //               
					////offsetDst = m_cisComm->GetGrabExcutor(0)->GetHorizontalSize();
     //             					
					//
					//for (long y = 0; y < heightSrc; y++)
     //               {
     //                   const auto pTopSrc = imageTopSrc.GetMem(0, y);
     //                   auto pTopDst = imageTopDst.GetMem(offsetDst, y);
     //                   memcpy(pTopDst, pTopSrc, widthSrc);
     //              

					//	const auto pBottomSrc = imageBottomSrc.GetMem(0, y);
     //                   auto pBottomDst = imageBottomDst.GetMem(offsetDst, y);
     //                   memcpy(pBottomDst, pBottomSrc, widthSrc);
					//
					//}
					
								
				
				
				}
                break;

                default:;
            } // end switch
		}
	}
   
    //m_cellImageSaver->PushBufferAlarmImage(image, (nCam == 0));
	// proceed
	if (headFrameIndex == 0)
	{
		long margin = g_systemParameter.nCuttingMargin;
		m_cellImageProducers->SetLatestSensorPosition(margin);
	}
	for (long cnt = 0; cnt < m_cellImageMerger->m_sizeExecutor; cnt++)
	{
		GrabExecutor* grabExecutor = m_cellImageMerger->m_grabExecutor[cnt];
		grabExecutor->ProceedHeadFrameIndex(tailFrameIndex - headFrameIndex);
	}

	::InterlockedExchange((uint64_t*)&m_cellImageMerger->m_tailFrameIndex, tailFrameIndex);

	
	m_cellImageProducers->Trigger();
}

void CIBCIDlg::CallbackOnFrameUpdated(void* callbackData, int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error)
{
	((CIBCIDlg*)callbackData)->CallbackOnFrameUpdated(cameraIndex, horizontalSize, verticalSize, grabCount, error);
}

void CIBCIDlg::CallbackOnFrameUpdated_Area(void* callbackData, int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error)
{
	((CIBCIDlg*)callbackData)->CallbackOnFrameUpdated_Area(cameraIndex, horizontalSize, verticalSize, grabCount, error);

}

#define FIRST_LINE 0

enum enLineHead
{
	LH_DYMMY = 0,
	CELLID_POSITION = 1,
	LINE_ERROR_CHECK_POSITION = 2,
};
void CIBCIDlg::CallbackOnFrameUpdated(int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error)
{
	
	if (error == GrabError::OverRun)
	{
		
		return;
	}

	const auto frameIndex = grabCount - 1;
	const auto pCh1Image = m_cisComm->GetGrabExcutor(cameraIndex)->GetFrameAddress(frameIndex);
    const auto pCh2Image = m_cisComm->GetGrabExcutor(cameraIndex)->GetSecFrameAddress(frameIndex);

	IPVM::Image_8u_C1 srcTop(horizontalSize, horizontalSize, verticalSize, pCh1Image);
    IPVM::Image_8u_C1 srcBottom(horizontalSize, horizontalSize, verticalSize, pCh2Image);
	
	auto cameraName = (cameraIndex == 0) ? _T("Up") : _T("Dn");
			
	BYTE prevLineCheckValue = 0;
	bool bFirstIdLine = true;
	
	//if (g_systemParameter.nInspectionMode != InspectionMode::Grabonly)
	int sensorID = ReadSyncCellID();
	//sensorID = m_cellImageProducers->FrontSyncCellID();
	sensorID = m_pSyncIO->GetCell_ID();
	if (cameraIndex == 0)
	{
		m_cellImageProducers->PushSensorInfo(-1, sensorID);
		TRACE1("=== Sensor ID = %d\r\n", sensorID);
        m_nCellId = sensorID;
	}
	
	m_cellImageMerger->TriggerMerge(frameIndex, cameraIndex, true); /// 요고 변형 하면 될것 같은데.......

	int nindex = frameIndex % 51;

    int imageWidth = srcTop.GetSizeX();
    int imageHeight = srcTop.GetSizeY();

    int imageHeightDst = imageHeight * 51; // 10200

    auto TopDst = m_cisComm->GetGrabExcutor(cameraIndex)->GetSumAddress_Top(0); // 목적지 버퍼
    auto bottomDst = m_cisComm->GetGrabExcutor(cameraIndex)->GetSumAddress_Bottom(0); //목적지 버퍼

    IPVM::Image_8u_C1 imageTopSrc(imageWidth, imageWidth, imageHeightDst, TopDst);
    IPVM::Image_8u_C1 imageBottomSrc(imageWidth, imageWidth, imageHeightDst, bottomDst);
    
    BYTE* image_TotalTopdst = imageTopSrc.GetMem();
    BYTE* image_TotalBottomdst = imageBottomSrc.GetMem();

    // 원본 이미지 포인터
    auto* image_src = srcTop.GetMem();
    auto* image_src_sec = srcBottom.GetMem();

    int copySize = imageWidth * imageHeight;
    int offset = nindex * copySize;

    BYTE* dst_offset = image_TotalTopdst + offset;
    BYTE* dst_offset_sec = image_TotalBottomdst + offset;

    memcpy(dst_offset, image_src, copySize);
    memcpy(dst_offset_sec, image_src_sec, copySize);



	 if (frameIndex != 0 && frameIndex % 50 == 0)
    {
        if (m_pViewGrabImage->IsWindowVisible())
        {
            //UINT ver = verticalSize;
            UINT ver = (g_systemParameter.machineType == MachineType::ES_mismatch
                           || g_systemParameter.machineType == MachineType::ES_rollpress_pinhole
                           || g_systemParameter.machineType == MachineType::ES_rollpress_surface)
                ? verticalSize * 2
                : verticalSize;

            if (g_systemParameter.bDisplayShow == TRUE)
            {
                //jjy
                m_viewGrabImageUpdaters[cameraIndex]->Push(
                    IPVM::Image_8u_C1(horizontalSize, horizontalSize, imageHeightDst, TopDst));
                m_viewGrabImageUpdaters[cameraIndex + 1]->Push(
                    IPVM::Image_8u_C1(horizontalSize, horizontalSize, imageHeightDst, bottomDst));
            }
        }
    }

}

void CIBCIDlg::CallbackOnFrameUpdated_Area(int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error)
{
	//shared image queue에서 넘어온다면 적혀있는 cell 정보를 빼와야 한다. 

	if (error == GrabError::OverRun)
	{
		//if (!m_pSyncIO->GetOutputBit(SyncboardOutput::VisionAlarm))
		//{
		//	// Grab Buffer를 초과하여 영상이 얻어지고 있다.
		//	// 파단 등에 의해 정상적이지 않은 영상이 들어와 셀 Cutting에 문제가 생기거나
		//	// 획득에 문제가 생기면 발생할 수 있다.
		//	// Host와 PLC에 각각 알람을 전달한다

		//	//m_pClientSocket->SendAlarm_CIS(130 /*임시*/,
		//	//	Share::ALARM_LEVEL::red,
		//	//	_T(""),
		//	//	_T("CAMERA OVERRUN"),
		//	//	_T("Camera Alarm Occured"),
		//	//	_T(""));

		//	m_pSyncIO->SetSignal_HandlerOutput(SyncboardOutput::VisionAlarm, TRUE);
		//}
		return;
	}

	const auto frameIndex = grabCount;
	
	auto cameraName = (cameraIndex == 0) ? _T("Up") : _T("Dn");

	// merge start, todo:2024.03.29 merge buffer를 각 executor에서 사용하도록
	CString debug;
	debug.Format(_T("CallbackOnFrameUpdated%d %d\n"), cameraIndex, frameIndex);
	OutputDebugString(debug);

	  //todo:2024.03.29 merge option
	int widthDst = m_cellImageMerger->GetHorizontalSize();
	int heightDst = m_cellImageMerger->GetVerticalSize();
	auto pImageDst = m_cellImageMerger->GetFrameAddress(frameIndex);
	IPVM::Image_8u_C1 imageDst(widthDst, widthDst, heightDst, pImageDst);
	IPVM::Rect Rect_Merge(0, 0, 0, 0);
	
	for (auto nindex = 0; nindex < m_cisComm->GetCameraCount(); nindex++)
	{
		const auto pImage = m_cisComm->GetGrabExcutor(nindex)->GetFrameAddress(frameIndex);
		int widthSrc = m_cisComm->GetGrabExcutor(nindex)->GetHorizontalSize();
		int heightSrc = m_cisComm->GetGrabExcutor(nindex)->GetVerticalSize();
		IPVM::Image_8u_C1 imageSrc(widthSrc, widthSrc, heightSrc, pImage);
        IPVM::Image_8u_C1 imageCalc(widthSrc, heightSrc);

		//왜곡보정
        if (m_vecpDistortion[nindex]->IsLoadSuccess() == TRUE)
        {
            m_vecpDistortion[nindex]->Modify(imageSrc, CPoint(0, 0), imageCalc);
            memcpy(imageSrc.GetMem(), imageCalc.GetMem(), widthSrc * heightSrc * sizeof(unsigned char));
		}



		//이미지 크기 및 복사 위치 계산
		if(nindex == 0)
		{ 
			Rect_Merge = IPVM::Rect(imageSrc);
		}
		else
		{
			Rect_Merge = IPVM::Rect(imageSrc);
			Rect_Merge.m_left = IPVM::Rect(imageSrc).Width() * nindex;
			Rect_Merge.m_right = Rect_Merge.m_right + IPVM::Rect(imageSrc).Width() * nindex;
		}
		

		IPVM::ImageProcessing::Copy(imageSrc, IPVM::Rect(imageSrc), Rect_Merge, imageDst);
	}
	
	CString strTestPath = _T("");
	strTestPath.Format(_T("H:\\Area_Murge_%d.bmp"), frameIndex);
	
	//imageDst.SaveBmp(strTestPath); 


	/*IPVM::Image_8u_C1 image;
	image.Create(horizontalSize, verticalSize);
	memcpy(image.GetMem(), pImage, horizontalSize * verticalSize);*/

	BYTE prevLineCheckValue = 0;
	bool bFirstIdLine = true;

	CString strCamera = (cameraIndex == 0) ? _T("[UP]") : _T("[DOWN]");


	//if (g_systemParameter.nInspectionMode != InspectionMode::Grabonly)
	/*int sensorID = ReadSyncCellID();
	sensorID = m_cellImageProducers[cameraIndex]->FrontSyncCellID();*/
    int sensorID = ReadSyncCellID();
    //sensorID = m_cellImageProducers->FrontSyncCellID();
    sensorID = m_pSyncIO->GetCell_ID();

    if (cameraIndex == 0)
    {
        m_cellImageProducers->PushSensorInfo(-1, sensorID);
        TRACE1("=== Sensor ID = %d\r\n", sensorID);
    }

	if (m_pViewGrabImage->IsWindowVisible())
	{
		UINT ver = verticalSize;
		if (g_systemParameter.machineType == MachineType::ES_mismatch || g_systemParameter.machineType == MachineType::ES_rollpress_surface)
			ver *= 2;
		
		m_viewGrabImageUpdaters[cameraIndex]->Push(IPVM::Image_8u_C1(imageDst.GetSizeX(), imageDst.GetSizeX(), imageDst.GetSizeY(), imageDst.GetMem()));
	}
    m_cellImageMerger->m_headFrameIndex = frameIndex;
    m_cellImageMerger->m_tailFrameIndex = frameIndex + 1;
    m_cellImageMerger->Trigger();
}

void CIBCIDlg::CallbackOnCellImageProduced(void* callbackData, InspectionInputData& inputData)
{
	((CIBCIDlg*)callbackData)->CallbackOnCellImageProduced(inputData);
}

void CIBCIDlg::CallbackOnCellImageProduced(InspectionInputData& inputData)
{
    
	if (m_pViewCellImage->IsWindowVisible() && m_pViewCellImage->m_nModeUpdate == ENUM_VIEW_MODE_CELL_IMAGE::UPDATE_ALWAYS)
	{

       m_viewCellImageUpdater->SetGrayMode(m_pViewCellImage->m_nModeGray);
	   m_viewCellImageUpdater->Push(inputData.m_imageBright, inputData.m_imageDark, inputData.m_cellID);        
	}

	m_cellImageDispathcer->Push(inputData); 
}



BOOL CIBCIDlg::ReadSyncCellID()
{
	CSingleLock lock(&m_csCellID, TRUE);
	long CellID;

	//if (ExecutorType::iGrab != m_cisComm->GetExecutorType())
	if (false) // g_systemparameter->useEncoderModule
	{
		CellID = m_nvirtualid;
		m_nvirtualid++;
	}
	else
		CellID = m_pSyncIO->GetCell_ID();

	if (-1 != CellID)
	{
		m_cellImageProducers->PushSyncCellID(CellID);
		//m_loggerJudge.Log(0, _T("[Insert SyncCellID] CellID : %d, UP Queue size : %d, DOWN Queue size : %d "), CellID, m_cellImageProducers[0]->SizeSyncCellID(), m_cellImageProducers[1]->SizeSyncCellID());
		return true;
	}
	else
	{
		return false;
	}
}