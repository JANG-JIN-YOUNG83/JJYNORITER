#include "pch.h"

#include "CellImageMerger.h"
#include "CellImageProducer.h"
#include "InspectionInputData.h"
#include "RecyclableImage.h"
#include "SystemParameter.h"
#include "Types.h"
#include "Utility.h"

//#include "../Grab/GrabExecutor.h"
//#include "../Inspection/basedef.h"
#include "../SyncPci/Syncboard.h"
//#include "../SharedModule/VisionHostShare/HostCommSocket.h"

#include "Gadget/TimeCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CellImageProducer::CellImageProducer(CellImageMerger& cellImageMerger, IPVM::LoggerInterface& loggerTime,
    IPVM::LoggerInterface& loggerSensorPosition, IPVM::LoggerInterface& loggerSequence, bool upSide)
    : m_cellImageMerger(cellImageMerger)
    , m_upSide(upSide)
    , m_funcCallbackOnCellImageProduced(nullptr)
    , m_dataCallbackOnCellImageProduced(nullptr)
    , m_funcCallbackOnCellImageProduced_Cutting(nullptr)
    , m_dataCallbackOnCellImageProduced_Cutting(nullptr)
    , m_loggerTime(loggerTime)
    , m_loggerSensorPosition(loggerSensorPosition)
    , m_loggerSequence(loggerSequence)
    , m_LastestSensorID(-1)
    , m_LastestSensorPosition(-1)
    , LastestGradationValue(-1)
    , GradationAndCellID_Offset(-1)
    , m_bIsFirst(true)
    , m_nCarryCellId(0)
    , m_nCountLastExcessCellId(0)
    , m_prevCellId(LONG_MAX)
{
    testcellid = 0;
    __super::BeginWorker();
}

CellImageProducer::~CellImageProducer()
{
    __super::EndWorker();
}

void CellImageProducer::RegisterCallbackOnCellImageProduced(
    CALLBACK_ON_CELL_IMAGE_PRODUCED callbackFunc, void* callbackData)
{
    //if (g_systemParameter.enRunMode == RUN_MODE::CUTTING_SIMULATION)
    //{
    //	m_funcCallbackOnCellImageProduced_Cutting = callbackFunc;
    //	m_dataCallbackOnCellImageProduced_Cutting = callbackData;
    //}
    //else
    {
        m_funcCallbackOnCellImageProduced = callbackFunc;
        m_dataCallbackOnCellImageProduced = callbackData;
    }
}

void CellImageProducer::PushSensorInfo(int64_t sensorPosition, long sensorId)
{
    CSingleLock lock(&m_sensorGroupLock, TRUE);

    m_sensorPositionGroup.push_back(sensorPosition);
    if (g_systemParameter.machineType != MachineType::ES_mismatch)
        m_LastestSensorPosition = sensorPosition;
    m_sensorIDGroup.push_back(sensorId);
    m_LastestSensorID = sensorId;
}

void CellImageProducer::SetLatestSensorPosition(int64_t value)
{
    CSingleLock lock(&m_sensorGroupLock, TRUE);
    m_LastestSensorPosition = value;
}

int64_t CellImageProducer::GetLatestSensorPosition()
{
    CSingleLock lock(&m_sensorGroupLock, TRUE);
    return m_LastestSensorPosition;
    /*if (m_sensorPositionGroup.size() == 0)
	{
		return -1;
	}

	return m_sensorPositionGroup.back();*/
}

long CellImageProducer::GetLatestSensorID()
{
    CSingleLock lock(&m_sensorGroupLock, TRUE);

    return m_LastestSensorID;
    /*if (m_sensorIDGroup.size() == 0)
	{
		return -1;
	}

	return m_sensorIDGroup.back();*/
}

void CellImageProducer::ClearLot()
{
    m_nCarryCellId = 0;
    m_nCountLastExcessCellId = 0;

    if (this == nullptr)
    {
        return;
    }
    //if (Share::GenerationVersion::SplitterRemoved == g_systemParameter.nGenerationVersion)
    {
        ClearSyncCellID();
        LastestGradationValue = -1;
        GradationAndCellID_Offset = -1;
    }

    {
        CSingleLock lock(&m_sensorGroupLock, TRUE);

        m_sensorPositionGroup.clear();
        m_LastestSensorPosition = -1;
        m_sensorIDGroup.clear();
        m_LastestSensorID = -1;
    }
    CString strSide = 0 == m_upSide == true ? _T("Up") : _T("Down");

    //m_loggerSensorPosition.Log(0, _T("%s Camera Sensor Position, Sensor ID Group Clear"), strSide);
}

