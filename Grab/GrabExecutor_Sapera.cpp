#include "stdafx.h"
#include "GrabExecutor_Sapera.h"
#include "../Module/SharedImageQueue/ImageQueuePusher.h"
#include "Base/LoggerInterface.h"
#include <ppl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define STRING_LENGTH		256 

void GrabExecutor_Sapera::XferCallback(SapXferCallbackInfo* pInfo)		//사페라 콜백
{
	GrabExecutor_Sapera* pParent = (GrabExecutor_Sapera*)pInfo->GetContext();


	void* pData_buf;// = pbyImageBuff;

	pParent->m_Buffer->GetAddress(&pData_buf);
	//int circleBufIndex = (pParent->m_nCallbackCnt - 1) % m_circularBufferCount; // 그랩 개수라 1부터 시작하는 숫자라서....-1한다..
	long nSizeX = pParent->m_horizontalSize;
	long nSizeY = pParent->m_verticalSize;
	const int64_t frameIndex = pParent->m_nCallbackCnt - 1;
	BYTE* pDst = (BYTE*)pParent->m_circularBuffer[frameIndex % m_circularBufferCount];

	long size = nSizeX * nSizeY;
	memcpy(pDst, pData_buf, size);
    memcpy(pDst + size, ((BYTE*)pData_buf + size), size);


	pParent->CallbackOnOneFrameGrabbed(pParent, pParent->m_nCallbackCnt, NULL, NULL);
	pParent->m_nCallbackCnt++;
}


GrabExecutor_Sapera::GrabExecutor_Sapera(IPVM::LoggerInterface& logger, const int cameraIndex, const int grabberIndex, const int channelIndex, LPCTSTR ccfPathName, MachineType machineType)
	: GrabExecutor(logger, cameraIndex)
	, m_grabberIndex(grabberIndex)
	, m_channelIndex(channelIndex)
	, m_cfIndex(-1)
	, m_bgrabon(false)
	, m_logger(logger)
{
	m_machineType = machineType;

	m_Xfer = NULL;
	m_Buffer = NULL;
	m_Acq = NULL;

	m_pDevice = NULL;
    m_pFeature = NULL;

	m_horizontalSize = 0;
	m_verticalSize = 0;

	m_cellTirggerPosInPixel = 0;

	m_iSizeX = 0;
	m_iSizeY = 0;

	//카메라 연결
	//Grabber 정보가져오기
	//Server : Grabber - Port 개수??
	char serverName[CORSERVER_MAX_STRLEN];
	char userDefinedName[STRING_LENGTH];
	int featureCount = 20;

	m_fPrevGrabTime = -1.0f;

	SapManager::GetServerName(grabberIndex, serverName, sizeof(serverName));
    
	CStringA sB(ccfPathName);
	const char* pszB = sB;
	char* pszBo = const_cast<char*>(pszB);
	m_Acq = new SapAcquisition(SapLocation(serverName, channelIndex), pszBo);//, SapAcquisition::EventEndOfFrame, NULL, NULL);

	/*"C:\\Program Files\\Teledyne DALSA\\Sapera\\CamFiles\\User\\N_No_Name_Default_Default.ccf"*//*, SapAcquisition::EventEndOfFrame, NULL, NULL*/
	if (grabberIndex != m_grabberIndex)
	{
		m_logger.Log(0, _T("grabberIndex != m_grabberIndex [%d]"), m_grabberIndex);
		AfxMessageBox(_T("Check cam calble maybe it should be wrong"));
	}

	NumberOfFrames = featureCount;
	m_Buffer = new SapBuffer(NumberOfFrames, m_Acq);
	m_Xfer = new SapAcqToBuf(m_Acq, m_Buffer, XferCallback, this);

	m_pDevice = new SapAcqDevice(SapLocation(serverName, channelIndex));
    m_pFeature = new SapFeature(SapLocation(serverName, channelIndex));
    strcpy_s(m_chServerName, serverName);
	
	OpenPort();
	
	m_queuePusher = new SharedMemory::ImageQueuePusher(cameraIndex);
}

