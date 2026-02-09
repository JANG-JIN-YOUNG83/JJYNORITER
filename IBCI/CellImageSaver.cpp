#include "pch.h"
#include "CellImageSaver.h"
#include "Path.h"
#include "RecyclableImage.h"
#include "SaveImageQueue.h"
#include "SystemParameter.h"

#include "Base/LoggerInterface.h"
#include "Gadget/Miscellaneous.h"

#include "../Inspection/basedef.h"
#include "../Grab/GrabExecutor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CellImageSaver::CellImageSaver(const CString& strEquipmentID, const CString& strLotID, IPVM::LoggerInterface& loggerImageSaveFailed, int nGenerationVersion, int nCISInspType)
	: m_strEquipmentID(strEquipmentID)
	, m_strLotID(strLotID)
	, m_nGenerationVersion(nGenerationVersion)
	, m_loggerImageSaveFailed(loggerImageSaveFailed)
	, m_nCISInspType(nCISInspType)
{
	m_queue[0] = new SaveImageQueue(loggerImageSaveFailed, 0);
	m_queue[1] = new SaveImageQueue(loggerImageSaveFailed, 1);

	UpdateQueueSize();
}

CellImageSaver::~CellImageSaver()
{
	delete m_queue[1];
	delete m_queue[0];
    
}