bool CellImageProducer::OnTrigger()
{
    std::deque<int64_t> sensorPositionGroup;
    std::deque<long> sensorIDGroup;
    {
        CSingleLock lock(&m_sensorGroupLock, TRUE);
        sensorPositionGroup = m_sensorPositionGroup;
        sensorIDGroup = m_sensorIDGroup;
    }

    // 처음 체크
    int64_t headFrameIndex = 0;
    int64_t tailFrameIndex = 0;

    m_cellImageMerger.GetUpdatedFrameIndices(headFrameIndex, tailFrameIndex);

    if (headFrameIndex >= tailFrameIndex)
    {
        return true;
    }
    else if (m_cellImageMerger.m_grabExecutor[0]->GetGrabMode() == GrabMode::AREA)
    {
        std::deque<InlineImageInfo> inlineImageQueue;

        long sizeMin = min(sensorIDGroup.size(), tailFrameIndex);
        for (int64_t frameIndex = headFrameIndex; frameIndex < tailFrameIndex; frameIndex++)
        {
            IPVM::Image_8u_C1 image(m_cellImageMerger.GetHorizontalSize(), m_cellImageMerger.GetHorizontalSize(),
                m_cellImageMerger.GetVerticalSize(), m_cellImageMerger.GetFrameAddress(frameIndex));

            InlineImageInfo inlineImageInfo;
            inlineImageInfo.iImage = image;
            inlineImageInfo.imagePosition = frameIndex * m_cellImageMerger.GetVerticalSize();
            if (g_systemParameter.machineType == MachineType::ES_mismatch || g_systemParameter.machineType == MachineType::ES_rollpress_surface)
                inlineImageInfo.imagePosition /= 2; // sapera는 /2
            inlineImageInfo.m_grabTime_ms = m_cellImageMerger.GetFrameGrabbedTime_ms(frameIndex);
            ::GetLocalTime(&inlineImageInfo.m_inputTime);

            inlineImageQueue.push_back(inlineImageInfo);
        }

        //AlignInlineImage(m_upSide, inlineImageQueue, sensorPositionGroup, sensorIDGroup);
        long countMin = min(inlineImageQueue.size(), sensorIDGroup.size());
        for (long cnt = 0; cnt < countMin; cnt++) // no Cutting
        {
            long cellId = sensorIDGroup.front();
            sensorIDGroup.pop_front();
            //TRACE1("CutInlineImage CellId : %d\n", cellId);
            //m_loggerSensorPosition.Log(0, _T("CutInlineImage CellId : %d"), cellId);

            if (m_prevCellId == cellId)
            {
                TRACE(_T("XXXXXXXXX**Cell_ID**XXXXXXXXXXXX\n"));
                m_loggerSensorPosition.Log(0, _T("XXXXXXXXX**Cell_ID**XXXXXXXXXXXX"));
            }
            m_prevCellId = cellId;

            std::deque<InlineImageInfo> inlineImageQueueDis;
            inlineImageQueueDis.push_back(inlineImageQueue[cnt]);

            //CutInlineImage(inlineImageQueueDis, true, cellId, 0, 0, inlineImageQueue[cnt].iImage.GetSizeY());

            long nToTalCam = 0;
            nToTalCam = m_cellImageMerger.GetHorizontalSize() / m_cellImageMerger.m_grabExecutor[0]->GetHorizontalSize();
            CutInlineImage_Area(inlineImageQueueDis, true, cellId, 0, nToTalCam, m_cellImageMerger.m_grabExecutor[0]->GetHorizontalSize());
        }
        // 사용한 inline image 제거
        if (countMin > 0)
        {
            TRACE1("countMin %d\n", countMin);
            m_cellImageMerger.ProceedHeadFrameIndex(countMin);

            // Delete SensorIDGroub
            {
                CSingleLock lock(&m_sensorGroupLock, TRUE);
                m_sensorPositionGroup.erase(m_sensorPositionGroup.begin(), m_sensorPositionGroup.begin() + countMin);
                m_sensorIDGroup.erase(m_sensorIDGroup.begin(), m_sensorIDGroup.begin() + countMin);
            }
        }
        return TRUE;
    }

    std::deque<InlineImageInfo> inlineImageQueue;

    long sizeMin = min(sensorIDGroup.size(), tailFrameIndex);
    for (int64_t frameIndex = headFrameIndex; frameIndex < tailFrameIndex; frameIndex++)
    {
        IPVM::Image_8u_C1 image(m_cellImageMerger.GetHorizontalSize(), m_cellImageMerger.GetHorizontalSize(),
            m_cellImageMerger.GetVerticalSize(), m_cellImageMerger.GetFrameAddress(frameIndex));

        
        IPVM::Image_8u_C1 Secimage(m_cellImageMerger.GetHorizontalSize(), m_cellImageMerger.GetHorizontalSize(),
            m_cellImageMerger.GetVerticalSize(), m_cellImageMerger.GetSecFrameAddress(frameIndex));

        InlineImageInfo inlineImageInfo;
        inlineImageInfo.iImage = image;
        inlineImageInfo.iImage_sec = Secimage;
        
        //inlineImageInfo.imagePosition = frameIndex * m_cellImageMerger.GetVerticalSize();
        inlineImageInfo.imagePosition = frameIndex;
        inlineImageInfo.m_grabTime_ms = m_cellImageMerger.GetFrameGrabbedTime_ms(frameIndex);
        ::GetLocalTime(&inlineImageInfo.m_inputTime);
       
        inlineImageQueue.push_back(inlineImageInfo);
        
         
    }
    long lastPosition = 0;
    if (!inlineImageQueue.empty())
    {
        lastPosition = inlineImageQueue.back().imagePosition;
        SumInlineImage_insnex(inlineImageQueue, lastPosition);//jjy
    }
     
    //AlignInlineImage(m_upSide, inlineImageQueue, sensorPositionGroup, sensorIDGroup);
    long countMin = min(inlineImageQueue.size(), sensorIDGroup.size());
    //long countMin = inlineImageQueue.size();
    for (long cnt = 0; cnt < countMin; cnt++) // no Cutting
    {
        long cellId = sensorIDGroup.front();
        sensorIDGroup.pop_front();
        //TRACE1("CutInlineImage CellId : %d\n", cellId);
        //m_loggerSensorPosition.Log(0, _T("CutInlineImage CellId : %d"), cellId);

        if (m_prevCellId == cellId)
        {
            TRACE(_T("XXXXXXXXX**Cell_ID**XXXXXXXXXXXX\n"));
            m_loggerSensorPosition.Log(0, _T("XXXXXXXXX**Cell_ID**XXXXXXXXXXXX"));
        }
        m_prevCellId = cellId;

        std::deque<InlineImageInfo> inlineImageQueueDis;
        inlineImageQueueDis.push_back(inlineImageQueue[cnt]);

        //CutInlineImage(inlineImageQueueDis, true, cellId, 0, 0, inlineImageQueue[cnt].iImage.GetSizeY());
    }
    // 사용한 inline image 제거
    if (countMin > 0)
    {
        //TRACE1("countMin %d\n", countMin);
        m_cellImageMerger.ProceedHeadFrameIndex(countMin);

        // Delete SensorIDGroub
        {
            CSingleLock lock(&m_sensorGroupLock, TRUE);
            m_sensorPositionGroup.erase(m_sensorPositionGroup.begin(), m_sensorPositionGroup.begin() + countMin);
            m_sensorIDGroup.erase(m_sensorIDGroup.begin(), m_sensorIDGroup.begin() + countMin);
        }
    }


    return true;
}