GrabExecutor_Sapera::~GrabExecutor_Sapera()
{
	if (m_Xfer && *m_Xfer)
	{
		m_Xfer->Freeze();
	}

	// Destroy view object
	//if (m_View && *m_View) m_View->Destroy();

	// Destroy transfer object
	if (m_Xfer && *m_Xfer) m_Xfer->Destroy();

	// Destroy buffer object
	if (m_Buffer && *m_Buffer) m_Buffer->Destroy();

	// Destroy buffer object
	//if (m_MultiBuffer && *m_Buffer) m_MultiBuffer->Destroy();

	// Destroy acquisition object
	if (m_Acq && *m_Acq) m_Acq->Destroy();

	// Delete all objects
	//if (m_View)			delete m_View; 
	if (m_Xfer)			delete m_Xfer;
	if (m_Buffer)		delete m_Buffer;
	//if (m_MultiBuffer)	delete m_MultiBuffer;
	if (m_Acq)			delete m_Acq;
    if (m_pDevice)      delete m_pDevice;
    if (m_pFeature)     delete m_pFeature;

	//delete[] m_queuePusher;
}

BOOL GrabExecutor_Sapera::IsAvailable()
{
	return FALSE;
}

BOOL GrabExecutor_Sapera::OpenPort()
{
	// Create acquisition object
	if (m_Acq && !*m_Acq && !m_Acq->Create())
		return FALSE;

	// Create buffer object
	if (m_Buffer && !*m_Buffer && !m_Buffer->Create())
		return FALSE;

	// Create transfer object
	if (m_Xfer && !*m_Xfer && !m_Xfer->Create())
		return FALSE;


	// Create Device object
    if (m_pDevice && !*m_pDevice && !m_pDevice->Create())
        return FALSE;

	// Create Feature object
    if (m_pFeature && !*m_pFeature && !m_pFeature->Create())
        return FALSE;


	m_Acq->GetParameter(CORACQ_PRM_CROP_WIDTH, &m_horizontalSize);
	//m_verticalSize = 2000;
	m_Acq->GetParameter(CORACQ_PRM_CROP_HEIGHT, &m_verticalSize);
	//m_MultiBuffer = new SapBuffer(1, m_horizontalSize , m_verticalSize * NumberOfFrames, SapFormatMono8, SapBuffer::TypeScatterGather, SapLocation::ServerSystem);
	//m_verticalSize = 600;
	InitializeCircularBuffer();
	// Create buffer object
	//if (m_MultiBuffer && !*m_MultiBuffer && !m_MultiBuffer->Create())
	//	return FALSE;


	return TRUE;
}

void GrabExecutor_Sapera::ResetShaftEncoder()
{
	m_Acq->SetParameter(CORACQ_PRM_SHAFT_ENCODER_ENABLE, 0);
	m_Acq->SetParameter(CORACQ_PRM_INT_LINE_TRIGGER_ENABLE, 1);  // internal 1

	m_Acq->SetParameter(CORACQ_PRM_INT_LINE_TRIGGER_ENABLE, 0);
	m_Acq->SetParameter(CORACQ_PRM_SHAFT_ENCODER_ENABLE, 1);     // shaft 1
}

BOOL GrabExecutor_Sapera::SingleGrab_Begin(BYTE* pbyImageBuff, long nTimeout)
{
	m_nTimeOut = nTimeout;
	m_nCallbackCnt = 1; //-> 초기화 하면 Circle Buffer 순서가 초기화 됨으로....주석처리...
	m_Buffer->ResetIndex();
	m_bgrabon = true;
	m_Xfer->Init();
	m_Xfer->Grab();
	m_Xfer->Wait(500);

	return TRUE;
}

BOOL GrabExecutor_Sapera::SingleGrab_End(/*BYTE* pbyImageBuff*/)
{
	m_bgrabon = false;

	if (m_Xfer->Freeze())
	{
		//Sleep 추가 안 하면, 다음 Grab부터 영상 윗부분이 아래에 들어옴
		m_Xfer->Wait(1000);
		//Sleep(500);
		m_Xfer->Abort();
	}

	return TRUE;
}

BOOL GrabExecutor_Sapera::SetDirection(long /*nDirection*/, BOOL /*bUseGrabber*/)
{
	return TRUE;
}

