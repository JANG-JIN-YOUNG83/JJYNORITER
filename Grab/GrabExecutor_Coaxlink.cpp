//#include "../StdAfx.h"
#include "stdafx.h"
#include "GrabExecutor_Coaxlink.h"

#include "../Module/SharedImageQueue/ImageQueuePusher.h"
#include "Base/LoggerInterface.h"
#include <Types/Image_8u_C1.h>


GrabExecutor_Coaxlink::GrabExecutor_Coaxlink(IPVM::LoggerInterface& logger, const int cameraIndex, const int grabberIndex)
: m_nBuffer_cnt(BUFFER_CNT)
, m_bOpen(false)
, m_bGrabbing(false)
//, m_pCoaxlink(NULL)
, m_nContext(NULL)
, m_bSaveImage(false)
, m_nSave(0)
, m_grabberIndex(grabberIndex)
, m_cameraIndex(cameraIndex)
, m_strVenderName(_T(""))
, m_nToTalCameraCnt(0)
, m_nCamNum(0)
, m_bReleaseGrabDoneSign(TRUE)
{
	m_pCoaxlink[0] = NULL;
	m_pCoaxlink[1] = NULL;
	m_pCoaxlink[2] = NULL;
	m_pCoaxlink[3] = NULL;
	m_pDC = NULL;

	//BMP
	m_pBitmapInfo = NULL;
	m_pBitmapInfo = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFO)];
	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	//m_pBitmapInfo->bmiHeader.biBitCount = 24;
	m_pBitmapInfo->bmiHeader.biBitCount = 8;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = 0;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;
	
	m_pBitmapInfo->bmiHeader.biWidth = 0;
	m_pBitmapInfo->bmiHeader.biHeight = 0;
	//BMP

	

	memset(&m_sCallbackGrab, 0, sizeof(m_sCallbackGrab));
	


	Open(logger);
	//SetConfigure(MODE_CUSTOM);

	//InitializeCircularBuffer();

	//m_queuePusher = new SharedMemory::ImageQueuePusher(cameraIndex);

	//enableEvent<NewBufferData>();

	//m_GenTL.tlClose(tl);
}

GrabExecutor_Coaxlink::~GrabExecutor_Coaxlink(void)
{
	if(m_pBitmapInfo!=NULL)
	{
		delete m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}
}

bool GrabExecutor_Coaxlink::Open(IPVM::LoggerInterface& logger)
{
	bool res = false;

	if(m_pCoaxlink[0] ==NULL)
	{
		try
		{
			std::string strname;
			
			//EGenTL genTL;
			gc::TL_HANDLE tl = m_GenTL.tlOpen();
			uint32_t numInterfaces = m_GenTL.tlGetNumInterfaces(tl);
			for (uint32_t interfaceIndex = 0; interfaceIndex < numInterfaces; interfaceIndex++) {
				std::string interfaceID = m_GenTL.tlGetInterfaceID(tl, interfaceIndex);
				gc::IF_HANDLE interfaceHandle = m_GenTL.tlOpenInterface(tl, interfaceID);
				uint32_t numDevice = m_GenTL.ifGetNumDevices(interfaceHandle);
				for (uint32_t deviceIndex = 0; deviceIndex < numDevice; deviceIndex++) {
					std::string deviceID = m_GenTL.ifGetDeviceID(interfaceHandle, deviceIndex);
					gc::DEV_HANDLE deviceHandle = m_GenTL.ifOpenDevice(interfaceHandle, deviceID);
					try {
						if (m_GenTL.devGetPort(deviceHandle)) {
                            
							m_pCoaxlink[deviceIndex] = new CCoaxlinkGrabber(logger, m_GenTL, interfaceIndex, deviceIndex, 0, this);
							m_Width = m_pCoaxlink[deviceIndex]->getInteger<RemoteModule>("Width");
							//m_Height		= m_pCoaxlink->getInteger<StreamModule>("ScanLength");		//LineScan
							m_Height = m_pCoaxlink[deviceIndex]->getInteger<RemoteModule>("Height");			//AreaScan

							PixelComponentCount = m_pCoaxlink[deviceIndex]->getInteger<StreamModule>("PixelComponentCount");
							//SetAllEvent();
							// ** Auto Buffer Allocation
							m_pCoaxlink[deviceIndex]->reallocBuffers(m_nBuffer_cnt, m_Width * m_Height * PixelComponentCount);
							strname = m_pCoaxlink[deviceIndex]->getString<DeviceModule>("DeviceVendorName");
							m_strVenderName = strname.c_str();
							//m_verticalSize = m_Width;
							//m_horizontalSize = m_Height;

							m_pCoaxlink_config = m_pCoaxlink[deviceIndex];

							SetConfigure(MODE_CUSTOM);
                            //SetConfigure(MODE_CONTINUOUS);// 트리거 일단 안받도록 수정
                            
							

							//enableEvent<NewBufferData>();

							m_bOpen = true;

							res = true;
							m_nToTalCameraCnt = deviceIndex + 1;
						}
					}
					catch (const gentl_error&) {
					}
					
				}

			}
			
			/*
			EGenTL m_GenTL;

			gc::TL_HANDLE tl = m_GenTL.tlOpen();

			std::string interfaceID = m_GenTL.tlGetInterfaceID(tl, 0);
			gc::IF_HANDLE interfaceHandle = m_GenTL.tlOpenInterface(tl, interfaceID);
			uint32_t numDevice = m_GenTL.ifGetNumDevices(interfaceHandle);
			std::string deviceID = m_GenTL.ifGetDeviceID(interfaceHandle, m_grabberIndex);
			gc::DEV_HANDLE deviceHandle = m_GenTL.ifOpenDevice(interfaceHandle, deviceID);
			if (m_GenTL.devGetPort(deviceHandle))
			*/
			/*
			m_pCoaxlink = new CCoaxlinkGrabber(m_GenTL, m_cameraIndex, m_grabberIndex, 0, this);

			m_Width				= m_pCoaxlink->getInteger<RemoteModule>("Width");
			//m_Height		= m_pCoaxlink->getInteger<StreamModule>("ScanLength");		//LineScan
			m_Height			= m_pCoaxlink->getInteger<RemoteModule>("Height");			//AreaScan

			PixelComponentCount	= m_pCoaxlink->getInteger<StreamModule>("PixelComponentCount");
			//SetAllEvent();
			// ** Auto Buffer Allocation
			m_pCoaxlink->reallocBuffers(m_nBuffer_cnt, m_Width*m_Height*PixelComponentCount);
			strname = m_pCoaxlink->getString<DeviceModule>("DeviceVendorName");
			m_strVenderName = strname.c_str();
			m_verticalSize = m_Width;
			m_horizontalSize = m_Height;

			m_bOpen = true;

			res = true;
			*/
		}
		catch (const std::exception &e)
		{
			//m_GenTL.memento(e.what());
			//TRACE("%s\n", e.what());
			//m_bOpen = false;
			//res = false;
		}

	}

	return res;
}