bool CellImageProducer::AlignInlineImage(bool isUp, const std::deque<InlineImageInfo>& inlineImageQueue,
    std::deque<int64_t>& sensorPositionGroup, std::deque<long>& sensorIDGroup)
{
    int margin = g_systemParameter.nCuttingMargin;

    // mm to pixel
    int pitchPixel = g_systemParameter.fCuttingPitch * 1000 / g_systemParameter.m_dPixelToUmY;
    pitchPixel = (pitchPixel / 2) * 2; // pixelPixel 값을 짝수로 만듬
    int cutSize = pitchPixel + 2 * margin;

    int imageID1MinusLast = -1;

    int sensorID_removed = 0;

    float sensorDistOffset = 0; // 물리 거리 : CIS위치 - 센서위치 (센서 먼저)
    float sensorDelayOffset = 0; // 설비별 offset
    float sensorMoveOffset = 0; // 기준점 변경
    int sensorTotalOffset = sensorDistOffset + sensorDelayOffset + sensorMoveOffset;

    int prevCellID = -1;

    bool bFirstPass = false;
    int nSensorDiff = 0;

    if (sensorPositionGroup.size() > 2)
    {
        if (m_bIsFirst)
        {
            if ((pitchPixel * 0.8) > sensorPositionGroup[1] - sensorPositionGroup[0])
            {
                bFirstPass = true;
                nSensorDiff = sensorPositionGroup[1] - sensorPositionGroup[0];
            }
        }
    }

    long nCountDeleteCellId = 0;
    int count = (inlineImageQueue.size() - 1) * m_cellImageMerger.GetVerticalSize() / 2 / pitchPixel;
    for (int i = 0; i < count; i++)
    {
        int64_t sensorOffsetedAcc1 = 0;
        int64_t sensorOffsetedAcc2 = 0;

        if (i == 0 && bFirstPass)
            continue;

        sensorOffsetedAcc1 = GetLatestSensorPosition()
            + sensorTotalOffset; //Sensor Position과 시스템의 V Home Offset + 탭 사이드 스펙 + 탭 센서 딜레이 오프셋 값과의 차이
        sensorOffsetedAcc2 = sensorOffsetedAcc1 + pitchPixel; //여기에서 sensorOffsetedAcc2 값을 바꿈

        int64_t cutAcc1Minus = sensorOffsetedAcc1
            - margin; //Sensor Position과 시스템의 V Home Offset + 탭 사이드 스펙 + 탭 센서 딜레이 오프셋 값과의 차이 값 - 시스템의 셀 커팅 마진 값
        int64_t cutAcc2Plus = sensorOffsetedAcc2
            + margin; //대략적인 다음 sensor 위치 값에서 시스템의 셀 커팅 마진 값만큼 더하기(커팅 종료 위치인듯)
        if (cutAcc2Plus
            >= inlineImageQueue[inlineImageQueue.size() - 1].imagePosition + m_cellImageMerger.GetVerticalSize())
        {
            m_loggerSequence.Log(0, _T("[CellImageProducer Position WARNING]"));
            break;
        }

        int imageID1Minus, imageIDAcc1;
        int64_t cut1Minus, cutAcc1;
        ConvertInlinePosition(inlineImageQueue, cutAcc1Minus, imageID1Minus,
            cut1Minus); //실제 커팅 위치 계산, 여기서 계산한 Image ID 값으로 사용한 이미지를 삭제함
        ConvertInlinePosition(inlineImageQueue, sensorOffsetedAcc1, imageIDAcc1, cutAcc1); // for CellId

        // Cut Start 위치가 margin 범위 Minus 인 경우
        //if (imageID1 == 0 && cut1 < margin)		// 추가
        //{
        //	sensorID_removed++;
        //	continue;
        //}

        // Invalid 처리 (커팅 불가)
        if (imageID1Minus == -1)
        {
            sensorID_removed = i + 1;
            break;
        }

        // Cell ID
        int cellId = -1;
        {
            long dst_line_cur = 0;
            long dst_line_remain = pitchPixel;
            int64_t src_line_cur = cutAcc1;

            double dSumCur = 0.;
            long nSumCur = 0;
            m_nCarryCellId;
            m_nCountLastExcessCellId;

            long sizeY = m_cellImageMerger.GetVerticalSize();

            long nCountLine = 0;
            for (long queue = imageIDAcc1; queue < inlineImageQueue.size(); queue++)
            {
                if (sensorIDGroup.size() <= queue)
                    goto EXIT_CELLID;

                nCountDeleteCellId++;
                long src_line_remain = inlineImageQueue[queue].iImage.GetSizeY() - src_line_cur;

                // src remain copy
                if (dst_line_remain > src_line_remain)
                {
                    if (src_line_cur == 0)
                    {
                        nSumCur += sensorIDGroup[queue] * sizeY;
                        nCountLine += sizeY;
                    }
                    else
                    {
                        dSumCur += sensorIDGroup[queue] * src_line_remain;
                        nCountLine += src_line_remain;
                    }

                    dst_line_cur += src_line_remain;
                    dst_line_remain -= src_line_remain;
                    src_line_cur = 0;
                }
                // dst remain copy
                else
                {
                    nCountDeleteCellId--;
                    if (dst_line_remain == 0)
                    {
                    }
                    else
                    {
                        dSumCur += sensorIDGroup[queue] * dst_line_remain;
                        nCountLine += dst_line_remain;
                    }
                    dst_line_remain = 0;
                    break;
                }
            }
            dSumCur += nSumCur;
            double dAverageCur = dSumCur / nCountLine;
            double dAverageTotal = (dSumCur - m_nCountLastExcessCellId) / nCountLine;

            cellId = dAverageTotal - m_nCarryCellId;

            m_nCountLastExcessCellId = (dAverageCur - cellId) * nCountLine;
            //if (dAverageCur - cellId > 1.)
            //	m_nCarryCellId++;
        }
EXIT_CELLID:

        if (prevCellID == cellId)
        {
            TRACE(_T("xxx\n"));
        }
        prevCellID = cellId;

        IPVM::TimeCheck timeCheck;
        timeCheck.Reset();

        if (bFirstPass)
            m_loggerSensorPosition.Log(0,
                _T("********%s First Cutting Sensor Position Difference Exception [Difference-%d]********"),
                m_upSide ? _T("UP") : _T("DOWN"), nSensorDiff);
        CString strIsUp = (isUp == true ? _T("UP") : _T("DOWN"));
        m_loggerSequence.Log(
            0, _T("[Cell Cutting] %6s - CellID(%03d) SensorDiff(%d)"), (LPCTSTR)strIsUp, cellId, nSensorDiff);
        if (0 > inlineImageQueue.size() || 0 > imageID1Minus)
            break;
        CutInlineImage(inlineImageQueue, isUp, cellId, imageID1Minus, cut1Minus, cutSize);
        // cell ID 가 0 ~ 127 이 되도록

        CString strUp = _T("Down");
        if (m_upSide)
        {
            strUp = _T("Up");
        }

        //{{ calculation time
        float time = timeCheck.Elapsed_ms();
        m_loggerTime.Log(0, _T("[Cutting - %s(%d)]%.2f ms"), (LPCTSTR)strUp, cellId, time);
        //}} calculation time

        imageID1MinusLast = imageID1Minus;

        sensorID_removed = i + 1;
        CString strSide = m_upSide == true ? _T("Up") : _T("Down");
        if (i == 1)
        {
            m_loggerSensorPosition.Log(0, _T("[ERROR - %s]ACC1: %d,Position[%d]:%d, offset:%d, "), strSide,
                sensorOffsetedAcc1, i, sensorPositionGroup[i], sensorTotalOffset);
        }
        SetLatestSensorPosition(GetLatestSensorPosition() + pitchPixel);
    }

    // 사용한 inline image 제거
    if (imageID1MinusLast > 1)
    {
        m_cellImageMerger.ProceedHeadFrameIndex(imageID1MinusLast - 1);

        // Delete SensorIDGroub
        {
            CSingleLock lock(&m_sensorGroupLock, TRUE);
            m_sensorPositionGroup.erase(
                m_sensorPositionGroup.begin(), m_sensorPositionGroup.begin() + nCountDeleteCellId);
            m_sensorIDGroup.erase(m_sensorIDGroup.begin(), m_sensorIDGroup.begin() + nCountDeleteCellId);
        }
    }

    //{
    //	CSingleLock lock(&m_sensorGroupLock, TRUE);

    //	CString strSide = m_upSide == true ? _T("Up") : _T("Down");

    //	for (int k = 0; k < sensorID_removed; k++)
    //	{
    //		int nPosition = m_sensorPositionGroup.front();
    //		int nCellID = m_sensorIDGroup.front();

    //		m_sensorPositionGroup.pop_front();
    //		m_sensorIDGroup.pop_front();

    //		m_loggerSensorPosition.Log(0, _T("[%s]lnline함수에서 제거 SensorPosition :%d Size : %d, CellID :%d Size : %d, "), strSide, nPosition,m_sensorPositionGroup.size(), nCellID,m_sensorIDGroup.size());
    //	}
    //}

    return true;
}