BOOL GrabExecutor_Sapera::SetColorGap(long /*nValue*/, BOOL /*bUseGrabber*/)
{
	return TRUE;
}
BOOL GrabExecutor_Sapera::SetTriggerType(BOOL /*flg*/)
{
	return TRUE;
}
BOOL GrabExecutor_Sapera::SetExposureTime(float /*fHigh */, float /*fLow*/)
{
	return TRUE;
}
BOOL GrabExecutor_Sapera::SetPageLength(long nPageLength)
{
	return true;

	long nGetImageNum = nPageLength / 1000;

	if (m_Xfer && *m_Xfer)
	{
		m_Xfer->Freeze();
	}

	// Destroy transfer object
	if (m_Xfer && *m_Xfer) m_Xfer->Destroy();

	// Destroy buffer object
	if (m_Buffer && *m_Buffer) m_Buffer->Destroy();

	// Destroy buffer object
	//if (m_MultiBuffer && *m_Buffer) m_MultiBuffer->Destroy();

	// Destroy acquisition object
	if (m_Acq && *m_Acq) m_Acq->Destroy();

	// Delete all objects
	//if (m_View)			delete m_View; 
	if (m_Xfer)			delete m_Xfer;
	if (m_Buffer)		delete m_Buffer;
	//if (m_MultiBuffer)	delete m_MultiBuffer;
	if (m_Acq)			delete m_Acq;

	NumberOfFrames = nGetImageNum;

	m_Acq = NULL;
	m_Buffer = NULL;
	m_Xfer = NULL;

	//m_Acq = new SapAcquisition(SapLocation("Xtium2-CLHS_PX8_1", 0), "C:\\LinearML_GRABTEST\\N_No_Name_Default_Default.ccf", SapAcquisition::EventEndOfFrame, NULL, NULL);
	//m_Acq = new SapAcquisition(SapLocation("Xtium2-CLHS_PX8_1", 0), "C:\\LinearML_GRABTEST\\FreeRun.ccf", SapAcquisition::EventEndOfFrame, NULL, NULL);

	if (m_grabberIndex == 0)
		m_Acq = new SapAcquisition(SapLocation("Xtium2-CLHS_PX8_1", 0), "C:\\LinearML_GRABTEST\\Master_2000.ccf", SapAcquisition::EventEndOfFrame, NULL, NULL);
	else
		m_Acq = new SapAcquisition(SapLocation("Xtium2-CLHS_PX8_2", 1), "C:\\LinearML_GRABTEST\\Slave_2000.ccf", SapAcquisition::EventEndOfFrame, NULL, NULL);


	m_Buffer = new SapBuffer(NumberOfFrames, m_Acq);
	m_Xfer = new SapAcqToBuf(m_Acq, m_Buffer, XferCallback, this);
	//m_View		= new SapView(m_Buffer,m_ctrlDisplay.GetSafeHwnd());	

	// Create acquisition object
	if (m_Acq && !*m_Acq && !m_Acq->Create())
		return FALSE;

	// Create buffer object
	if (m_Buffer && !*m_Buffer && !m_Buffer->Create())
		return FALSE;

	// Create transfer object
	if (m_Xfer && !*m_Xfer && !m_Xfer->Create())
		return FALSE;

	//// Create view object
	//if (m_View && !*m_View && !m_View->Create())
	//	OnCancel();
    //m_Acq->SetParameter(CORACQ_PRM_CROP_HEIGHT, &m_verticalSize);
	m_Acq->GetParameter(CORACQ_PRM_CROP_WIDTH, &m_horizontalSize);
	m_Acq->GetParameter(CORACQ_PRM_CROP_HEIGHT, &m_verticalSize);
	//m_MultiBuffer = new SapBuffer(1, m_horizontalSize, m_iSizeY * NumberOfFrames, SapFormatMono8, SapBuffer::TypeScatterGather, SapLocation::ServerSystem);

	// Create buffer object
	//if (m_MultiBuffer && !*m_MultiBuffer && !m_MultiBuffer->Create())
	//	return FALSE;
	return TRUE;
}

BOOL GrabExecutor_Sapera::GetImageInfo(long& /*nwidth*/, long& /*nheight*/, long& /*nbpp*/)
{
	return TRUE;
}

void GrabExecutor_Sapera::GetLastError(CString& strErr)
{
}


bool GrabExecutor_Sapera::OnTrigger()
{
	if (__super::OnTrigger() == false)
	{
		return false;
	}

	//PutGrabbuffer();
	long frameIndex = 1;
	return true;
}

void GrabExecutor_Sapera::OnStart()
{
	//Grabbuffer에 넣어주기 위해 추가함(이미지커팅테스트용)///
	if (!m_Acq->Create())
	{
		m_bgrabon = true;
	}
	else SingleGrab_Begin(*m_circularBuffer, 500);
}


UINT GrabExecutor_Sapera::threadPushBuffer(LPVOID pParam)
{
	GrabExecutor_Sapera* pwnd = (GrabExecutor_Sapera*)pParam;
	//pwnd->PutGrabbuffer();

	return true;
}
void GrabExecutor_Sapera::OnRequestStop()
{
	bool bsuccess = SingleGrab_End(/**m_circularBuffer*/);
	bsuccess = true;
}