int GrabExecutor_Coaxlink::GetPixelComponentCount()
{
	return PixelComponentCount;
}

void GrabExecutor_Coaxlink::Close(void)
{
	if(m_bGrabbing)
		Stop();

	for (auto i = 0; i < 4; i++)
	{
		if (m_pCoaxlink[i] != NULL)
		{
			delete m_pCoaxlink[i];
			m_pCoaxlink[i] = NULL;
		}
	}
	

	m_bOpen = false;
}

void GrabExecutor_Coaxlink::Grab(uint64_t grab_cnt/* = GENTL_INFINITE*/)
{

	if(!m_bGrabbing)
	{
		m_pCoaxlink[m_nCamNum]->m_nImageCnt = 0;
        if (grab_cnt == 0)
        {
            m_pCoaxlink[m_nCamNum]->start(GENTL_INFINITE);
		}
        else
        {
            m_pCoaxlink[m_nCamNum]->start(grab_cnt);
		}
		
		//m_bGrabbing = true;
		m_pCoaxlink[m_nCamNum]->memento("Grab started");

	}
}

void GrabExecutor_Coaxlink::Stop(void)
{
	try
	{
		m_pCoaxlink[m_nCamNum]->stop();
	}
	catch (std::exception& )
	{
		m_pCoaxlink[m_nCamNum]->memento("m_Coaxlink.Stop() - Timeout");
	}

	m_bGrabbing = false;
}

void GrabExecutor_Coaxlink::SetBufferCount(uint64_t buffer_cnt)
{
	//if you change the number of buffers, must be call this function before "Grab" function
	m_nBuffer_cnt = buffer_cnt;	
}