void CellImageProducer::ConvertInlinePosition(
    const std::deque<InlineImageInfo>& inlineImageQueue, int64_t positionAcc, int& imageID, int64_t& position)
{
    imageID = -1;
    position = -1;

    if (inlineImageQueue.size() == 0)
    {
        return;
    }

    if (positionAcc < 0)
    {
        imageID = 0;
        position = positionAcc;
        return;
    }
    else if (positionAcc < inlineImageQueue[0].imagePosition)
    {
        imageID = -1;
        position = -1;
    }
    else if (positionAcc >= inlineImageQueue[inlineImageQueue.size() - 1].imagePosition)
    {
        imageID = -2;
        position = -2;
    }
    else
    {
        for (int i = 0; i < inlineImageQueue.size(); i++)
        {
            if (positionAcc >= inlineImageQueue[i].imagePosition
                && positionAcc < inlineImageQueue[i].imagePosition
                        + inlineImageQueue[i]
                              .iImage
                              .GetSizeY()) //잔여 이미지의 누적 라인이 한 피치 이상인지 체크하는거 같음, && 뒤의 조건문으로 한 피치 초과되는 뒤의 이미지들을 제한하는거 같음
            {
                imageID = i; //조건에 해당한 inilineImage 큐의 인덱스 번호를 image ID로 넘겨줌
                position = positionAcc
                    - inlineImageQueue[i]
                          .imagePosition; //센서 위치 값에서 imagePosition 값을 빼면 정확히 어디부터 커팅하면 될지 Y 좌표 값이 나옴
                return;
            }
        }
    }
}

