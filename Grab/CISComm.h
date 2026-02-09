#pragma once
#include "basedef.h"
//#include "../SharedModule/VisionHostShare/HostCommSocket.h"
#include "CISParameter.h"

enum class MachineType;
enum class ExecutorType;
enum class GrabMode;
class CCisSetting;
class Database;
class GrabExecutor;
class GrabExecutorContainer;

class __GRAB_CLASS__ CISComm
{
public:
	CISComm(IPVM::LoggerInterface& loggerSequence, IPVM::LoggerInterface& loggerGrab, MachineType machineType, int nPC_ID, LPCTSTR cfPath, ExecutorType executorType, int nGenerationVersion);
	~CISComm();

	void LinkDataBase(bool bSave, Database &dbJob);
	bool IsGrabExcutorContainerIntialized() const;

	GrabExecutor* GetGrabExcutor(int cameraIndex);
	bool SplitterStart();
	bool SplitterStop();

	CCisSetting* GetSetting();

	void GrabExcutorContainerStart(GrabMode grabMode);
	void GrabExcutorContainerStop();

	CCriticalSection m_csCisScanStart;
	static UINT ThreadCisMasterScanStart(LPVOID pParam);
	UINT ThreadCisMasterScanStart();

	int GetCameraCount();
	void SetAlarmCode(long nCode);

	CISParameter m_paraCur;
	CISParameter m_paraEdit;

	ExecutorType GetExecutorType();
	MachineType GetMachineType();

	int m_selectedPortIndex;

private:
	IPVM::LoggerInterface& m_loggerSequence;
	IPVM::LoggerInterface& m_loggerGrab;
	//int m_nCamSingleDual;
	int m_nPC_ID;
	int m_nGenerationVersion;
	//bool m_isExistGrabHardware;
	ExecutorType m_executorType;
	MachineType  m_machineType;
	CCisSetting* m_pCisSetting;
	GrabExecutorContainer *m_grabExecutorContainer;
};
