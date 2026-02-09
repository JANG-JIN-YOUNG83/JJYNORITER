#pragma once

//#include "../SharedModule/VisionHostShare/HostCommSocket.h"

#ifdef __GRAB_CLASS_EXPORT__
#define __GRAB_CLASS__	__declspec(dllexport)
#else
#define __GRAB_CLASS__	__declspec(dllimport)
#endif


enum class GrabMode;
class GrabExecutor;
enum class ExecutorType;
enum class MachineType;

class GrabExecutor_Coaxlink;

class __GRAB_CLASS__ GrabExecutorContainer
{
public:
	GrabExecutorContainer() = delete;
	GrabExecutorContainer(const GrabExecutorContainer &rhs) = delete;
	GrabExecutorContainer(IPVM::LoggerInterface& logger, LPCTSTR cfPathName, const MachineType machineType, const ExecutorType executorType);
	~GrabExecutorContainer();

	GrabExecutorContainer &operator=(const GrabExecutorContainer &rhs) = delete;

public:
	GrabExecutor *GetExecutor(int cameraIndex);

	bool IsInitializeOk() const;
	void Start(GrabMode grabMode);
	void Stop();

	void SetAlarm(long ncode);
	int GetCameraCount();

	void SetPageLength(long nCameraIndex, long nPageLength);

private:
	GrabExecutor_Coaxlink* m_Euresys;
	IPVM::LoggerInterface& m_logger;

	int m_nCameraCount;
	GrabExecutor *m_camera[5];		// UP, DOWN
									// Grabber 0, 1, 2 - camera 0, 1, 2(medium, medium, base)
									// Grabber 0, 1, 2 - camera 0, 1, 2, 3, 4(base * 5)
};