void GrabExecutor_Coaxlink::SetConfigure(Grabmode_t mode)
{
	switch (mode)
	{
	case MODE_CONTINUOUS:
		// Camera
		setString<RemoteModule>("TriggerMode", "Off"); // On, Off
		setString<RemoteModule>("AcquisitionMode", "Continuous"); // Continuous, SingleFrame, MultiFrame
		setString<DeviceModule>("CameraControlMethod", "NC");
		setString<RemoteModule>("ExposureMode", "Timed");
		setFloat<RemoteModule>("ExposureTime", 10000.0);

		break;

	case MODE_HARDTRIGGER_FG:
		setString<RemoteModule>("TriggerMode", "On"); // On, Off
		setString<RemoteModule>("TriggerSource", "CXPin"); //LineIn0, CXPin
		setString<DeviceModule>("CameraControlMethod", "RC"); // NC, RC, RG
		setInteger<DeviceModule>("ExposureReadoutOverlap", true);
		setFloat<DeviceModule>("ExposureRecoveryTime", 0);
		setFloat<DeviceModule>("StrobeDuration", 10.0);
		setString<DeviceModule>("CycleTriggerSource", "MDV1"); // Immediate, StartCycle, LIN1, LIN2, LIN3, LIN4, MDV1
		setFloat<DeviceModule>("CycleMinimumPeriod", 10.0);
		setString<DeviceModule>("CxpTriggerMessageFormat", "Pulse");
		setFloat<DeviceModule>("CxpTriggerAckTimeout", 20.0);
		setString<InterfaceModule>("LineInputToolSelector", "LIN1"); 
		setString<InterfaceModule>("LineInputToolSource", "IIN11"); 
		setString<InterfaceModule>("LineInputToolActivation", "RisingEdge");
		setString<InterfaceModule>("MultiplierDividerToolSource", "LIN1");
		setString<InterfaceModule>("MultiplierDividerToolOutputControl", "Enable");	//Enable, Disable
		setFloat<InterfaceModule>("MultiplierDividerToolMultiplicationFactor", 1.0);
		setFloat<InterfaceModule>("MultiplierDividerToolDivisionFactor", 1.0);
		break;

	case MODE_HARDTRIGGER_CAM:
		setString<RemoteModule>("TriggerMode", "On"); // On, Off
		setString<RemoteModule>("TriggerSource", "CXPin"); //LineIn0, CXPin
		setString<RemoteModule>("TriggerRescalerMode", "On");	//On, Off
		setFloat<RemoteModule>("TriggerRescalerRate", 9.0);
		setString<DeviceModule>("CameraControlMethod", "RG"); // NC, RC, RG
		setInteger<DeviceModule>("ExposureReadoutOverlap", true);
		setFloat<DeviceModule>("ExposureRecoveryTime", 10.0);
		setFloat<DeviceModule>("StrobeDuration", 10.0);
		setString<DeviceModule>("CycleTriggerSource", "LIN1"); // Immediate, StartCycle, LIN1, LIN2, LIN3, LIN4, MDV1
		setFloat<DeviceModule>("CycleTargetPeriod", 10.0);
		setString<InterfaceModule>("LineInputToolSelector", "LIN1"); 
		setString<InterfaceModule>("LineInputToolSource", "IIN11"); 
		setString<InterfaceModule>("LineInputToolActivation", "RisingEdge");
		break;
	case MODE_SETTING:
		//setString<InterfaceModule>("LineSelector", "IIN21");
		//setString<InterfaceModule>("LineSelector", "IIN11");
		setString<InterfaceModule>("LineInputToolSelector", "LIN1");
		setString<InterfaceModule>("LineInputToolSource", "IIN11");
		//setString<InterfaceModule>("LineInputToolSource", "IIN21");
		//setString<InterfaceModule>("LineInputToolSource", "IIN21");
		setString<InterfaceModule>("LineInputToolActivation", "RisingEdge");
		//setString<DeviceModule>("CameraControlMethod", "RC"); //원본
		setString<DeviceModule>("CameraControlMethod", "RC");
		setString<DeviceModule>("CycleTriggerSource", "LIN1");
		//setFloat<DeviceModule>("CxpTriggerAckTimeout", 0.0);
		setString<RemoteModule>("TriggerMode", "On"); // On, Off
		setString<RemoteModule>("TriggerSource", "CXPin"); //LineIn0, CXPin
		setString<RemoteModule>("ExposureMode", "Timed");
		//setFloat<RemoteModule>("ExposureTime", 1000.0);
		setFloat<RemoteModule>("ExposureTime", 13);
		setString<RemoteModule>("CxpLinkConfiguration", "CXP6_X4");
		//setString<StreamModule>("BayerMethod", "Disable");
		break;	
	case MODE_CXP6_X2:
		setString<RemoteModule>("CxpLinkConfiguration", "CXP6_X2");
		break;	
	case MODE_CXP6_X4:
		setString<RemoteModule>("CxpLinkConfiguration", "CXP6_X4");
		break;
	case MODE_CUSTOM:
        setString<InterfaceModule>("LineSelector", "IIN11");
        setString<InterfaceModule>("LineInputToolSelector", "LIN1");
        setString<InterfaceModule>("LineInputToolSource", "IIN11");
		setString<RemoteModule>("TriggerMode", "On"); // On, Off
		setString<RemoteModule>("TriggerSource", "CXPin"); //LineIn0, CXPin
		setString<RemoteModule>("ExposureMode", "Timed");
		//setFloat<RemoteModule>("ExposureTime", 1000.0);
		setFloat<RemoteModule>("ExposureTime", 5000);
		//후아레이는, NumberOfLink를 사용하는 듯 함
		if(m_strVenderName == _T("VIEWORKS"))
			setString<RemoteModule>("CxpLinkConfiguration", "CXP6_X1");
		//setString<StreamModule>("BayerMethod", "Disable");
		break;

	default:
		break;
	}

	m_Grabmode = mode;

}