void GrabExecutor_Sapera::SaveQueueImage(LPCTSTR targetFolder, long saveMaxCount)
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

void GrabExecutor_Sapera::SetVerticalLength(long nPageLength)
{
    //BOOL bCheck = SetPageLength(nPageLength);
    //if (bCheck==TRUE)
    {
		// nPageLength와 n값이 같은지 확인
        long nTotalLine = nPageLength * 28.5;
        m_Xfer->Disconnect();
        m_Acq->SetParameter(CORACQ_PRM_CROP_HEIGHT, nTotalLine);
        m_Xfer->Connect();
        long n;
        m_Acq->GetParameter(CORACQ_PRM_CROP_HEIGHT, &n);
    }

}

//void GrabExecutor_Sapera::SetExposureTime(CString strExposure, int nRow) // Row 값 추가
//{
//    BOOL status;
//    //SapAcqDevice device(*m_pDevice);
//    //SapFeature feature(*m_pFeature);
//	
//	BOOL isAvailable = FALSE;
//    int numberOfFeatures = 0;
//    m_pDevice->GetFeatureCount(&numberOfFeatures);
//
//    SapFeature::IncrementType myIncrement;
//    SapFeature::Type myType;
//
//    for (long featureIndex = 0; featureIndex < numberOfFeatures; featureIndex++)
//    {
//        char featureName[108];
//        status = m_pDevice->GetFeatureNameByIndex(featureIndex, featureName, sizeof(featureName));
//
//        CString strTemp;
//        strTemp = (CStringA)featureName;
//
//        double dSetValue = _ttof(strExposure);
//        double dGetValue = 0.f;
//        int nGetValue;
//        int nSetValue = nRow;
//        if (strTemp.Compare(_T("exposureTimeSelector")) == 0)
//        {
//            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
//            {
//                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);
//                char name[11] = "systemGain";
//
//                SapFeature::AccessMode myAccess;
//                status = m_pFeature->GetAccessMode(&myAccess);
//                status = m_pFeature->GetType(&myType);
//                status = m_pFeature->GetIncrementType(&myIncrement);
//                if (status)
//                {
//                    // systemGrain = 35
//                    status = m_pDevice->GetFeatureValue(featureName, &nGetValue);
//                    status = m_pDevice->SetFeatureValue(featureName, nSetValue);
//                }
//            }
//        }
//
//        if (strTemp.Compare(_T("ExposureTime")) == 0)
//        {
//            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
//            {
//                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);
//                char name[11] = "systemGain";
//
//                SapFeature::AccessMode myAccess;
//                status = m_pFeature->GetAccessMode(&myAccess);
//                status = m_pFeature->GetType(&myType);
//                status = m_pFeature->GetIncrementType(&myIncrement);
//                if (status)
//                {
//                    // systemGrain = 35
//                    status = m_pDevice->GetFeatureValue(featureName, &dGetValue);
//                    status = m_pDevice->SetFeatureValue(featureName, dSetValue);
//                }
//            }
//        }
//    }
//}

void GrabExecutor_Sapera::SetExposureTime(CString strExposure, int nRow) // Row 값 추가
{
    BOOL status; //  m_pDevice    m_pFeature
    //SapAcqDevice device(m_chServerName);
    //device.Create();
    //SapFeature feature(m_chServerName);
    //feature.Create();
    BOOL isAvailable = FALSE;
    int numberOfFeatures = 0;
    m_pDevice->GetFeatureCount(&numberOfFeatures);

    SapFeature::IncrementType myIncrement;
    SapFeature::Type myType;

    for (long featureIndex = 0; featureIndex < numberOfFeatures; featureIndex++)
    {
        char featureName[108];
        status = m_pDevice->GetFeatureNameByIndex(featureIndex, featureName, sizeof(featureName));

        CString strTemp;
        strTemp = (CStringA)featureName;

        double dSetValue = _ttof(strExposure);
        double dGetValue = 0.f;
        int nGetValue;
        int nSetValue = nRow;
        if (strTemp.Compare(_T("exposureTimeSelector")) == 0)
        {
            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
            {
                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);
                char name[11] = "systemGain";

                SapFeature::AccessMode myAccess;
                status = m_pFeature->GetAccessMode(&myAccess);
                status = m_pFeature->GetType(&myType);
                status = m_pFeature->GetIncrementType(&myIncrement);
                if (status)
                {
                    // systemGrain = 35
                    status = m_pDevice->GetFeatureValue(featureName, &nGetValue);
                    status = m_pDevice->SetFeatureValue(featureName, nSetValue);

                }
            }
        }

        if (strTemp.Compare(_T("ExposureTime")) == 0)
        {
            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
            {
                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);
                //char name[11] = "systemGain";

                SapFeature::AccessMode myAccess;
                status = m_pFeature->GetAccessMode(&myAccess);
                status = m_pFeature->GetType(&myType);
                status = m_pFeature->GetIncrementType(&myIncrement);
                if (status)
                {
                    // systemGrain = 35
                    status = m_pDevice->GetFeatureValue(featureName, &dGetValue);
                    status = m_pDevice->SetFeatureValue(featureName, dSetValue);
                }
            }
        }

    }

}