void CellImageProducer::CutInlineImage(const std::deque<InlineImageInfo>& inlineImageQueue, bool isUp, long cellID,
    int imageID1, int64_t position1, int cutSize)
{
    TRACE1("CutInlineImage CellId : %d\n", cellID);

    // Cut Image
    int imageWidth = inlineImageQueue[0].iImage.GetSizeX();
    int imageHeightDst = cutSize;

    IPVM::Image_8u_C1 iImageDst;
    bool bAlloc = RecyclableImage::Alloc_Full(iImageDst, imageWidth, imageHeightDst);

    m_loggerSensorPosition.Log(0, _T("[CutInlineImage] %s Cell ID : %d Image Size X : %d Y : %d Alloc : %s"),
        isUp ? _T("UP") : _T("DOWN"), cellID, imageWidth, imageHeightDst, bAlloc ? _T("Success") : _T("Fail"));

    if (m_bIsFirst)
    {
        m_bIsFirst = false;
    }

    if (g_systemParameter.machineType == MachineType::ES_mismatch
        || g_systemParameter.machineType == MachineType::ES_rollpress_pinhole
        || g_systemParameter.machineType == MachineType::ES_rollpress_surface)
    {
        BYTE* image_dst1 = iImageDst.GetMem();
        BYTE* image_dst2 = iImageDst.GetMem(0, imageHeightDst / 2);

        long dst_line_cur = 0;
        long dst_line_remain = imageHeightDst / 2;
        int64_t src_line_cur;
        if (position1 < 0)
        {
            const BYTE* image_src1 = inlineImageQueue[imageID1].iImage.GetMem();
            const BYTE* image_src2
                = inlineImageQueue[imageID1].iImage.GetMem(0, inlineImageQueue[imageID1].iImage.GetSizeY() / 2);

            for (int i = 0; i < abs(position1); i++)
            {
                // 첫 line 을 dummy size 만큼 copy
                memcpy(image_dst1 + (dst_line_cur * imageWidth), image_src1, imageWidth);
                memcpy(image_dst2 + (dst_line_cur * imageWidth), image_src2, imageWidth);
                dst_line_cur++;
                dst_line_remain--;
            }
            src_line_cur = 0;
        }
        else
        {
            src_line_cur = position1 / 2;
        }

        for (long queue = imageID1; queue < inlineImageQueue.size(); queue++)
        {
            const BYTE* image_src1 = inlineImageQueue[queue].iImage.GetMem();
            const BYTE* image_src2
                = inlineImageQueue[queue].iImage.GetMem(0, inlineImageQueue[queue].iImage.GetSizeY() / 2);

            long src_line_remain = inlineImageQueue[queue].iImage.GetSizeY() / 2 - src_line_cur;

            // src remain copy
            if (dst_line_remain > src_line_remain)
            {
                memcpy(image_dst1 + (dst_line_cur * imageWidth), image_src1 + (src_line_cur * imageWidth),
                    src_line_remain * imageWidth);
                memcpy(image_dst2 + (dst_line_cur * imageWidth), image_src2 + (src_line_cur * imageWidth),
                    src_line_remain * imageWidth);
                dst_line_cur += src_line_remain;
                dst_line_remain -= src_line_remain;

                src_line_cur = 0;
            }
            // dst remain copy
            else
            {
                memcpy(image_dst1 + (dst_line_cur * imageWidth), image_src1 + (src_line_cur * imageWidth),
                    dst_line_remain * imageWidth);
                memcpy(image_dst2 + (dst_line_cur * imageWidth), image_src2 + (src_line_cur * imageWidth),
                    dst_line_remain * imageWidth);
                dst_line_remain = 0;
            }

            if (dst_line_remain == 0)
                break;
        }
    }
    else
    {
        BYTE* image_dst = iImageDst.GetMem();

        // IBNI Code - 기존 코드
        long dst_ptr_cur = 0;
        long dst_line_cur = 0;
        long dst_line_remain = imageHeightDst;
        int64_t scr_line_cur;
        if (position1 < 0)
        {
            auto* image_src = inlineImageQueue[imageID1].iImage.GetMem();
            for (int i = 0; i < abs(position1); i++)
            {
                // 첫 line 을 dummy size 만큼 copy
                memcpy(image_dst + dst_ptr_cur, image_src, imageWidth);
                dst_ptr_cur += imageWidth;
                dst_line_cur++;
                dst_line_remain = imageHeightDst - dst_line_cur;
            }
            scr_line_cur = 0;
        }
        else
        {
            scr_line_cur = position1;
        }

        for (long i = imageID1; i < inlineImageQueue.size(); i++)
        {
            auto* image_src = inlineImageQueue[i].iImage.GetMem();

            long src_line_remain = inlineImageQueue[i].iImage.GetSizeY() - scr_line_cur;

            // image copy
            if (dst_line_remain > src_line_remain)
            {
                memcpy(image_dst + dst_ptr_cur, image_src + (scr_line_cur * imageWidth), src_line_remain * imageWidth);
                dst_ptr_cur += (src_line_remain * imageWidth);
                dst_line_cur += src_line_remain;
                dst_line_remain = imageHeightDst - dst_line_cur;

                scr_line_cur = 0;
            }
            // line copy
            else
            {
                memcpy(image_dst + dst_ptr_cur, image_src + (scr_line_cur * imageWidth), dst_line_remain * imageWidth);
                dst_line_remain = 0;
            }

            if (dst_line_remain == 0)
                break;
        }

        // IBCI Code - mk 수정 본
        //long dst_line_cur = 0;
        //long dst_line_remain = imageHeightDst;
        //int64_t src_line_cur;
        //if (position1 < 0)
        //{
        //    auto* image_src = inlineImageQueue[imageID1].iImage.GetMem();
        //    for (int i = 0; i < abs(position1); i++)
        //    {
        //        // 첫 line 을 dummy size 만큼 copy
        //        memcpy(image_dst + (dst_line_cur * imageWidth), image_src, imageWidth);
        //        dst_line_cur++;
        //        dst_line_remain--;
        //    }
        //    src_line_cur = 0;
        //}
        //else
        //{
        //    src_line_cur = position1;
        //}

        //for (long queue = imageID1; queue < inlineImageQueue.size(); queue++)
        //{
        //    const BYTE* image_src = inlineImageQueue[queue].iImage.GetMem();

        //    long src_line_remain = inlineImageQueue[queue].iImage.GetSizeY() - src_line_cur;

        //    // src remain copy
        //    if (dst_line_remain > src_line_remain)
        //    {
        //        memcpy(image_dst + (dst_line_cur * imageWidth), image_src + (src_line_cur * imageWidth),
        //            src_line_remain * imageWidth);
        //        dst_line_cur += src_line_remain;
        //        dst_line_remain -= src_line_remain;
        //        
        //        src_line_cur = 0;
        //    }
        //    // dst remain copy
        //    else
        //    {
        //        memcpy(image_dst + (dst_line_cur * imageWidth), image_src + (src_line_cur * imageWidth),
        //            dst_line_remain * imageWidth);
        //        dst_line_remain = 0;
        //    }

        //    if (dst_line_remain == 0)
        //        break;
        //}
    }

    //{{ Inspection Input Data Set 추가
    InspectionInputData inputData;
    inputData.m_nSide = 0;
    inputData.m_isUp = isUp;
    inputData.m_imageFull = iImageDst;

    //TODO: 이 두 이미지는 포인터만 가지고 있어도 되므로, inputData.imageFull 의 child 이미지를 사용하는 것으로 수정할 수 있는지 확인할 것
    IPVM::Image_8u_C1 iImageBright, iImageDark;

    if (g_systemParameter.machineType == MachineType::ES_mismatch
        || g_systemParameter.machineType == MachineType::ES_rollpress_pinhole
        || g_systemParameter.machineType == MachineType::ES_rollpress_surface)
    {
        if (!UtilityImage::CreateBrightDarkImage_sapera(iImageDst, iImageBright, iImageDark, true, &m_loggerTime))
        {
            //m_pClientSocket.SendAlarm_CIS(165 , Share::ALARM_LEVEL::red, _T("IMAGE SPLIT ERROR"), _T("IMAGE SPLIT ERROR"), _T("	"), _T(""));
            m_loggerTime.Log(0, _T("Create Bright Dark Image : Failed (CellID:%d)"), cellID);
        }
    }
    else
    {
        if (!UtilityImage::CreateBrightDarkImage(iImageDst, iImageBright, iImageDark, true, &m_loggerTime))
        {
            //m_pClientSocket.SendAlarm_CIS(165 , Share::ALARM_LEVEL::red, _T("IMAGE SPLIT ERROR"), _T("IMAGE SPLIT ERROR"), _T("	"), _T(""));
            m_loggerTime.Log(0, _T("Create Bright Dark Image : Failed (CellID:%d)"), cellID);
        }
    }

    inputData.m_imageBright = iImageBright;
    inputData.m_imageDark = iImageDark;
    inputData.m_cellID = cellID;
    inputData.m_inspectioinStartTime_ms = IPVM::TimeCheck::Current_ms();
    inputData.m_grabTime_ms = inlineImageQueue[imageID1].m_grabTime_ms;
    inputData.m_inputTime = inlineImageQueue[imageID1].m_inputTime;
    inputData.m_imagePos = inlineImageQueue[imageID1].imagePosition + position1;

    if (m_funcCallbackOnCellImageProduced && m_dataCallbackOnCellImageProduced)
    {
        m_funcCallbackOnCellImageProduced(m_dataCallbackOnCellImageProduced, inputData);
    }
}
void CellImageProducer::SumInlineImage_insnex(const std::deque<InlineImageInfo>& inlineImageQueue, int frameCount)
{
    
    int frameIndex = frameCount % 51; 
    
    int imageWidth = inlineImageQueue[0].iImage.GetSizeX();
    int imageHeight = inlineImageQueue[0].iImage.GetSizeY();
       
    int imageHeightDst = imageHeight * 51; // 10200
       
    auto TopDst = m_cellImageMerger.GetTotalFrameAddress(0);
    auto bottomDst = m_cellImageMerger.GetTotalSecFrameAddress(0);
     
    IPVM::Image_8u_C1 imageTopSrc(imageWidth, imageWidth, imageHeightDst, TopDst);
    IPVM::Image_8u_C1 imageBottomSrc(imageWidth, imageWidth, imageHeightDst, bottomDst);
    IPVM::Image_8u_C1 iImageDst(imageWidth, imageHeightDst);
  
    BYTE* image_TotalTopdst = imageTopSrc.GetMem();
    BYTE* image_TotalBottomdst = imageBottomSrc.GetMem();
     
  
    // 원본 이미지 포인터
    auto* image_src = inlineImageQueue[0].iImage.GetMem();
    auto* image_src_sec = inlineImageQueue[0].iImage_sec.GetMem();
    
    int copySize = imageWidth * imageHeight; 
    int offset = frameIndex * copySize;

    BYTE* dst_offset = image_TotalTopdst + offset;
    BYTE* dst_offset_sec = image_TotalBottomdst + offset;
      
    memcpy(dst_offset, image_src, copySize);
    memcpy(dst_offset_sec, image_src_sec, copySize);
     
    
    if (frameCount != 0 && frameCount % 50 == 0)
    {
           
        IPVM::Image_8u_C1 iImageGradient, iImageABS;

        InspectionInputData inputData;
        inputData.m_nSide = 0;
        inputData.m_isUp = 0;
        inputData.m_imageFull = iImageDst;

        if (!UtilityImage::CreateGradientImage_insnex(
                imageTopSrc, imageBottomSrc, iImageGradient, iImageABS, true, &m_loggerTime))
        {
           // 예외 처리
            TRACE(_T(" [CreateGradientImage_insnex Fail] \n"));
        }

        inputData.m_imageBright = iImageGradient;
        inputData.m_imageDark = iImageABS;
        inputData.m_cellID = 0;
        inputData.m_inspectioinStartTime_ms = IPVM::TimeCheck::Current_ms();
        inputData.m_grabTime_ms = 0.f;
        inputData.m_inputTime = inlineImageQueue[0].m_inputTime;
        inputData.m_imagePos = 0;
    
         if (m_funcCallbackOnCellImageProduced && m_dataCallbackOnCellImageProduced)
        {
            m_funcCallbackOnCellImageProduced(m_dataCallbackOnCellImageProduced, inputData);
        }
   
    
    }
   
}
void CellImageProducer::CutInlineImage_Area(const std::deque<InlineImageInfo>& inlineImageQueue, bool isUp, long cellID,
    int imageID1, long nToTalCam, int cutSize)
{
    TRACE1("CutInlineImage CellId : %d\n", cellID);

    // Area 영상 특성상, 4개 카메라가 모두 동일한 사이즈로 영상 획득
    int imageWidth = inlineImageQueue[0].iImage.GetSizeX();;
    
    int imageHeightDst = inlineImageQueue[0].iImage.GetSizeY();

    int imageCutSize = cutSize;

    IPVM::Image_8u_C1 iImageDst;
    bool bAlloc = RecyclableImage::Alloc_Full(iImageDst, imageWidth, imageHeightDst);
    m_loggerSensorPosition.Log(0, _T("[CutInlineImage] %s Cell ID : %d Image Size X : %d Y : %d Alloc : %s"),
        isUp ? _T("UP") : _T("DOWN"), cellID, imageWidth, imageHeightDst, bAlloc ? _T("Success") : _T("Fail"));

    if (m_bIsFirst)
    {
        m_bIsFirst = false;
    }

    BYTE* image_dst = iImageDst.GetMem();

    for (long queue = imageID1; queue < inlineImageQueue.size(); queue++)
    {
        const BYTE* image_src = inlineImageQueue[queue].iImage.GetMem();

        IPVM::ImageProcessing::Copy(inlineImageQueue[queue].iImage, IPVM::Rect(inlineImageQueue[queue].iImage), iImageDst);
    }

    for (long nidx = 0; nidx < nToTalCam; nidx++)
    {
        

         //{{ Inspection Input Data Set 추가
         InspectionInputData inputData;
         inputData.m_nSide = nidx;
         inputData.m_isUp = isUp;

         //imageFull 영상을 쪼개서 각각 버퍼에 저장

         IPVM::Rect Rect_Split(0, 0, 0, 0);
         IPVM::Image_8u_C1 imgSplit(imageCutSize, imageHeightDst);
         IPVM::Image_8u_C1 iImageBright, iImageDark;

         if (nidx == 0)
         {
             Rect_Split = IPVM::Rect(imgSplit);
         }
         else
         {
             Rect_Split = IPVM::Rect(imgSplit);
             Rect_Split.m_left = IPVM::Rect(imgSplit).Width() * nidx;
             Rect_Split.m_right = Rect_Split.m_right + IPVM::Rect(imgSplit).Width() * nidx;
         }
         IPVM::ImageProcessing::Copy(iImageDst, Rect_Split, IPVM::Rect(imgSplit), imgSplit);
         inputData.m_imageFull = imgSplit;
         if (g_systemParameter.machineType == MachineType::ES_mismatch
             || g_systemParameter.machineType == MachineType::ES_rollpress_pinhole
             || g_systemParameter.machineType == MachineType::ES_rollpress_surface)
         {
             if (!UtilityImage::CreateBrightDarkImage_sapera(imgSplit, iImageBright, iImageDark, true, &m_loggerTime))
             {
                 //m_pClientSocket.SendAlarm_CIS(165 , Share::ALARM_LEVEL::red, _T("IMAGE SPLIT ERROR"), _T("IMAGE SPLIT ERROR"), _T("	"), _T(""));
                 m_loggerTime.Log(0, _T("Create Bright Dark Image : Failed (CellID:%d)"), cellID);
             }
         }
         else
         {
             if (!UtilityImage::CreateBrightDarkImage(imgSplit, iImageBright, iImageDark, true, &m_loggerTime))
             {
                 //m_pClientSocket.SendAlarm_CIS(165 , Share::ALARM_LEVEL::red, _T("IMAGE SPLIT ERROR"), _T("IMAGE SPLIT ERROR"), _T("	"), _T(""));
                 m_loggerTime.Log(0, _T("Create Bright Dark Image : Failed (CellID:%d)"), cellID);
             }
         }
         inputData.m_imageBright = iImageBright;
         inputData.m_imageDark = iImageDark;
         

         //TODO: 이 두 이미지는 포인터만 가지고 있어도 되므로, inputData.imageFull 의 child 이미지를 사용하는 것으로 수정할 수 있는지 확인할 것

         
         //inputData.m_imageBright = inputData.m_imageArea[nidx].m_imageBright;
         //inputData.m_imageDark = inputData.m_imageArea[nidx].m_imageDark;
         inputData.m_cellID = cellID;
         inputData.m_inspectioinStartTime_ms = IPVM::TimeCheck::Current_ms();
         inputData.m_grabTime_ms = inlineImageQueue[imageID1].m_grabTime_ms;
         inputData.m_inputTime = inlineImageQueue[imageID1].m_inputTime;
         inputData.m_imagePos = inlineImageQueue[imageID1].imagePosition;

         if (m_funcCallbackOnCellImageProduced && m_dataCallbackOnCellImageProduced)
         {
             m_funcCallbackOnCellImageProduced(m_dataCallbackOnCellImageProduced, inputData);
         }
    }
}