void GrabExecutor_Coaxlink::SetAllEvent()
{
	setString<DeviceModule>("EventSelector", "CameraTriggerRisingEdge");
	setInteger<DeviceModule>("EventNotification", true);

	setString<DeviceModule>("EventSelector", "CameraTriggerFallingEdge");
	setInteger<DeviceModule>("EventNotification", true);

	setString<DeviceModule>("EventSelector", "StrobeRisingEdge");
	setInteger<DeviceModule>("EventNotification", true);

	setString<DeviceModule>("EventSelector", "StrobeFallingEdge");
	setInteger<DeviceModule>("EventNotification", true);

	setString<DeviceModule>("EventSelector", "AllowNextCycle");
	setInteger<DeviceModule>("EventNotification", false);

	setString<DeviceModule>("EventSelector", "DiscardedCicTrigger");
	setInteger<DeviceModule>("EventNotification", true);

	setString<DeviceModule>("EventSelector", "PendingCicTrigger");
	setInteger<DeviceModule>("EventNotification", false);

	setString<DeviceModule>("EventSelector", "CxpTriggerAck");
	setInteger<DeviceModule>("EventNotification", false);

	setString<DeviceModule>("EventSelector", "CxpTriggerResend");
	setInteger<DeviceModule>("EventNotification", false);

	setString<DeviceModule>("EventSelector", "Trigger");
	setInteger<DeviceModule>("EventNotification", false);

	//enableEvent<CicData>();

 	setString<StreamModule>("EventSelector", "StartOfCameraReadout");
 	setInteger<StreamModule>("EventNotification", true);
 
 	setString<StreamModule>("EventSelector", "EndOfCameraReadout");
 	setInteger<StreamModule>("EventNotification", true);
 
 	setString<StreamModule>("EventSelector", "StartOfScan");
 	setInteger<StreamModule>("EventNotification", true);
 
 	setString<StreamModule>("EventSelector", "EndOfScan");
 	setInteger<StreamModule>("EventNotification", true);
	
	setString<StreamModule>("EventSelector", "RejectedFrame");
	setInteger<StreamModule>("EventNotification", true);

	setString<StreamModule>("EventSelector", "RejectedScan");
	setInteger<StreamModule>("EventNotification", true);
	enableEvent<DataStreamData>();

	setString<InterfaceModule>("EventSelector", "LIN1");
	setInteger<InterfaceModule>("EventNotification", true);
}

void GrabExecutor_Coaxlink::OnImageEvent(unsigned char* pBuffer)
{
	/*
	Image_t img;
	img.Index = m_nImageCnt;
	img.Width = m_Width;
	img.Height = m_Height;
	img.Buffer = (void*)pBuffer;

	void* m_pBuffer = (void*)pBuffer;
	int nWidth = m_Width;
	int nHeight = m_Height;

	m_pBitmapInfo->bmiHeader.biWidth = nWidth;
	m_pBitmapInfo->bmiHeader.biHeight = -nHeight;
	
	if(m_sCallbackGrab.m_pFuncCallback)
	{
		if(::IsWindow(m_sCallbackGrab.m_hwndOwner))
		{
			m_sCallbackGrab.m_pFuncCallback(m_sCallbackGrab.m_pUserData, *m_pBitmapInfo, pBuffer);
		}
		else
		{
			memset(&m_sCallbackGrab, 0, sizeof(m_sCallbackGrab));
		}
	}
	m_nSave++;

	return;*/
	//ScopedBuffer buf(*this, Data);
	
	
	Image_t img;
	img.Index = m_nImageCnt;
	img.Width = m_Width;
	img.Height = m_Height;
	img.Buffer = pBuffer;
	
	BYTE* m_pBuff_Insp = (BYTE*)((GrabExecutor*)m_pCoaxlink[m_grabberIndex])->GetFrameAddress(img.Index);
	int nWidth = m_Width;
	int nHeight = m_Height;

	//IPVM::Image_8u_C1 imgTest(nWidth, nHeight);
	//memcpy(imgTest.GetMem(), img.Buffer, nWidth * nHeight);
	//CString strName = _T("");
	//strName.Format(_T("D:\\idiotHun%d.bmp"), m_cameraIndex);
	//imgTest.SaveBmp(strName);
	
	memcpy(m_pBuff_Insp, img.Buffer, nWidth * nHeight);
	
	if (m_cameraIndex == 0)
	{
		//임시로, 하드코딩 동기화
		Sleep(40);
		//((GrabExecutor*)m_pCoaxlink[m_grabberIndex])->CallbackOnOneFrameGrabbed(this, m_nImageCnt, NULL, NULL);
	}
	

	m_nImageCnt++;
	return;
}