void GrabExecutor_Sapera::SetSystemGain(CString strGain)
{
    BOOL status; //  m_pDevice    m_pFeature
    //SapAcqDevice device(m_chServerName);
    //device.Create();
    //SapFeature feature(m_chServerName);
    //feature.Create();
    BOOL isAvailable = FALSE;
    int numberOfFeatures = 0;
    m_pDevice->GetFeatureCount(&numberOfFeatures);

    SapFeature::IncrementType myIncrement;
    SapFeature::Type myType;

	double dSetValue = _ttof(strGain);
    double dGetValue = 0.f;
    int nGetValue;

    for (long featureIndex = 0; featureIndex < numberOfFeatures; featureIndex++)
    {
        char featureName[108];
        status = m_pDevice->GetFeatureNameByIndex(featureIndex, featureName, sizeof(featureName));

        CString strTemp;
        strTemp = (CStringA)featureName;
     
		if (strTemp.Compare(_T("exposureTimeSelector")) == 0)
        {
            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
            {
                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);

                SapFeature::AccessMode myAccess;
                status = m_pFeature->GetAccessMode(&myAccess);
                status = m_pFeature->GetType(&myType);
                status = m_pFeature->GetIncrementType(&myIncrement);
                if (status)
                {
                    // systemGrain = 35
                    status = m_pDevice->GetFeatureValue(featureName, &nGetValue);
                    //status = device.SetFeatureValue(featureName, dSetValue);
                }
            }
		}

        if (strTemp.Compare(_T("systemGain")) == 0)
        {
            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
            {
                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);

                SapFeature::AccessMode myAccess;
                status = m_pFeature->GetAccessMode(&myAccess);
                status = m_pFeature->GetType(&myType);
                status = m_pFeature->GetIncrementType(&myIncrement);
                if (status)
                {
                    // systemGrain = 35
                    status = m_pDevice->GetFeatureValue(featureName, &dGetValue);
                    status = m_pDevice->SetFeatureValue(featureName, dSetValue);
                }
            }
        }
    }
}

double GrabExecutor_Sapera::GetSystemGain()
{
    BOOL status; //  m_pDevice    m_pFeature
    //SapAcqDevice device(m_chServerName);
    //device.Create();
    //SapFeature feature(m_chServerName);
    //feature.Create();
    BOOL isAvailable = FALSE;
    int numberOfFeatures = 0;
    m_pDevice->GetFeatureCount(&numberOfFeatures);

    SapFeature::IncrementType myIncrement;
    SapFeature::Type myType;
    double dGain;
    for (long featureIndex = 0; featureIndex < numberOfFeatures; featureIndex++)
    {
        char featureName[108];
        status = m_pDevice->GetFeatureNameByIndex(featureIndex, featureName, sizeof(featureName));

        CString strTemp;
        strTemp = (CStringA)featureName;

        double dGetValue = 0.f;

        if (strTemp.Compare(_T("systemGain")) == 0)
        {
            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
            {
                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);

                SapFeature::AccessMode myAccess;
                status = m_pFeature->GetAccessMode(&myAccess);
                status = m_pFeature->GetType(&myType);
                status = m_pFeature->GetIncrementType(&myIncrement);
                if (status)
                {
                    // systemGrain = 35
                    status = m_pDevice->GetFeatureValue(featureName, &dGetValue);
                    dGain = dGetValue;
                    //status = device.SetFeatureValue(featureName, dSetValue);
                }
            }
        }
    }
    return dGain;
}

