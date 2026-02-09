#include "pch.h"
#include "SystemParameter.h"
#include "Path.h"

#include "../CompanySharedLibrary/Persistence/IniHelper.h"
#include "../Module/Utility/IBNI_PCSetting.h"
#include "../Grab/GrabExecutor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SystemParameter g_systemParameter;

SystemParameter::SystemParameter()
{
}

SystemParameter::~SystemParameter()
{
}

void SystemParameter::Load()
{
	CStdioFile file;
	CString filePath;
	filePath.Format(_T("%s\\System.ini"), Path::GetRootFolder());

	m_isExistGrabHardware = IniHelper::LoadINT(filePath, _T("System"), _T("Exist Grab Hardware"), TRUE) ? TRUE : FALSE;
	strCfPath = IniHelper::LoadSTRING(filePath, _T("System"), _T("CF Path"), 0);
	nPC_ID = IniHelper::LoadINT(filePath, _T("System"), _T("PC ID"), 1);
	nAnodeCathode = IniHelper::LoadINT(filePath, _T("System"), _T("Anode / Cathode"), 0);
	bSensorImageMirror = IniHelper::LoadINT(filePath, _T("System"), _T("Sensor Image Mirror"), 0);
	bXaxisMirror = IniHelper::LoadINT(filePath, _T("Image Send"), _T("Roll Map X Axis Mirror"), 0);
	bYaxisMirror = IniHelper::LoadINT(filePath, _T("Image Send"), _T("Roll Map Y Axis Mirror"), 0);

	m_dPixelToUmXUp = IniHelper::LoadDOUBLE(filePath, _T("Optics"), _T("Pixel To Um X"), 40);
	m_dPixelToUmXDown = IniHelper::LoadDOUBLE(filePath, _T("Optics"), _T("Pixel To Um X Down"), m_dPixelToUmXUp);
	m_dPixelToUmY = IniHelper::LoadDOUBLE(filePath, _T("Optics"), _T("Pixel To Um Y"), 40);

	m_currentModel = IniHelper::LoadSTRING(filePath, _T("Miscellaneous"), _T("Last Recipe"), 0);


	nLanguage = IniHelper::LoadINT(filePath, _T("System"), _T("Language"), 1);
	executorType = (ExecutorType)IniHelper::LoadINT(filePath, _T("System"), _T("ExecutorType"), 0);
	machineType  = (MachineType)IniHelper::LoadINT(filePath, _T("System"), _T("MachineType"), 0);


	bUseISI = IniHelper::LoadINT(filePath, _T("System"), _T("Use_ISI"), 0);
	fCuttingPitch = IniHelper::LoadDOUBLE(filePath, _T("Inspection"), _T("Cutting Pitch"), 100.);
	nCuttingMargin = IniHelper::LoadINT(filePath, _T("Inspection"), _T("Cutting Margin"), 200);


	m_coreCount = IniHelper::LoadINT(filePath, _T("Core"), _T("Core Count"), 5);
	
	m_imageQueueSize = IniHelper::LoadINT(filePath, _T("File Save"), _T("Image Queue Size"), 100);
	m_imageSaveOption = IniHelper::LoadINT(filePath, _T("File Save"), _T("Image Save"), 2);
	m_imageSaveType = (ImageSaveType)IniHelper::LoadINT(filePath, _T("File Save"), _T("Image Save Type"), (int)ImageSaveType::Jpg);
    m_nImageResize = IniHelper::LoadINT(filePath, _T("File Save"), _T("Image Resize"), 1);
	szMachineNumber = IniHelper::LoadSTRING(filePath, _T("File Save"), _T("Machine Number"), _T(""));
    bBatchFolderTree = IniHelper::LoadINT(filePath, _T("File Save"), _T("Batch Folder Tree"), 0);


	szConnectIP = IniHelper::LoadSTRING(filePath, _T("TCP/IP Info"), _T("Connect IP"), _T("10.0.10.10"));
	nConnectPort = IniHelper::LoadINT(filePath, _T("TCP/IP Info"), _T("Connect Port"), 5001);
	nConnectPort2 = IniHelper::LoadINT(filePath, _T("TCP/IP Info"), _T("Connect Port2"), 5002);

	bConnectionTapeInterlock = IniHelper::LoadINT(filePath, _T("System"), _T("Connection Tape Interlock"), 1);

	bUseServer = IniHelper::LoadINT(filePath, _T("System"), _T("bUseServer"), 0);
	bServer = IniHelper::LoadINT(filePath, _T("System"), _T("bServer"), 0);
	strHostIP = IniHelper::LoadSTRING(filePath, _T("System"), _T("Host IP"), _T("10.0.10.10"));
	nPort = IniHelper::LoadINT(filePath, _T("System"), _T("Port"), 5000);
	nClientPCID = IniHelper::LoadINT(filePath, _T("System"), _T("Client PC ID"), 1);

	bDirection = IniHelper::LoadINT(filePath, _T("System"), _T("TOP/BACK"), 0);
    bOSDS = IniHelper::LoadINT(filePath, _T("System"), _T("OS/DS"), 1);
    bRollerOS = IniHelper::LoadINT(filePath, _T("System"), _T("Start OS"), 1);
    bRollerTwoSide = IniHelper::LoadINT(filePath, _T("System"), _T("Roller TwoSide"), 0);

	nLaneCount = IniHelper::LoadINT(filePath, _T("System"), _T("Lane 1/2"), 2);

	bDefectCsvSave = IniHelper::LoadINT(filePath, _T("System"), _T("Defect CSV Save"), 0);
	bMSASave = IniHelper::LoadINT(filePath, _T("System"), _T("MSA CSV Save"), 0);
	bMeasureSave = IniHelper::LoadINT(filePath, _T("System"), _T("Measure CSV Save"), 0);
    bMismatchSave = IniHelper::LoadINT(filePath, _T("System"), _T("Mismatch CSV Save"), 0);

	bNotPair = IniHelper::LoadINT(filePath, _T("System"), _T("Not Pair"), 0);
    pathResultDataCsv = IniHelper::LoadSTRING(filePath, _T("File Save"), _T("Path_Result_Data_Csv"), _T(""));
    bDisplayShow = IniHelper::LoadINT(filePath, _T("System"), _T("Display Show"), 0);

	strPathImageSave = IniHelper::LoadSTRING(filePath, _T("File Save"), _T("Path_Image_Save"), _T(""));
    nWarningCnt = IniHelper::LoadINT(filePath, _T("System"), _T("Warning Count"), 0);
    nSpecCnt = IniHelper::LoadINT(filePath, _T("System"), _T("Spec Count"), 0);
    nEdgeViewImageType = IniHelper::LoadINT(filePath, _T("System"), _T("EdgeView Image Type"), 0);
    bEdgeViewSampling = IniHelper::LoadINT(filePath, _T("System"), _T("EdgeView Sampling"), 0);
    nSamplingCnt = IniHelper::LoadINT(filePath, _T("System"), _T("Sampling Count"), 0);
    
#if INSPECTION_MACHINE_UC1
    CString strPath = IniHelper::LoadSTRING(filePath, _T("SavePath"), _T("SavePath"), _T("Z:\\TOP"));
    Path::SetImageFolder(strPath);
#else
    //CString strPath = IniHelper::LoadSTRING(filePath, _T("SavePath"), _T("SavePath"), _T("Z:\\TOP"));
    //Path::SetImageFolder(strPath);	
#endif // 

    //<<<<<<< .mine
    
//||||||| .r427
//=======
//
//    strPathImageSave = IniHelper::LoadSTRING(filePath, _T("File Save"), _T("Path_Image_Save"), _T(""));
//>>>>>>> .r434
}