void GrabExecutor_Coaxlink::InitSaveNumber()
{
	m_nSave = 0;
}

void GrabExecutor_Coaxlink::StandstillSaveNumber()
{
	m_nSave -= 1;
}

void GrabExecutor_Coaxlink::SetPictureBox(CRect rect)
{
	m_Rect = rect;
}

void GrabExecutor_Coaxlink::SetView(CDC* pDC)
{
	m_pDC = pDC;
}

void GrabExecutor_Coaxlink::SetContext(int64_t nContext)
{
	m_nContext = nContext;
}

// call back
void GrabExecutor_Coaxlink::RegisterCallback_Grab(HWND hwndOwner, void *pUserData, FUNC_GRAB_CALLBACK *pHandler)
{
	m_sCallbackGrab.m_hwndOwner = hwndOwner;
	m_sCallbackGrab.m_pUserData = pUserData;
	m_sCallbackGrab.m_pFuncCallback = pHandler;
}

long GrabExecutor_Coaxlink::GetCamWidth()
{
	return m_Width;
}

long GrabExecutor_Coaxlink::GetCamHeight()
{
	return m_Height;
}

BOOL GrabExecutor_Coaxlink::MakeBuffer(long nToTalCamCount)
{
    return FALSE;
	//1번 카메라 기준 영상 크기를 설정.
    int nMax_Width = m_pCoaxlink[0]->getInteger<RemoteModule>("Width");
    int nMax_Height = m_pCoaxlink[0]->getInteger<RemoteModule>("Height");
    int nPixelCnt = m_pCoaxlink[0]->getInteger<StreamModule>("PixelComponentCount");
    for (long nindex = 0; nindex < nToTalCamCount; nindex++)
    {
        if (nindex == 0)
        {
            m_pCoaxlink[nindex]->reallocBuffers(m_nBuffer_cnt, nMax_Width * nMax_Height * nPixelCnt);
		}
        else
        {

        }
    }
}

//bool GrabExecutor_Coaxlink::OnTrigger()
//{
//	/*
//	if (__super::OnTrigger() == false)
//	{
//		return false;
//	}
//
//	DWORD dwRet = ::Ig_MultiGrabProcess(m_grabberIndex, m_channelIndex, m_circularBuffer, m_circularBufferCount, -1, MTG_END_WAIT, -1, CallbackOnOneFrameGrabbed, this);
//
//	if (dwRet != IGSTATUS_GRAB_SUCCESS)
//	{
//		m_logger.Log(0, _T("iGrab[%d/%d] Ig_MultiGrabProcess returned %s"), m_grabberIndex, m_channelIndex, (LPCTSTR)GetErrorString(dwRet));
//	}
//	*/
//
//	return true;
//}
//
//void GrabExecutor_Coaxlink::OnStart()
//{
//	Grab(1);
//}
//
//void GrabExecutor_Coaxlink::OnRequestStop()
//{
//	//::Ig_GrabStop(m_grabberIndex, m_channelIndex);
//	Stop();
//}
//
//
//void GrabExecutor_Coaxlink::SaveQueueImage(LPCTSTR targetFolder, long saveMaxCount)
//{
//	if (m_queuePusher != nullptr)
//	{
//		CString strPath;
//
//		CFileFind find;
//
//		if (m_cameraIndex == 0)
//		{
//			strPath.Format(_T("%s\\UP"), targetFolder);
//
//			if (!find.FindFile(strPath))
//				CreateDirectory(strPath, NULL);
//		}
//		else
//		{
//			strPath.Format(_T("%s\\DOWN"), targetFolder);
//
//			if (!find.FindFile(strPath))
//				CreateDirectory(strPath, NULL);
//		}
//
//		if (!m_queuePusher->IsPusherBusy())
//		{
//			m_queuePusher->SetPusherBusy();
//			m_queuePusher->Save(strPath, saveMaxCount);
//			m_queuePusher->SetPusherIdle();
//		}
//		else
//			m_queuePusher->Save(strPath, saveMaxCount);
//	}
//}