void GrabExecutor_Sapera::GetExposureTime(double& dBright, double& dDark)
{
       BOOL status; //  m_pDevice    m_pFeature
       BOOL isAvailable = FALSE;
       int numberOfFeatures = 0;
       m_pDevice->GetFeatureCount(&numberOfFeatures);

       double dExpoTime = 0.f;
       SapFeature::IncrementType myIncrement;
       SapFeature::Type myType;
       double dGetValue = 0.f;
       char featureName[21] = "exposureTimeSelector";
       char expoName[14] = "ExposureTime";

       CString strTemp;
       strTemp = (CStringA)featureName;

       if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
       {
           status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);

           SapFeature::AccessMode myAccess;
           status = m_pFeature->GetAccessMode(&myAccess);
           status = m_pFeature->GetType(&myType);
           status = m_pFeature->GetIncrementType(&myIncrement);
           if (status)
           {
               status = m_pDevice->SetFeatureValue(featureName, 1);
               status = m_pDevice->GetFeatureValue(expoName, &dGetValue);
               dBright = dGetValue;

               status = m_pDevice->SetFeatureValue(featureName, 2);
               status = m_pDevice->GetFeatureValue(expoName, &dGetValue);
               dDark = dGetValue;
           }
       }
    return;






 //   BOOL status; //  m_pDevice    m_pFeature
 //   //SapAcqDevice device(m_chServerName);
 //   //device.Create();
 //   //SapFeature feature(m_chServerName);
 //   //feature.Create();
 //   BOOL isAvailable = FALSE;
 //   int numberOfFeatures = 0;
 //   m_pDevice->GetFeatureCount(&numberOfFeatures);


 //   double dExpoTime = 0.f;
 //   SapFeature::IncrementType myIncrement;
 //   SapFeature::Type myType;
 //   double dGetValue = 0.f;
 //   char featureName[21] = "exposureTimeSelector";
 //   char expoName[14] = "ExposureTime";

 //   CString strTemp;
 //   strTemp = (CStringA)featureName;

 //   if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
 //   {
 //       status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);

 //       SapFeature::AccessMode myAccess;
 //       status = m_pFeature->GetAccessMode(&myAccess);
 //       status = m_pFeature->GetType(&myType);
 //       status = m_pFeature->GetIncrementType(&myIncrement);
 //       if (status)
 //       {
 //           status = m_pDevice->SetFeatureValue(featureName, 1);
 //           status = m_pDevice->GetFeatureValue(expoName, &dGetValue);
 //           dBright = dGetValue;

 //           status = m_pDevice->SetFeatureValue(featureName, 2);
 //           status = m_pDevice->GetFeatureValue(expoName, &dGetValue);
 //           dDark = dGetValue;
 //       }
 //   }
	//return;
}

void GrabExecutor_Sapera::SetSensorSelector(CString strSensorSel)
{
    BOOL status; //  m_pDevice    m_pFeature
    BOOL isAvailable = FALSE;
    int numberOfFeatures = 0;
    m_pDevice->GetFeatureCount(&numberOfFeatures);

    SapFeature::IncrementType myIncrement;
    SapFeature::Type myType;

    int nSetValue = _ttof(strSensorSel);
    int nGetValue = 0.f;

    for (long featureIndex = 0; featureIndex < numberOfFeatures; featureIndex++)
    {
        char featureName[108];
        status = m_pDevice->GetFeatureNameByIndex(featureIndex, featureName, sizeof(featureName));

        CString strTemp;
        strTemp = (CStringA)featureName;

        if (strTemp.Compare(_T("gainSensorSelect")) == 0)
        {
            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
            {
                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);

                SapFeature::AccessMode myAccess;
                status = m_pFeature->GetAccessMode(&myAccess);
                status = m_pFeature->GetType(&myType);
                status = m_pFeature->GetIncrementType(&myIncrement);
                if (status)
                {
                    // systemGrain = 35
                    status = m_pDevice->GetFeatureValue(featureName, &nGetValue);

                    status = m_pDevice->SetFeatureValue(featureName, nSetValue);
                    status = m_pDevice->GetFeatureValue(featureName, &nGetValue);

                }
            }
        }
    }
}