void CellImageProducer::PushSyncCellID(long CellID)
{
    CSingleLock lock(&m_SyncCellIDGroupLock, TRUE);
    m_CellIDGroup.push_back(CellID);
}

long CellImageProducer::FrontSyncCellID()
{
    CSingleLock lock(&m_SyncCellIDGroupLock, TRUE);
    long CellID = -1;
    //if (Share::GenerationVersion::SplitterRemoved == g_systemParameter.nGenerationVersion && g_systemParameter.IsExistGrabHardware() == FALSE)
    {
        testcellid++;
        if (64 == testcellid)
            testcellid = 0;
        return testcellid;
    }
    if (m_CellIDGroup.size())
        CellID = m_CellIDGroup.front();
    return CellID;
}
int CellImageProducer::PopfrontSyncCellID()
{
    CSingleLock lock(&m_SyncCellIDGroupLock, TRUE);
    if (m_CellIDGroup.size())
    {
        int cellid = m_CellIDGroup.front();
        m_CellIDGroup.pop_front();
        return cellid;
    }
    return -1;
}

void CellImageProducer::ClearSyncCellID()
{
    CSingleLock lock(&m_SyncCellIDGroupLock, TRUE);
    m_CellIDGroup.clear();
}

long CellImageProducer::SizeSyncCellID()
{
    CSingleLock lock(&m_SyncCellIDGroupLock, TRUE);

    return m_CellIDGroup.size();
}
BOOL CellImageProducer::CheckGradation(long Value)
{
    CSingleLock lock(&m_GradationCountLock, TRUE);
    if (-1 == LastestGradationValue)
    {
        LastestGradationValue = Value;
        return TRUE;
    }
    if (128 == Value)
    {
        if (-127 == Value - LastestGradationValue)
        {
            LastestGradationValue = Value;
            return TRUE;
        }
    }
    else
    {
        if (1 == Value - LastestGradationValue)
        {
            LastestGradationValue = Value;
            return TRUE;
        }
    }
    LastestGradationValue = Value;
    return FALSE;
}