/********************************************************************************/
/********************************************************************************/
/* CCoaxlinkGrabber class                                                       */
CCoaxlinkGrabber::CCoaxlinkGrabber(IPVM::LoggerInterface& logger, EGenTL &GenTL, int interfaceIndex, int deviceIndex, int dataStreamIndex, void* pParent)
: EGrabber<CallbackMultiThread>(GenTL, interfaceIndex, deviceIndex, dataStreamIndex), GrabExecutor(logger, deviceIndex)
, m_pParent(pParent)
, rgbConverter(GenTL)
, m_grabberIndex(deviceIndex)
, m_nGrabCnt(1)
, m_bGrabDoneSign(FALSE)
{
	m_poImgPtr = NULL;

	//m_poImgPtr = NULL;

	m_iWidth =  getWidth();
	m_iHeight = getHeight();
	m_stPayloadSize = m_iWidth * m_iHeight;

	m_PixelFormat = getPixelFormat();

	m_verticalSize = m_iHeight;
	m_horizontalSize = m_iWidth;

	InitializeCircularBuffer();

	m_nMaxImageCnt = BUFFER_CNT;
    if (GetCircularBufferCount() <= BUFFER_CNT)
    {
        m_nMaxImageCnt = GetCircularBufferCount();
    }

	enableEvent<NewBufferData>();
}


CCoaxlinkGrabber::~CCoaxlinkGrabber(void)
{
}

void CCoaxlinkGrabber::onCicEvent(const CicData& data)
{
	switch (data.numid)
	{
	case ge::EVENT_DATA_NUMID_CIC_CAMERA_TRIGGER_RISING_EDGE:
		TRACE("TRIGGER_RISING_EDGE\n");
		break;
	case ge::EVENT_DATA_NUMID_CIC_CAMERA_TRIGGER_FALLING_EDGE:
		TRACE("TRIGGER_FALLING_EDGE\n");
		break;
	case ge::EVENT_DATA_NUMID_CIC_STROBE_RISING_EDGE:
		TRACE("STROBE_RISING_EDGE\n");
		break;
	case ge::EVENT_DATA_NUMID_CIC_STROBE_FALLING_EDGE:
		TRACE("STROBE_FALLING_EDGE\n");
		break;
	case ge::EVENT_DATA_NUMID_CIC_ALLOW_NEXT_CYCLE:
		TRACE("ALLOW_NEXT_CYCLE\n");
		break;
	case ge::EVENT_DATA_NUMID_CIC_DISCARDED_CIC_TRIGGER:
		TRACE("DISCARDED_CIC_TRIGGER\n");
		break;
	case ge::EVENT_DATA_NUMID_CIC_PENDING_CIC_TRIGGER:
		TRACE("PENDING_CIC_TRIGGER\n");
		break;
	case ge::EVENT_DATA_NUMID_CIC_CXP_TRIGGER_ACK:
		TRACE("CXP_TRIGGER_ACK\n");
		break;
	case ge::EVENT_DATA_NUMID_CIC_CXP_TRIGGER_RESEND:
		TRACE("CXP_TRIGGER_RESEND\n");
		break;
	case ge::EVENT_DATA_NUMID_CIC_TRIGGER:
		TRACE("TRIGGER\n");
		break;
	default:
		break;
	}  
}

void CCoaxlinkGrabber::onIoToolboxEvent(const IoToolboxData &data)
{
	TRACE("onIoToolboxEvent\n");
}

void CCoaxlinkGrabber::onDataStreamEvent(const DataStreamData &data)
{
	switch (data.numid)
	{
	case ge::EVENT_DATA_NUMID_DATASTREAM_START_OF_CAMERA_READOUT:
		TRACE("START_OF_CAMERA_READOUT\n");
		break;
	case ge::EVENT_DATA_NUMID_DATASTREAM_END_OF_CAMERA_READOUT:
		TRACE("END_OF_CAMERA_READOUT\n");
		break;
	case ge::EVENT_DATA_NUMID_DATASTREAM_START_OF_SCAN:
		TRACE("START_OF_SCAN\n");
		break;
	case ge::EVENT_DATA_NUMID_DATASTREAM_END_OF_SCAN:
		TRACE("END_OF_SCAN\n");
		break;
	case ge::EVENT_DATA_NUMID_DATASTREAM_REJECTED_FRAME:
		TRACE("REJECTED_FRAME\n");
		break;
	case ge::EVENT_DATA_NUMID_DATASTREAM_REJECTED_SCAN:
		TRACE("REJECTED_SCAN\n");
		break;
	default:
		break;
	}  
}