void GrabExecutor_Sapera::SetSensorGain(CString strSensorGain)
{
    BOOL status; //  m_pDevice    m_pFeature
    BOOL isAvailable = FALSE;
    int numberOfFeatures = 0;
    m_pDevice->GetFeatureCount(&numberOfFeatures);

    SapFeature::IncrementType myIncrement;
    SapFeature::Type myType;

    double dSetValue = _ttof(strSensorGain);
    double dGetValue = 0.f;
    for (long featureIndex = 0; featureIndex < numberOfFeatures; featureIndex++)
    {
        char featureName[108];
        status = m_pDevice->GetFeatureNameByIndex(featureIndex, featureName, sizeof(featureName));

        CString strTemp;
        strTemp = (CStringA)featureName;

        if (strTemp.Compare(_T("sensorGain")) == 0)
        {
            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
            {
                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);

                SapFeature::AccessMode myAccess;
                status = m_pFeature->GetAccessMode(&myAccess);
                status = m_pFeature->GetType(&myType);
                status = m_pFeature->GetIncrementType(&myIncrement);
                if (status)
                {
                    status = m_pDevice->GetFeatureValue(featureName, &dGetValue);

                    status = m_pDevice->SetFeatureValue(featureName, dSetValue);
                    status = m_pDevice->GetFeatureValue(featureName, &dGetValue);


                }
            }
        }
    }
}

void GrabExecutor_Sapera::GetSensorInfo(int& nSensorSelector, double& dSensorGain)
{
    BOOL status; //  m_pDevice    m_pFeature
    BOOL isAvailable = FALSE;
    int numberOfFeatures = 0;
    m_pDevice->GetFeatureCount(&numberOfFeatures);

    SapFeature::IncrementType myIncrement;
    SapFeature::Type myType;

    int nGetValue = 0;
    double dGetValue = 0.f;

    if (m_pDevice!=nullptr)
    {
        char featureNameSensorSelect[19] = "gainSensorSelect";
        char featureNamesensorGain[11] = "sensorGain";

        status = m_pDevice->SetFeatureValue(featureNameSensorSelect, nSensorSelector);
        
        status = m_pDevice->GetFeatureValue(featureNamesensorGain, &dGetValue);
        dSensorGain = dGetValue;
    }

    return;
}

//void GrabExecutor_Sapera::GetSensorGain(int& nSensorSelector, double& dSensorGain)
//{
//    BOOL status; //  m_pDevice    m_pFeature
//    BOOL isAvailable = FALSE;
//    int numberOfFeatures = 0;
//    m_pDevice->GetFeatureCount(&numberOfFeatures);
//
//    SapFeature::IncrementType myIncrement;
//    SapFeature::Type myType;
//
//    int nGetValue = 0;
//    double dGetValue = 0.f;
//
//    if (m_pDevice != nullptr)
//    {
//        char featureNameSensorSelect[19] = "gainSensorSelect";
//        char featureNamesensorGain[11] = "sensorGain";
//
//        status = m_pDevice->SetFeatureValue(featureNameSensorSelect, &nGetValue);
//        nSensorSelector = nGetValue;
//
//        status = m_pDevice->GetFeatureValue(featureNamesensorGain, &dGetValue);
//        dSensorGain = dGetValue;
//    }
//
//    return;
//}
void GrabExecutor_Sapera::GetRowGain(double& dRowGainBright, double& dRowGainDark)
{
    BOOL status; //  m_pDevice    m_pFeature
    BOOL isAvailable = FALSE;
    int numberOfFeatures = 0;
    m_pDevice->GetFeatureCount(&numberOfFeatures);

    SapFeature::IncrementType myIncrement;
    SapFeature::Type myType;

    double dGetBright = 0.f;
    double dGetDark = 0.f;

    if (m_pDevice != nullptr)
    {
        char featureNameGainSel[13] = "GainSelector";
        char featureNameRowGain[5] = "Gain";

        status = m_pDevice->SetFeatureValue(featureNameGainSel, 1);
        status = m_pDevice->GetFeatureValue(featureNameRowGain, &dGetBright);
        dRowGainBright = dGetBright;

        status = m_pDevice->SetFeatureValue(featureNameGainSel, 2);
        status = m_pDevice->GetFeatureValue(featureNameRowGain, &dGetDark);
        dRowGainDark = dGetDark;

    }

    return;
}

