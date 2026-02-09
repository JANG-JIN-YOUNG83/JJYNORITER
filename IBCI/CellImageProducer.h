#pragma once

#include "../CompanySharedLibrary/LogControl/SimpleListBoxLogger.h"
#include "Gadget/SimpleWorkerThread.h"
#include <deque>
//class GrabExecutor;
class CellImageMerger;
struct CELL_DESIGN;
struct InlineImageInfo;
class CSyncboard;
class InspectionInputData;

typedef void (*CALLBACK_ON_CELL_IMAGE_PRODUCED)(void* callbackData, InspectionInputData& inputData);

class CellImageProducer : public IPVM::SimpleWorkerThread
{
public:
    CellImageProducer() = delete;
    CellImageProducer(const CellImageProducer& rhs) = delete;
    CellImageProducer(CellImageMerger& cellImageMerger, IPVM::LoggerInterface& loggerTime,
        IPVM::LoggerInterface& loggerSensorPosition, IPVM::LoggerInterface& loggerSequence, bool upSide);
    //CellImageProducer(CISCommSocket& ClientSocket, GrabExecutor &grabExecutor, CELL_DESIGN &cellDesign, IPVM::LoggerInterface& loggerTime, IPVM::LoggerInterface& loggerSensorPosition, IPVM::LoggerInterface& loggerSequence, bool upSide);
    virtual ~CellImageProducer();

    CellImageProducer& operator=(const CellImageProducer& rhs) = delete;

public:
    void RegisterCallbackOnCellImageProduced(CALLBACK_ON_CELL_IMAGE_PRODUCED callbackFunc, void* callbackData);
    void PushSensorInfo(int64_t sensorPosition, long sensorId);

    void SetLatestSensorPosition(int64_t value);
    int64_t GetLatestSensorPosition();
    long GetLatestSensorID();

    void ClearLot();

private:
    bool m_bIsTabLeft;
    IPVM::Rect m_tabRect;
    //CISCommSocket& m_pClientSocket;

public:
    bool m_bIsFirst;

public:
    void SetTabRect(IPVM::Rect tabRect)
    {
        m_tabRect = tabRect;
    }
    //1.5세대
    void PushSyncCellID(long sensorId);
    long FrontSyncCellID();
    int PopfrontSyncCellID();
    void ClearSyncCellID();
    long SizeSyncCellID();
    long LastestGradationValue;
    long GradationAndCellID_Offset;
    BOOL CheckGradation(long Value);

    CALLBACK_ON_CELL_IMAGE_PRODUCED m_funcCallbackOnCellImageProduced_Cutting;
    void* m_dataCallbackOnCellImageProduced_Cutting;
    //!1.5세대

protected:
    virtual bool OnTrigger() override;

private:
    bool AlignInlineImage(bool isUp, const std::deque<InlineImageInfo>& inlineImageQueue,
        std::deque<int64_t>& sensorPositionGroup, std::deque<long>& sensorIDGroup);
    void ConvertInlinePosition(const std::deque<InlineImageInfo>& inlineImageQueue, int64_t positionAcc, int& imageID,
        int64_t& position); //이미지 커팅을 위한 값 계산하는 함수
    void CutInlineImage(const std::deque<InlineImageInfo>& inlineImageQueue, bool isUp, long cellID, int imageID1,
        int64_t position1, int cutSize);
    void CutInlineImage_Area(const std::deque<InlineImageInfo>& inlineImageQueue, bool isUp, long cellID, int imageID1,
        long nToTalCam, int cutSize);
    void SumInlineImage_insnex(const std::deque<InlineImageInfo>& inlineImageQueue, int frameCount);

private:
    CellImageMerger& m_cellImageMerger;
    //CELL_DESIGN &m_cellDesign;
    const bool m_upSide;

    CCriticalSection m_sensorGroupLock;
    std::deque<int64_t> m_sensorPositionGroup;
    std::deque<long> m_sensorIDGroup;
    long m_LastestSensorID;
    int64_t m_LastestSensorPosition;

    long m_nCarryCellId;
    long m_nCountLastExcessCellId;
    long m_prevCellId;

    CALLBACK_ON_CELL_IMAGE_PRODUCED m_funcCallbackOnCellImageProduced;
    void* m_dataCallbackOnCellImageProduced;

    IPVM::LoggerInterface& m_loggerTime;
    IPVM::LoggerInterface& m_loggerSensorPosition;
    IPVM::LoggerInterface& m_loggerSequence;
    //1.5세대
    CCriticalSection m_SyncCellIDGroupLock;
    CCriticalSection m_GradationCountLock;
    std::deque<long> m_CellIDGroup;
    //!1.5세대
    int testcellid;
};