void CCoaxlinkGrabber::onCxpInterfaceEvent(const CxpInterfaceData &data)
{
	switch (data.numid)
	{
	case ge::EVENT_DATA_NUMID_IO_TOOLBOX_LIN1:
		TRACE("IO_TOOLBOX_LIN1\n");
		break;
	case ge::EVENT_DATA_NUMID_IO_TOOLBOX_LIN2:
		TRACE("IO_TOOLBOX_LIN2\n");
		break;
	case ge::EVENT_DATA_NUMID_IO_TOOLBOX_LIN3:
		TRACE("IO_TOOLBOX_LIN3\n");
		break;
	case ge::EVENT_DATA_NUMID_IO_TOOLBOX_LIN4:
		TRACE("IO_TOOLBOX_LIN4\n");
		break;
	default:
		break;
	}  
}
unsigned char* CCoaxlinkGrabber::RGBConvert(unsigned char * imageptr, std::string pixelformat, int width, int height)
{
	RGBConverter::Auto bgr(rgbConverter, FormatConverter::OutputFormat("RGB8"), imageptr, pixelformat,width, height);

	return bgr.getBuffer();
}


void CCoaxlinkGrabber::onNewBufferEvent(const NewBufferData& Data)
{
	memento("Entering Callback");

	ScopedBuffer buffer(*this, Data);
	m_poImgPtr = buffer.getInfo<unsigned char *>(gc::BUFFER_INFO_BASE);

	//GrabExecutor_Coaxlink* pCoaxlink = (GrabExecutor_Coaxlink*)m_pParent;
	////pCoaxlink->m_nImageCnt++;
	//pCoaxlink->OnImageEvent(m_poImgPtr);
	////pCoaxlinkLinescan->OnImageEvent(RGBConvert(m_poImgPtr,m_PixelFormat, m_iWidth, m_iHeight));


	OnImageEvent(m_poImgPtr);

}

bool CCoaxlinkGrabber::OnTrigger()
{
	/*
	if (__super::OnTrigger() == false)
	{
		return false;
	}

	DWORD dwRet = ::Ig_MultiGrabProcess(m_grabberIndex, m_channelIndex, m_circularBuffer, m_circularBufferCount, -1, MTG_END_WAIT, -1, CallbackOnOneFrameGrabbed, this);

	if (dwRet != IGSTATUS_GRAB_SUCCESS)
	{
		m_logger.Log(0, _T("iGrab[%d/%d] Ig_MultiGrabProcess returned %s"), m_grabberIndex, m_channelIndex, (LPCTSTR)GetErrorString(dwRet));
	}
	*/

	return true;
}

void CCoaxlinkGrabber::OnStart()
{
	GrabExecutor_Coaxlink* pCoaxlink = (GrabExecutor_Coaxlink*)m_pParent;
	pCoaxlink->m_nCamNum = m_grabberIndex;
    pCoaxlink->m_bReleaseGrabDoneSign = FALSE;
	pCoaxlink->Grab(m_nGrabCnt);
}

void CCoaxlinkGrabber::OnRequestStop()
{
	//::Ig_GrabStop(m_grabberIndex, m_channelIndex);
	GrabExecutor_Coaxlink* pCoaxlink = (GrabExecutor_Coaxlink*)m_pParent;
	pCoaxlink->Stop();
	
}


void CCoaxlinkGrabber::SaveQueueImage(LPCTSTR targetFolder, long saveMaxCount)
{
	if (m_queuePusher != nullptr)
	{
		CString strPath;

		CFileFind find;

		if (m_cameraIndex == 0)
		{
			strPath.Format(_T("%s\\UP"), targetFolder);

			if (!find.FindFile(strPath))
				CreateDirectory(strPath, NULL);
		}
		else
		{
			strPath.Format(_T("%s\\DOWN"), targetFolder);

			if (!find.FindFile(strPath))
				CreateDirectory(strPath, NULL);
		}

		if (!m_queuePusher->IsPusherBusy())
		{
			m_queuePusher->SetPusherBusy();
			m_queuePusher->Save(strPath, saveMaxCount);
			m_queuePusher->SetPusherIdle();
		}
		else
			m_queuePusher->Save(strPath, saveMaxCount);
	}
}