void SystemParameter::Save()
{
	CString filePath;
	filePath.Format(_T("%s\\System.ini"), Path::GetRootFolder());
	SavePara(filePath);

	SavePara(MakeBackupPath());
}

void SystemParameter::SavePara(CString filePath)
{
	IniHelper::SaveSTRING(filePath, _T("System"), _T("CF Path"), strCfPath);
	IniHelper::SaveINT(filePath, _T("System"), _T("Use_ISI"), bUseISI);
	IniHelper::SaveINT(filePath, _T("System"), _T("PC ID"), nPC_ID);
	IniHelper::SaveINT(filePath, _T("System"), _T("Anode / Cathode"), nAnodeCathode);
	IniHelper::SaveINT(filePath, _T("System"), _T("Sensor Image Mirror"), bSensorImageMirror);
	IniHelper::SaveINT(filePath, _T("System"), _T("Exist Grab Hardware"), m_isExistGrabHardware);

	IniHelper::SaveINT(filePath, _T("Image Send"), _T("Roll Map X Axis Mirror"), bXaxisMirror);
	IniHelper::SaveINT(filePath, _T("Image Send"), _T("Roll Map Y Axis Mirror"), bYaxisMirror);

	IniHelper::SaveDOUBLE(filePath, _T("Optics"), _T("Pixel To Um X"), m_dPixelToUmXUp);
	IniHelper::SaveDOUBLE(filePath, _T("Optics"), _T("Pixel To Um X Down"), m_dPixelToUmXDown);
	IniHelper::SaveDOUBLE(filePath, _T("Optics"), _T("Pixel To Um Y"), m_dPixelToUmY);

	IniHelper::SaveINT(filePath, _T("System"), _T("Language"), nLanguage);
	IniHelper::SaveINT(filePath, _T("System"), _T("ExecutorType"), (long)executorType);
	IniHelper::SaveINT(filePath, _T("System"), _T("MachineType"), (long)machineType);


	IniHelper::SaveDOUBLE(filePath, _T("Inspection"), _T("Cutting Pitch"), fCuttingPitch);
	IniHelper::SaveINT(filePath, _T("Inspection"), _T("Cutting Margin"), nCuttingMargin);

	IniHelper::SaveDOUBLE(filePath, _T("Core"), _T("Core Count"), m_coreCount);
	IniHelper::SaveSTRING(filePath, _T("Miscellaneous"), _T("Last Recipe"), m_currentModel);

	IniHelper::SaveINT(filePath, _T("File Save"), _T("Image Queue Size"), m_imageQueueSize);
	IniHelper::SaveINT(filePath, _T("File Save"), _T("Image Save"), m_imageSaveOption);
	IniHelper::SaveINT(filePath, _T("File Save"), _T("Image Save Type"), (int)m_imageSaveType);
    IniHelper::SaveINT(filePath, _T("File Save"), _T("Image Resize"), (int)m_nImageResize);
	IniHelper::SaveSTRING(filePath, _T("File Save"), _T("Machine Number"), szMachineNumber);
    IniHelper::SaveINT(filePath, _T("File Save"), _T("Batch Folder Tree"), bBatchFolderTree);

	IniHelper::SaveSTRING(filePath, _T("TCP/IP Info"), _T("Connect IP"), szConnectIP);
	IniHelper::SaveINT(filePath, _T("TCP/IP Info"), _T("Connect Port"), nConnectPort);
	IniHelper::SaveINT(filePath, _T("TCP/IP Info"), _T("Connect Port2"), nConnectPort2);

	IniHelper::SaveINT(filePath, _T("System"), _T("Connection Tape Interlock"), bConnectionTapeInterlock);

	IniHelper::SaveINT(filePath, _T("System"), _T("bUseServer"), bUseServer);
	IniHelper::SaveINT(filePath, _T("System"), _T("bServer"), bServer);
	IniHelper::SaveSTRING(filePath, _T("System"), _T("Host IP"), strHostIP);
	IniHelper::SaveINT(filePath, _T("System"), _T("Port"), nPort);
	IniHelper::SaveINT(filePath, _T("System"), _T("Client PC ID"), nClientPCID);
	
	IniHelper::SaveINT(filePath, _T("System"), _T("TOP/BACK"), bDirection);
	IniHelper::SaveINT(filePath, _T("System"), _T("OS/DS"), bOSDS);
    IniHelper::SaveINT(filePath, _T("System"), _T("Start OS"), bRollerOS);
    IniHelper::SaveINT(filePath, _T("System"), _T("Roller TwoSide"), bRollerTwoSide);
	IniHelper::SaveINT(filePath, _T("System"), _T("Lane 1/2"), nLaneCount);
	IniHelper::SaveINT(filePath, _T("System"), _T("Defect CSV Save"), bDefectCsvSave);
	IniHelper::SaveINT(filePath, _T("System"), _T("MSA CSV Save"), bMSASave);
	IniHelper::SaveINT(filePath, _T("System"), _T("Measure CSV Save"), bMeasureSave);
	IniHelper::SaveINT(filePath, _T("System"), _T("Mismatch CSV Save"), bMismatchSave);
    IniHelper::SaveINT(filePath, _T("System"), _T("Not Pair"), bNotPair);
    IniHelper::SaveINT(filePath, _T("System"), _T("Warning Count"), nWarningCnt);
    IniHelper::SaveINT(filePath, _T("System"), _T("Spec Count"), nSpecCnt);
    IniHelper::SaveINT(filePath, _T("System"), _T("EdgeView Image Type"), nEdgeViewImageType);
    IniHelper::SaveINT(filePath, _T("System"), _T("EdgeView Sampling"), bEdgeViewSampling);
    IniHelper::SaveINT(filePath, _T("System"), _T("Sampling Count"), nSamplingCnt);


    IniHelper::SaveSTRING(filePath, _T("File Save"), _T("Path_Result_Data_Csv"), pathResultDataCsv);
    IniHelper::SaveINT(filePath, _T("System"), _T("Display Show"), bDisplayShow);

	IniHelper::SaveINT(filePath, _T("System"), _T("Motor IO Output Retry"), m_nIORetry);
    IniHelper::SaveSTRING(filePath, _T("File Save"), _T("Path_Image_Save"), strPathImageSave);
}


