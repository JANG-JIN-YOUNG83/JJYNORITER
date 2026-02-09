#pragma once

#include <deque>
#include "SaveImageQueue.h"
struct CellResultInfo
{
	int m_nCellID;
	BOOL m_bisReject;
	BOOL m_bisUp;
	BOOL m_bisNotab;
	CString tagtotalNG;
};
class CellImageSaver
{
public:
	CellImageSaver() = delete;
	CellImageSaver(const CellImageSaver& rhs) = delete;
	CellImageSaver(const CString& strEquipmentID, const CString& strLotID, IPVM::LoggerInterface& loggerImageSaveFailed, int nGenerationVersion, int nCISInspType);
	virtual ~CellImageSaver();

	CellImageSaver& operator=(const CellImageSaver& rhs) = delete;

public:
	CString PushInspectedCell(const IPVM::Image_8u_C1& image, bool isUp, bool isRejected, int cellID, bool bIsNoTab, int SaveOption, LPCTSTR tag = _T(""));
	CString PushCell(const IPVM::Image_8u_C1& image, bool isUp, long cellID);
    CString PushCell(const IPVM::Image_8u_C1& image, bool isUp, long cellID, CString strResult, bool isFull = FALSE);
	void PushLineBlankImage(const IPVM::Image_8u_C1& image, bool isUp);
	void PushCircularBufferImage(const IPVM::Image_8u_C1& previmage, const IPVM::Image_8u_C1& curimage, bool isUp);
	void PushBufferAlarmImage(const IPVM::Image_8u_C1& image, bool isUp);
	void UpdateQueueSize();
	void ClearCellResultQueue();
private:
	const CString& m_strEquipmentID;
	const CString& m_strLotID;
	IPVM::LoggerInterface& m_loggerImageSaveFailed;

	SaveImageQueue* m_queue[2];
	int m_nGenerationVersion;
	int m_nCISInspType;
	std::deque<CellResultInfo>	m_dqCellResultInfo_UP;
	std::deque<CellResultInfo>	m_dqCellResultInfo_DOWN;
	CCriticalSection m_cs;
};