void GrabExecutor_Sapera::SetRowGain(CString strRowGain, int nRow)
{
    BOOL status; //  m_pDevice    m_pFeature
    BOOL isAvailable = FALSE;
    int numberOfFeatures = 0;
    m_pDevice->GetFeatureCount(&numberOfFeatures);

    SapFeature::IncrementType myIncrement;
    SapFeature::Type myType;

    for (long featureIndex = 0; featureIndex < numberOfFeatures; featureIndex++)
    {
        char featureName[108];
        status = m_pDevice->GetFeatureNameByIndex(featureIndex, featureName, sizeof(featureName));

        CString strTemp;
        strTemp = (CStringA)featureName;

        double dSetValue = _ttof(strRowGain);
        double dGetValue = 0.f;
        int nGetValue;
        int nSetValue = nRow;
        if (strTemp.Compare(_T("GainSelector")) == 0)
        {
            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
            {
                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);

                SapFeature::AccessMode myAccess;
                status = m_pFeature->GetAccessMode(&myAccess);
                status = m_pFeature->GetType(&myType);
                status = m_pFeature->GetIncrementType(&myIncrement);
                if (status)
                {
                    // systemGrain = 35
                    status = m_pDevice->GetFeatureValue(featureName, &nGetValue);
                    status = m_pDevice->SetFeatureValue(featureName, nSetValue);
                }
            }
        }

        if (strTemp.Compare(_T("Gain")) == 0)
        {
            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
            {
                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);
                //char name[11] = "systemGain";

                SapFeature::AccessMode myAccess;
                status = m_pFeature->GetAccessMode(&myAccess);
                status = m_pFeature->GetType(&myType);
                status = m_pFeature->GetIncrementType(&myIncrement);
                if (status)
                {
                    // systemGrain = 35
                    status = m_pDevice->GetFeatureValue(featureName, &dGetValue);
                    status = m_pDevice->SetFeatureValue(featureName, dSetValue);
                }
            }
        }
    }
}
void GrabExecutor_Sapera::ResponseLeveling()
{
    BOOL status; //  m_pDevice    m_pFeature
    BOOL isAvailable = FALSE;
    int numberOfFeatures = 0;
    m_pDevice->GetFeatureCount(&numberOfFeatures);

    SapFeature::IncrementType myIncrement;
    SapFeature::Type myType;

    for (long featureIndex = 0; featureIndex < numberOfFeatures; featureIndex++)
    {
        char featureName[108];
        status = m_pDevice->GetFeatureNameByIndex(featureIndex, featureName, sizeof(featureName));

        CString strTemp;
        strTemp = (CStringA)featureName;

        if (strTemp.Compare(_T("responseLevellingTrigger")) == 0)
        {
            if (m_pDevice->IsFeatureAvailable(featureName, &isAvailable) && isAvailable)
            {
                status = m_pDevice->GetFeatureInfo(featureName, m_pFeature);

                SapFeature::AccessMode myAccess;
                status = m_pFeature->GetAccessMode(&myAccess);
                status = m_pFeature->GetType(&myType);
                status = m_pFeature->GetIncrementType(&myIncrement);
                if (status)
                {
                    // systemGrain = 35
                    status = m_pDevice->SetFeatureValue(featureName, TRUE);
                }
            }
        }
    }
}

void GrabExecutor_Sapera::Reset()
{
    BOOL status; //  m_pDevice    m_pFeature
    BOOL isAvailable = FALSE;
    int numberOfFeatures = 0;
    m_pDevice->GetFeatureCount(&numberOfFeatures);

    SapFeature::IncrementType myIncrement;
    SapFeature::Type myType;

    CString strSensorSel = _T("0");
    CString strSensorVal = _T("1");
    SetSensorSelector(strSensorSel);
    SetSensorGain(strSensorVal);
    
}

double GrabExecutor_Sapera::GetFPGATemperature()
{
    CORSERVER hServer;
    CORCMD_DEVICE_PARAMETER parameters;
    CORHW_DEVICE_PARAMETER_GET_TEMPERATURE_DATA nbTemperature;


    CorManGetServerByName(m_chServerName, &hServer);

    parameters.deviceIndex = 0; // Not Used.
    parameters.prmSize = 0; // Not Used.

    // Temperature Info
    parameters.prmIndex = CORHW_USER_DEVICE_PRM_GET_TEMPERATURE;
    CorManControl(hServer, CORCMD_USER_DEVICE_PARAMETER_READ, &parameters, sizeof(parameters), &nbTemperature,
        sizeof(nbTemperature));

    
    double dDegree = (float)nbTemperature.value / 1000.0;

    return dDegree;
}

BOOL GrabExecutor_Sapera::GetCameraConnectCheck()
{
    return m_Xfer->IsConnected();
}

void GrabExecutor_Sapera::SetExposureTime_Multi(std::vector<CString> vecstrExposureValue, int nCntCh)
{
    return;
}