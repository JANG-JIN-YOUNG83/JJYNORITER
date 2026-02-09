#pragma once
#include "Gadget/SimpleWorkerThread.h"

#include <queue>

struct SaveImageData
{
	IPVM::Image_8u_C1 m_image;
	CString m_filePath;

	IPVM::Status Save(int& nSizeX, int& nSizeY);
};

class SaveImageQueue : public IPVM::SimpleWorkerThread
{
public:
	SaveImageQueue(IPVM::LoggerInterface& loggerImageSaveFailed, const long index);
	~SaveImageQueue();

	void SetMaximumQueueSize(long maximumSize);
	void Push(const IPVM::Image_8u_C1& image, LPCTSTR folderPath, LPCTSTR fileName);

private:
	virtual bool OnTrigger() override;

	IPVM::LoggerInterface& m_loggerImageSaveFailed;
	const long m_index;

	CCriticalSection m_cs;
	long m_maximumSize;
	std::queue<SaveImageData*> m_queue;
	std::queue<SaveImageData*> m_idleQueue;

	void Release(SaveImageData* data);
	SaveImageData* Pop();
	SaveImageData* GetBuffer();
};
