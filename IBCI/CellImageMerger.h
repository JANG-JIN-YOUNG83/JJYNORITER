#pragma once
#include "basedef.h"
#include <deque>
#include <vector>
#include <atomic>
#include "Gadget/SimpleWorkerThread.h"
#include "../Grab/GrabExecutor.h"
//struct CELL_DESIGN;
//struct InlineImageInfo;
//class CSyncboard;
//class InspectionInputData;

typedef void(*CALLBACK_ON_FRAME_MERGED)(void* callbackData, UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error);

class CellImageMerger : public IPVM::SimpleWorkerThread
{
public:
	CellImageMerger() = delete;
	CellImageMerger(const CellImageMerger &rhs) = delete;
	//CellImageMerger(CISCommSocket& ClientSocket, GrabExecutor& grabExecutor, CELL_DESIGN& cellDesign, IPVM::LoggerInterface& loggerTime, IPVM::LoggerInterface& loggerSensorPosition, IPVM::LoggerInterface& loggerSequence, bool upSide);
	//CellImageMerger(IPVM::LoggerInterface& loggerTime, IPVM::LoggerInterface& loggerSensorPosition, IPVM::LoggerInterface& loggerSequence);
    CellImageMerger(GrabExecutor** grabExecutor, long sizeExecutor, IPVM::LoggerInterface& loggerTime,
		IPVM::LoggerInterface& loggerSensorPosition, IPVM::LoggerInterface& loggerSequence,
        IPVM::LoggerInterface& loggerGrab);
	virtual ~CellImageMerger();

	CellImageMerger &operator=(const CellImageMerger &rhs) = delete;

	void RegisterCallbackOnFrameMerged(CALLBACK_ON_FRAME_MERGED callbackFunc, void *callbackData);

	UINT GetCircularBufferCount() const;
	void SetHorizontalSize(UINT horizontalSize);
	void SetVerticalSize(UINT verticalSize);
	void InitializeMergedBuffer();
	void InitializeMergedBufferReady(int cameraCount);
	bool IsMergedBufferReady(int bufferIndex);
	void TriggerMerge(int bufferindex, int cameraIndex, bool bValue);

	void Start();
	void Stop();	

	UINT GetHorizontalSize() const;
    UINT GetVerticalSize() const;

    GrabExecutor* m_grabExecutor[4];
    long m_sizeExecutor;

    void GetUpdatedFrameIndices(int64_t& headFrameIndex, int64_t& tailFrameIndex) const;
    const unsigned char* GetFrameAddress(const int64_t frameIndex) const;
    const unsigned char* GetSecFrameAddress(const int64_t frameIndex) const;
    const unsigned char* GetTotalFrameAddress(const int64_t frameIndex) const;
    const unsigned char* GetTotalSecFrameAddress(const int64_t frameIndex) const;
	
	float GetFrameGrabbedTime_ms(const int64_t frameIndex) const;
    void ProceedHeadFrameIndex(int64_t offset);
    void SetHeadFrameIndexInterval(int interval);
    //11.5세대
    void SetFrameAddress(int64_t frameIndex, int64_t index, unsigned char value);

    int64_t m_headFrameIndex; // Closed boundary
    int64_t m_tailFrameIndex; // Opened boundary
    int64_t m_maxUsedFrames;
    bool m_firstMergeAfterStart;

protected:
	virtual bool OnTrigger() override;
	static const UINT m_mergedBufferCount = 10;
    
	UINT	m_horizontalSize;
	UINT	m_verticalSize;
	//std::deque<DWORD>	m_qMergeCount;
    std::vector<DWORD> m_qMergeCount;

	unsigned char* m_mergedBuffer[m_mergedBufferCount];
    unsigned char* m_mergeSecdBuffer[m_mergedBufferCount];
    unsigned char* m_mergedTotalBuffer[1];
    unsigned char* m_mergeTotlaSecdBuffer[1];

	std::vector<std::vector<bool>> m_vvbBufferReady;
	//float m_frameGrabbedTimes_ms[m_circularBufferCount];

private:
	CALLBACK_ON_FRAME_MERGED m_funcCallbackOnFrameMerged;
	void* m_dataCallbackOnFrameMerged;

	IPVM::LoggerInterface& m_loggerGrab;
	IPVM::LoggerInterface& m_loggerTime;
	IPVM::LoggerInterface& m_loggerSensorPosition;
	IPVM::LoggerInterface& m_loggerSequence;

	CCriticalSection m_csMerger;




	//void PushSensorInfo(int64_t sensorPosition, long sensorId);

	//int64_t GetLatestSensorPosition();
	//long GetLatestSensorID();

	//void ClearLot();

private:
	//bool m_bIsTabLeft;
	//IPVM::Rect m_tabRect;
	//CISCommSocket& m_pClientSocket;

public:
	//bool m_bIsFirst;

public:
	//void SetTabRect(IPVM::Rect tabRect){ m_tabRect = tabRect; }
	////1.5세대
	//void PushSyncCellID(long sensorId);
	//long FrontSyncCellID();
	//int PopfrontSyncCellID();
	//void ClearSyncCellID();
	//long SizeSyncCellID();
	//long LastestGradationValue;
	//long GradationAndCellID_Offset;
	//BOOL CheckGradation(long Value);

	//CALLBACK_ON_CELL_IMAGE_PRODUCED m_funcCallbackOnCellImageProduced_Cutting;
	//void* m_dataCallbackOnCellImageProduced_Cutting;
	//!1.5세대

private:
	//bool AlignInlineImage(bool isUp, const std::deque<InlineImageInfo> &inlineImageQueue, const std::deque<int64_t> &sensorPositionGroup, const std::deque<long> &sensorIDGroup);
	//void ConvertInlinePosition(const std::deque<InlineImageInfo> &inlineImageQueue, int64_t positionAcc, int& imageID, int64_t& position);//이미지 커팅을 위한 값 계산하는 함수
	//void CutInlineImage(const std::deque<InlineImageInfo> &inlineImageQueue, bool isUp, long cellID, int imageID1, int64_t position1, int cutSize);

	//GrabExecutor &m_grabExecutor;
	//CELL_DESIGN &m_cellDesign;
	//const bool m_upSide;

	//CCriticalSection	m_sensorGroupLock;
	//std::deque<int64_t>	m_sensorPositionGroup;
	//std::deque<long>	m_sensorIDGroup;
	//long				m_LastestSensorID;
	//int64_t				m_LastestSensorPosition;
	////1.5세대
	//CCriticalSection	m_SyncCellIDGroupLock;
	//CCriticalSection	m_GradationCountLock;
	//std::deque<long>	m_CellIDGroup;
	////!1.5세대
	//int testcellid;
};