CString CellImageSaver::PushInspectedCell(const IPVM::Image_8u_C1& image, bool isUp, bool isRejected, int cellID, bool bIsNoTab,int SaveOption, LPCTSTR tag)
{
	CString strRes = isRejected ? _T("NG") : _T("OK");

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString imageFolder;

	if (g_systemParameter.nPC_ID == 1)
	{
		if (!bIsNoTab)
			imageFolder.Format(_T("%s\\%s\\%04d\\%02d\\%02d\\"), Path::GetImageFolder(), (LPCTSTR)strRes, time.wYear, time.wMonth, time.wDay);
		else
			imageFolder.Format(_T("%s\\%s\\%04d\\%02d\\%02d\\NoTab\\"), Path::GetImageFolder(), (LPCTSTR)strRes, time.wYear, time.wMonth, time.wDay);
	}
	else
		imageFolder.Format(_T("%s\\%s\\%04d\\%02d\\%02d\\"), Path::GetImageFolder(), (LPCTSTR)strRes, time.wYear, time.wMonth, time.wDay);

	IPVM::CreateDirectories(imageFolder);

	CString strInspID;
	if (Share::GenerationVersion::SplitterRemoved == m_nGenerationVersion)
	{
			strInspID = _T("CIS");
	}
	else
	{
		if (g_systemParameter.nPC_ID == 1)
			strInspID = _T("DIMENSION");
		else
			strInspID = _T("SURFACE");
	}
	CString tagText;

	if (!CString(tag).IsEmpty())
	{
		tagText.Format(_T("%s_"), tag);
	}

	ImageSaveType type = g_systemParameter.m_imageSaveType;
	CString ext;

	switch (type)
	{
	case ImageSaveType::Bmp:	ext = _T("bmp");	break;
	case ImageSaveType::Jpg:	ext = _T("jpg");	break;
	case ImageSaveType::Png:	ext = _T("png");	break;
	}

	CString fileHeader;
	fileHeader.Format(_T("%04d%02d%02d%02d%02d%02d%03d_%s_%s_%s_%s_%03d_%s"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, (LPCTSTR)m_strEquipmentID, (LPCTSTR)g_systemParameter.m_currentModel, (LPCTSTR)m_strLotID, (LPCTSTR)strInspID, cellID, (LPCTSTR)tagText);

	CString fileName;
	if (isUp)
	{
		fileName.Format(_T("%sUP.%s"), (LPCTSTR)fileHeader, (LPCTSTR)ext);
	}
	else
	{
		fileName.Format(_T("%sDN.%s"), (LPCTSTR)fileHeader, (LPCTSTR)ext);
	}

	//if (Share::GenerationVersion::SplitterRemoved != m_nGenerationVersion ||(IBNI_INSPTYPE::Type_CIS2 == m_nCISInspType || IBNI_INSPTYPE::Type_CIS1 == m_nCISInspType))
	{
		if (isRejected)
		{
			m_queue[0]->Push(image, imageFolder, fileName);
		}
		else
		{
			m_queue[1]->Push(image, imageFolder, fileName);
		}
		return imageFolder + fileName;
	}

	BOOL isExit = FALSE, isReject = FALSE;
	CellResultInfo temp;
	temp.m_bisReject = isRejected;
	temp.m_bisUp = isUp;
	temp.m_nCellID = cellID;
	temp.m_bisNotab = bIsNoTab;
	temp.tagtotalNG = tag;
	if (TRUE == isUp)
	{
		for (int i = 0; i < m_dqCellResultInfo_UP.size(); i++)
		{
			m_dqCellResultInfo_UP[i].tagtotalNG ;
			temp.tagtotalNG  ;
			if (m_dqCellResultInfo_UP[i].m_nCellID == temp.m_nCellID)
			{
				m_cs.Lock();
				isExit = TRUE;
				isReject = temp.m_bisReject || m_dqCellResultInfo_UP[i].m_bisReject;
				CString strResp = isReject ? _T("NG") : _T("OK");
				if (m_dqCellResultInfo_UP[i].m_bisNotab == TRUE || temp.m_bisNotab == TRUE)
					imageFolder.Format(_T("%s\\%s\\%04d\\%02d\\%02d\\NoTab\\"), Path::GetImageFolder(), (LPCTSTR)strResp, time.wYear, time.wMonth, time.wDay);
				else
					imageFolder.Format(_T("%s\\%s\\%04d\\%02d\\%02d\\"), Path::GetImageFolder(), (LPCTSTR)strResp, time.wYear, time.wMonth, time.wDay);

				CString UPNGname;
				UPNGname = m_dqCellResultInfo_UP[i].tagtotalNG + _T("_") +  temp.tagtotalNG;
				fileHeader.Format(_T("%04d%02d%02d%02d%02d%02d%03d_%s_%s_%s_%s_%03d_%s_"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, (LPCTSTR)m_strEquipmentID, (LPCTSTR)g_systemParameter.m_currentModel, (LPCTSTR)m_strLotID, (LPCTSTR)strInspID, cellID, UPNGname);
				fileName.Format(_T("%sUP.%s"), (LPCTSTR)fileHeader, (LPCTSTR)ext);
				
				m_dqCellResultInfo_UP.erase(m_dqCellResultInfo_UP.begin() + i);
				m_cs.Unlock();
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < m_dqCellResultInfo_DOWN.size(); i++)
		{
			m_dqCellResultInfo_DOWN[i].tagtotalNG;
			temp.tagtotalNG;
			if (m_dqCellResultInfo_DOWN[i].m_nCellID == temp.m_nCellID)
			{
				m_cs.Lock();
				isExit = TRUE;
				isReject = temp.m_bisReject || m_dqCellResultInfo_DOWN[i].m_bisReject;
				CString strResp = isReject ? _T("NG") : _T("OK");
				if (m_dqCellResultInfo_DOWN[i].m_bisNotab == TRUE || temp.m_bisNotab == TRUE)
					imageFolder.Format(_T("%s\\%s\\%04d\\%02d\\%02d\\NoTab\\"), Path::GetImageFolder(), (LPCTSTR)strRes, time.wYear, time.wMonth, time.wDay);
				else
					imageFolder.Format(_T("%s\\%s\\%04d\\%02d\\%02d\\"), Path::GetImageFolder(), (LPCTSTR)strResp, time.wYear, time.wMonth, time.wDay);

				CString DNNGname;
				DNNGname = m_dqCellResultInfo_DOWN[i].tagtotalNG+ _T("_") + temp.tagtotalNG;
				fileHeader.Format(_T("%04d%02d%02d%02d%02d%02d%03d_%s_%s_%s_%s_%03d_%s_"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, (LPCTSTR)m_strEquipmentID, (LPCTSTR)g_systemParameter.m_currentModel, (LPCTSTR)m_strLotID, (LPCTSTR)strInspID, cellID, DNNGname);
				fileName.Format(_T("%sDN.%s"), (LPCTSTR)fileHeader, (LPCTSTR)ext);

				m_dqCellResultInfo_DOWN.erase(m_dqCellResultInfo_DOWN.begin() + i);
				m_cs.Unlock();
				break;
			}
		}
	}


	if (isExit)
	{
		if (isReject)
		{
			if (4 /*save only reject image*/== SaveOption || 3 /*save all image*/ == SaveOption)
			{
				m_queue[0]->Push(image, imageFolder, fileName);
			}
		}
		else
		{
			if (3 /*save all image*/ == SaveOption)
			{
				m_queue[1]->Push(image, imageFolder, fileName);
			}
		}
	}
	else {
		m_cs.Lock();

		if(TRUE == isUp)
			m_dqCellResultInfo_UP.push_back(temp);
		else
			m_dqCellResultInfo_DOWN.push_back(temp);
		m_cs.Unlock();

	}
	//if (pow(2, g_systemParameter.nCellIDBit) < m_dqCellResultInfo_DOWN.size() && TRUE == isUp) {//최대 CELLID수만큼만 가지고있자.
	//	m_dqCellResultInfo_DOWN.pop_front();
	//	m_loggerImageSaveFailed.Log(0, _T("[ERROR] m_dqCellResultInfo_DOWN 사이즈 : %d 되어 pop front"),m_dqCellResultInfo_DOWN.size());

	//}
	//if (pow(2, g_systemParameter.nCellIDBit) < m_dqCellResultInfo_UP.size() && FALSE == isUp) {
	//	m_dqCellResultInfo_UP.pop_front();
	//	m_loggerImageSaveFailed.Log(0, _T("[ERROR] m_dqCellResultInfo_UP 사이즈 : %d 되어 pop front"), m_dqCellResultInfo_UP.size());
	//}

	return imageFolder + fileName;
}

CString CellImageSaver::PushCell(const IPVM::Image_8u_C1& image, bool isUp, long cellID, CString strResult, bool isFull)
{
	CString strPos;
	/*if (isUp)
		strPos = _T("UP");
	else
		strPos = _T("DOWN");*/
    if (isUp)
        strPos = _T("DARK");
    else
        strPos = _T("BRIGHT");

	if (isFull)
        strPos = _T("FULL");
	SYSTEMTIME time;
	GetLocalTime(&time);

	CString imageFolder;
    //imageFolder.Format(_T("%s\\TOTAL\\%04d\\%02d\\%02d\\"), Path::GetImageFolder(), time.wYear, time.wMonth, time.wDay);
    imageFolder.Format(_T("%s\\%04d\\%02d\\%02d\\"), Path::GetImageFolder(), time.wYear, time.wMonth, time.wDay);

    IPVM::CreateDirectories(imageFolder);

	CString strInspID;

	if (g_systemParameter.nPC_ID == 1)
		strInspID = _T("DIMENSION");
	else
		strInspID = _T("SURFACE");
    strInspID = _T("MISSMATCH");
	ImageSaveType type = g_systemParameter.m_imageSaveType;
	CString ext;

	switch (type)
	{
	case ImageSaveType::Bmp:	ext = _T("bmp");	break;
	case ImageSaveType::Jpg:	ext = _T("jpg");	break;
	case ImageSaveType::Png:	ext = _T("png");	break;
	}
    //*m_strEquipmentID = _T("test");
   // *m_strLotID = _T("test");
	CString fileName;
	fileName.Format(_T("%09d_%s_%s_%s_%s_%s.%s"), cellID, (LPCTSTR)strPos, 
        
		_T("test"),//(LPCTSTR)m_strEquipmentID,
		(LPCTSTR)strInspID, _T("test"),//(LPCTSTR)g_systemParameter.m_currentModel,
        strResult, //(LPCTSTR)m_strLotID,
		
		
		(LPCTSTR)ext);

	if (isFull)
	{
		m_queue[0]->Push(image, imageFolder, fileName);
	}
	else
	{
		m_queue[1]->Push(image, imageFolder, fileName);
	}

	return imageFolder + fileName;
}

CString CellImageSaver::PushCell(const IPVM::Image_8u_C1& image, bool isUp, long cellID)
{
    CString strPos;
    /*if (isUp)
		strPos = _T("UP");
	else
		strPos = _T("DOWN");*/
    if (isUp)
        strPos = _T("DARK");
    else
        strPos = _T("BRIGHT");
    SYSTEMTIME time;
    GetLocalTime(&time);

    CString imageFolder;
    //imageFolder.Format(_T("%s\\TOTAL\\%04d\\%02d\\%02d\\"), Path::GetImageFolder(), time.wYear, time.wMonth, time.wDay);
    imageFolder.Format(
        _T("%s\\%04d\\%02d\\%02d\\"), g_systemParameter.strPathImageSave, time.wYear, time.wMonth, time.wDay);

    IPVM::CreateDirectories(imageFolder);

    CString strInspID;

    if (g_systemParameter.nPC_ID == 1)
        strInspID = _T("DIMENSION");
    else
        strInspID = _T("SURFACE");

	switch (g_systemParameter.machineType)
    {
        case MachineType::UC1_mismatch:
        case MachineType::ES_mismatch:
            strInspID = _T("MISSMATCH");
            break;
        case MachineType::ES_rollpress_pinhole:
            strInspID = _T("PINHOLE");
            break;
        case MachineType::ES_rollpress_surface:
            strInspID = _T("SURFACE");
            break;
    }

    ImageSaveType type = g_systemParameter.m_imageSaveType;
    CString ext;

    switch (type)
    {
        case ImageSaveType::Bmp:
            ext = _T("bmp");
            break;
        case ImageSaveType::Jpg:
            ext = _T("jpg");
            break;
        case ImageSaveType::Png:
            ext = _T("png");
            break;
    }
    //*m_strEquipmentID = _T("test");
    // *m_strLotID = _T("test");
    CString fileName;
    fileName.Format(_T("%04d%02d%02d%02d%02d%02d%03d_%s_%s_%s_%s_%s_%03d.%s"), time.wYear, time.wMonth, time.wDay,
        time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
        _T("test"), //(LPCTSTR)m_strEquipmentID,
        (LPCTSTR)strInspID, _T("test"), //(LPCTSTR)g_systemParameter.m_currentModel,
        _T("test"), //(LPCTSTR)m_strLotID,
        (LPCTSTR)strPos, cellID, (LPCTSTR)ext);

    if (isUp)
    {
        m_queue[0]->Push(image, imageFolder, fileName);
    }
    else
    {
        m_queue[1]->Push(image, imageFolder, fileName);
    }

    return imageFolder + fileName;
}

void CellImageSaver::PushLineBlankImage(const IPVM::Image_8u_C1& image, bool isUp)
{
	CString strCamera = (isUp == true) ? _T("[UP]") : _T("[DOWN]");

	SYSTEMTIME time;

	GetLocalTime(&time);

	CString strFileName;

	strFileName.Format(_T("%04dY%02dM%02dD%02dH%02dm%02ds%03dms_%s.bmp"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, strCamera);

	CString strSavePath;
	strSavePath.Format(_T("C:\\Intekplus\\IBNI\\Line Blank Image\\%04d\\%02d\\%02d\\"), time.wYear, time.wMonth, time.wDay);

	IPVM::CreateDirectories(strSavePath);

	if (isUp)
		m_queue[0]->Push(image, strSavePath, strFileName);
	else
		m_queue[1]->Push(image, strSavePath, strFileName);
}

void CellImageSaver::PushCircularBufferImage(const IPVM::Image_8u_C1& previmage, const IPVM::Image_8u_C1& curimage, bool isUp)
{
	CString strCamera = (isUp == true) ? _T("[UP]") : _T("[DOWN]");

	SYSTEMTIME time;

	GetLocalTime(&time);

	CString strFileName, strFileName2;

	strFileName.Format(_T("%04dY%02dM%02dD%02dH%02dm%02ds%03dms_%s_Circular_Prev.bmp"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, strCamera);
	
	::Sleep(1);

	GetLocalTime(&time);
	
	strFileName2.Format(_T("%04dY%02dM%02dD%02dH%02dm%02ds%03dms_%s_Circular_Cur.bmp"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, strCamera);

	CString strSavePath;
	strSavePath.Format(_T("C:\\Intekplus\\IBNI\\Line Blank Image\\%04d\\%02d\\%02d\\"), time.wYear, time.wMonth, time.wDay);

	IPVM::CreateDirectories(strSavePath);

	if (isUp)
	{
		m_queue[0]->Push(previmage, strSavePath, strFileName);
		m_queue[0]->Push(curimage, strSavePath, strFileName2);
	}
	else
	{
		m_queue[1]->Push(previmage, strSavePath, strFileName);
		m_queue[1]->Push(curimage, strSavePath, strFileName2);
	}
}

void CellImageSaver::PushBufferAlarmImage(const IPVM::Image_8u_C1& image, bool isUp)
{
	CString strCamera = (isUp == true) ? _T("[UP]") : _T("[DOWN]");

	SYSTEMTIME time;

	GetLocalTime(&time);

	CString strFileName;

	strFileName.Format(_T("%04dY%02dM%02dD%02dH%02dm%02ds%03dms_%s.bmp"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, strCamera);

	CString strSavePath;
	strSavePath.Format(_T("C:\\Intekplus\\IBNI\\Buffer_Alarm_Image\\%04d\\%02d\\%02d\\"), time.wYear, time.wMonth, time.wDay);

	IPVM::CreateDirectories(strSavePath);

	if (isUp)
		m_queue[0]->Push(image, strSavePath, strFileName);
	else
		m_queue[1]->Push(image, strSavePath, strFileName);
}

void CellImageSaver::UpdateQueueSize()
{
	m_queue[0]->SetMaximumQueueSize(g_systemParameter.m_imageQueueSize);
	m_queue[1]->SetMaximumQueueSize(g_systemParameter.m_imageQueueSize);
}

void CellImageSaver::ClearCellResultQueue()
{
	m_dqCellResultInfo_UP.clear();
	m_dqCellResultInfo_DOWN.clear();
}