CString SystemParameter::MakeBackupPath()
{
	CString filePath;
	filePath.Format(_T("%s\\System_Backup"), Path::GetRootFolder());
	
	CreateDirectory(filePath, NULL);
	
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString strTime;
	strTime.Format(_T("%04dY%02dM%02dD%02dH%02dm%02ds%03dms_"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	
	filePath.Append(_T("\\"));
	filePath.Append(strTime);
	filePath.Append(m_currentModel);
	filePath.Append(_T("_System.ini"));


	return filePath;
}

LPCTSTR SystemParameter::GetConnectIP() const
{
	if (Utility::IBNI_PCSetting::IsEnabledSubOption_UseLocalNetworkForVision())
	{
		return _T("127.0.0.1");
	}

	return szConnectIP;
}

bool SystemParameter::IsExistGrabHardware() const
{
	if (Utility::IBNI_PCSetting::IsEnabledSubOption_NoHardware()) return false;

	return m_isExistGrabHardware ? true : false;
}
bool SystemParameter::GetImageSaveAll() const
{
	if (2 == m_imageSaveOption || 3 == m_imageSaveOption)
	{
		return true;
	}
	else
		return false;
}

long SystemParameter::GetCoreCount() const
{
	// Simulation PC에서는 Core할당을 2개만 하자 (다른 프로그램도 모두 한 PC에서 띄워야 하기 때문에)
	if (Utility::IBNI_PCSetting::IsEnableSimulationMode()) return 2;

	return m_coreCount;
}

long SystemParameter::GetImageSaveOption() const
{
	if (Utility::IBNI_PCSetting::IsEnabledSubOption_DoNotSaveVisionImage()) return 0;	// NONE

	return m_imageSaveOption;
}