void CCoaxlinkGrabber::OnImageEvent(unsigned char* pBuffer)
{

	/*
	Image_t img;
	img.Index = m_nImageCnt;
	img.Width = m_Width;
	img.Height = m_Height;
	img.Buffer = (void*)pBuffer;

	void* m_pBuffer = (void*)pBuffer;
	int nWidth = m_Width;
	int nHeight = m_Height;

	m_pBitmapInfo->bmiHeader.biWidth = nWidth;
	m_pBitmapInfo->bmiHeader.biHeight = -nHeight;

	if(m_sCallbackGrab.m_pFuncCallback)
	{
		if(::IsWindow(m_sCallbackGrab.m_hwndOwner))
		{
			m_sCallbackGrab.m_pFuncCallback(m_sCallbackGrab.m_pUserData, *m_pBitmapInfo, pBuffer);
		}
		else
		{
			memset(&m_sCallbackGrab, 0, sizeof(m_sCallbackGrab));
		}
	}
	m_nSave++;

	return;*/
	//ScopedBuffer buf(*this, Data);
	
	GrabExecutor_Coaxlink* pCoaxlink = (GrabExecutor_Coaxlink*)m_pParent;

	Image_t img;
	img.Index = m_nImageCnt;
	img.Width = pCoaxlink->GetCamWidth();
	img.Height = pCoaxlink->GetCamHeight();
	img.Buffer = pBuffer;

	BYTE* m_pBuff_Insp = this->m_circularBuffer[img.Index];
	int nWidth = pCoaxlink->GetCamWidth();
	int nHeight = pCoaxlink->GetCamHeight();

	//IPVM::Image_8u_C1 imgTest(nWidth, nHeight);
	//memcpy(imgTest.GetMem(), img.Buffer, nWidth * nHeight);
	//CString strName = _T("");
	//strName.Format(_T("D:\\idiotHun%d.bmp"), m_cameraIndex);
	//imgTest.SaveBmp(strName);

	memcpy(m_pBuff_Insp, img.Buffer, nWidth * nHeight);

	m_bGrabDoneSign = TRUE;
	
	if (m_grabberIndex == 0)
	{
		//임시로, 하드코딩 동기화
        long nCnt = 0;
        std::vector<BOOL> bCheckSign;
        bCheckSign.resize(pCoaxlink->m_nToTalCameraCnt);
        while (TRUE)
        {
            nCnt = 0;
            for (long nindex = 0; nindex < bCheckSign.size(); nindex++)
            {
                bCheckSign[nindex] = pCoaxlink->m_pCoaxlink[m_grabberIndex]->m_bGrabDoneSign;
                if (bCheckSign[nindex] == TRUE)
                {
                    nCnt++;
				}
			}
            if (nCnt == bCheckSign.size())
            {
                ((GrabExecutor*)(pCoaxlink->m_pCoaxlink[m_grabberIndex]))->SetFrameIndex_Area(m_nImageCnt, m_nImageCnt + 1);
				((GrabExecutor*)(pCoaxlink->m_pCoaxlink[m_grabberIndex]))->CallbackOnOneFrameGrabbed(((GrabExecutor*)pCoaxlink->m_pCoaxlink[m_grabberIndex]), m_nImageCnt, NULL, NULL);
                pCoaxlink->m_bReleaseGrabDoneSign = TRUE;
				break;

			}
		}
		
	}
    else
    {
        while (TRUE)
        {
            if (pCoaxlink->m_bReleaseGrabDoneSign == TRUE)
            {
                ((GrabExecutor*)(pCoaxlink->m_pCoaxlink[m_grabberIndex]))->SetFrameIndex_Area(m_nImageCnt, m_nImageCnt + 1);
                break;
			}
        }
    }
	
	m_nImageCnt++;
    if (m_nImageCnt >= m_nMaxImageCnt)
    {
        m_nImageCnt = 0;
    }
    m_bGrabDoneSign = FALSE;
	return;
}

void CCoaxlinkGrabber::SetGrabCount(long nCnt)
{
    m_nGrabCnt = nCnt;
}

long CCoaxlinkGrabber::GetGrabCount()
{
    return m_nGrabCnt;
}

void CCoaxlinkGrabber::SetVerticalLength(long nPageLength)
{
    return;
    
}

void CCoaxlinkGrabber::SetSystemGain(CString strGain)
{
    return;
}

void CCoaxlinkGrabber::SetExposureTime(CString strExposure, int nRow)
{
    return;
}
double CCoaxlinkGrabber::GetSystemGain()
{
    return 0;
}

void CCoaxlinkGrabber::GetExposureTime(double& dBright, double& dDark)
{
    return;
}

void CCoaxlinkGrabber::SetSensorSelector(CString strSensorSel)
{
    return;
}

void CCoaxlinkGrabber::SetSensorGain(CString strSensorGain)
{
    return;
}

void CCoaxlinkGrabber::GetSensorInfo(int& nSensorSelector, double& dSensorGain)
{
    return;
}

void CCoaxlinkGrabber::GetRowGain(double& dRowGainBright, double& dRowGainDark)
{
    return;
}

void CCoaxlinkGrabber::SetRowGain(CString strRowGain, int nRow)
{
    return;
}
void CCoaxlinkGrabber::ResponseLeveling()
{
    return;
}
void CCoaxlinkGrabber::Reset()
{
    return;
}

double CCoaxlinkGrabber::GetFPGATemperature()
{
    return 0;
}
BOOL CCoaxlinkGrabber::GetCameraConnectCheck()
{
    return TRUE;
}

void CCoaxlinkGrabber::SetExposureTime_Multi(std::vector<CString> vecstrExposureValue, int nCntCh)
{
    return;
}
    /* CCoaxlinkGrabber class                                                       */
/********************************************************************************/
/********************************************************************************/