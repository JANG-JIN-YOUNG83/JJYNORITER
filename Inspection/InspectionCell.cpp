#include "stdafx.h"
#include "InspectionCell.h"

#include "../CompanySharedLibrary/Persistence/Database.h"
#include "../VisionModule/Base/ParaCollector.h"
#include "../SharedModule/VisionHostShare/basedef.h"
#include "../IBCI/SurfaceInspect.h"

#include "Algorithm/ImageProcessing.h"
#include "Base/LoggerInterface.h"
#include "InspectionRoot.h"
#include "Gadget/TimeCheck.h"
#include "Base/LoggerInterface.h"

#include "Accessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInspectionCell::CInspectionCell(IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
	: CProcessingBase(m_unionResultData, ProcItemCategory::END /* Category 없음*/, logger, loggerResult)
	, m_logger(logger)
	, m_bUseISI(false)
{
	memset(m_processingList, 0, sizeof(m_processingList));

	//m_processingList[(long)ProcItemCategory::configuration] = new CInspectionConfiguration(this, logger, loggerResult);
	m_processingList[(long)ProcItemCategory::cellAlign] = new CInspectionCellAlign(this, logger, loggerResult);
	m_processingList[(long)ProcItemCategory::abnormalInspection] = new CInspectionAbnormal(this, logger, loggerResult);
	//m_processingList[(long)ProcItemCategory::uncoating] = new CInspectionUncoating(this, logger, loggerResult);
	//m_processingList[(long)ProcItemCategory::tape] = new CInspectionTape(this, logger, loggerResult);
//	m_processingList[(long)ProcItemCategory::surface] = new CInspectionUncoatingSurface(this, logger, loggerResult);
	//m_processingList[(long)ProcItemCategory::scrab] = new CInspectionScrab(this, logger, loggerResult);
	//m_processingList[(long)ProcItemCategory::tab] = new CInspectionTabFolding(this, logger, loggerResult);
	//m_processingList[(long)ProcItemCategory::shoulder] = new CInspectionShoulderLine(this, logger, loggerResult);
	//m_processingList[(long)ProcItemCategory::electrodeTorn] = new CInspectionElectrodeTorn(this, logger, loggerResult);
	//m_processingList[(long)ProcItemCategory::ngMark] = new CInspectionNgMark(this, logger, loggerResult);
    m_processingList[(long)ProcItemCategory::Chattering] = new CInspectionChattering(this, logger, loggerResult);
	m_processingList[(long)ProcItemCategory::measure] = new CInspectionMeasure(this, logger, loggerResult);
	//m_processingList[(long)ProcItemCategory::burr] = new CInspectionBurr(this, logger, loggerResult);
	//m_processingList[(long)ProcItemCategory::ETC] = new CInspectionEtc(this, logger, loggerResult);
	//m_processingList[(long)ProcItemCategory::ISI] = new CInspectionISI(this, logger, loggerResult);

	m_processingList[(long)ProcItemCategory::Blob] = new CInspectionBlob(this, logger, loggerResult);
	m_processingList[(long)ProcItemCategory::deeplearning] = new CInspectionDeeplearning(this, logger, loggerResult);
	


	//m_bufferBlob = new IPVM::BlobInfo[BLOB_NUM_MAX];
	//m_bInlineTest = true;
	//m_imagePosition = 0;

	m_iBlob = new CIBlob();
	m_psBlobInfo = new SBlobInfo[BLOB_NUM_MAX];
	m_cellResult.m_DetialResult = OtherDefect::OD_NONE;
	m_cellID = -1;

	for (auto* process : m_processingList)
	{
		if (process == nullptr)
		{
			ASSERT(!_T("Inspection Module를 생성하지 않고 Define만 추가하였다."));
			exit(0);
		}

		process->m_psBlobInfo = m_psBlobInfo;
		process->m_iBlob = m_iBlob;
	}

	m_bufferBlob = new IPVM::BlobInfo [BLOB_NUM_MAX];
	m_bInlineTest = true;
	m_imagePosition = 0;
	m_site = 0;
	m_generationversion = Share::GenerationVersion::Original;
	m_isTest = FALSE;

	m_isiSpecTable = nullptr;

}

CInspectionCell::~CInspectionCell()
{
	for (auto* process : m_processingList)
	{
		delete process;
	}

	delete[]m_bufferBlob;

	delete m_iBlob;
	delete[]m_psBlobInfo;
}

void CInspectionCell::ResetInspection()
{
	for (auto* process : m_processingList)
	{
		if (auto* inspection = dynamic_cast<CInspectionBase *>(process))
		{
			inspection->ResetInspection();
		}
	}
}

void CInspectionCell::Initialize()
{
	for (auto* process : m_processingList)
	{
		process->SetUpDown(m_isUp);
	}
}

void CInspectionCell::SetInspectionSplitImage(IPVM::Image_8u_C1 brightImage, IPVM::Image_8u_C1 darkImage)
{
	if (brightImage.GetSizeX() != darkImage.GetSizeX() || brightImage.GetSizeY() != darkImage.GetSizeY())
	{
		return;
	}

	bool changedSize = false;
	if (m_imageSizeX != brightImage.GetSizeX() ||
		m_imageSizeY != brightImage.GetSizeY())
	{
		changedSize = true;
	}

	m_imageSizeX = brightImage.GetSizeX();
	m_imageSizeY = brightImage.GetSizeY();

	// 참고: Image Size가 같을 때 IPVM::Image_xx_xx 의 Create 함수는 아무런 동작도 하지 않는다.
	m_bufferThreshold.Create(m_imageSizeX, m_imageSizeY);
	m_bufferThreshold2.Create(m_imageSizeX, m_imageSizeY);
	m_bufferThreshold3.Create(m_imageSizeX, m_imageSizeY);
	m_bufferThreshold4.Create(m_imageSizeX, m_imageSizeY);
	m_bufferThreshold5.Create(m_imageSizeX, m_imageSizeY);
	m_bufferThreshold6.Create(m_imageSizeX, m_imageSizeY);
	m_bufferLabel.Create(m_imageSizeX, m_imageSizeY);
	/*m_bufferLabelRoi1.Create(m_imageSizeX, m_imageSizeY);
	m_bufferLabelRoi1_Electrode.Create(m_imageSizeX, m_imageSizeY);
	m_bufferLabelRoi2.Create(m_imageSizeX, m_imageSizeY);
	m_bufferLabelRoi3.Create(m_imageSizeX, m_imageSizeY);
	m_bufferLabelRoi4.Create(m_imageSizeX, m_imageSizeY);
	m_bufferLabelRoi4_Electrode.Create(m_imageSizeX, m_imageSizeY);
	m_bufferLabelRoiRight.Create(m_imageSizeX, m_imageSizeY);
	m_bufferLabelRoiLeft.Create(m_imageSizeX, m_imageSizeY);*/
	m_tapeThreshold.Create(m_imageSizeX, m_imageSizeY);
	m_IslandBinaryImage.Create(m_imageSizeX, m_imageSizeY);

	if (changedSize)
	{
		// Test Run에서 계속 이함수를 호출해서 막아놓았다..
		ResetImageBuffer(true);
	}

    m_inspectionImageBright = brightImage;
    m_inspectionImageDark = darkImage;
	
	for (auto* process : m_processingList)
	{
        process->SetInspectionSplitImage(brightImage, darkImage);
		process->SetBufferThreshold(m_bufferThreshold);
		process->SetBufferThreshold2(m_bufferThreshold2);
		process->SetBufferThreshold3(m_bufferThreshold3);
		process->SetBufferThreshold4(m_bufferThreshold4);
		process->SetBufferThreshold5(m_bufferThreshold5);
		process->SetBufferThreshold6(m_bufferThreshold6);
		process->SetBufferLabel(m_bufferLabel);
		//process->SetBufferLabel_Burr_Only(m_bufferLabelRoi1, m_bufferLabelRoi2, m_bufferLabelRoi3, m_bufferLabelRoi4, m_bufferLabelRoi1_Electrode, m_bufferLabelRoi4_Electrode, m_bufferLabelRoiRight, m_bufferLabelRoiLeft);
		process->SetBufferBlob(m_bufferBlob);

		// m_tapeThreshold 를 사용하는 검사가 2개뿐이지만 그냥 다 넘겨줬음 (Uncoating, Tape)
		process->SetTapeThreshold(m_tapeThreshold);
	}
}

CProcessingBase* CInspectionCell::GetProcessingItem(ProcItemCategory categortIndex)
{
	return m_processingList[(long)categortIndex];
}

bool CInspectionCell::ProcInspection(Inspection::DebugInfo* debugInfo)
{
	std::vector<float> vecTimeTable;
	IPVM::TimeCheck TimeCheck;
	float preTime, endTime;
	PrePareInsp();
	m_cellResult.m_DetialResult = OtherDefect::OD_NONE;
	preTime = TimeCheck.Elapsed_ms();
	//auto* tabFolding = ((CInspectionTabFolding *)GetProcessingItem(ProcItemCategory::tab));
	//m_cellResult.tabImageIndex = tabFolding->m_para.m_imageUseIndex;

	//m_cellResult.cellPitchUm = 0;

//	m_logger.Log(0, _T("Inspection Start - Cell ID [%d]"), m_cellResult.cellID);

	auto* cellAlign = ((CInspectionCellAlign*)GetProcessingItem(ProcItemCategory::cellAlign));
	//auto* Surface = ((CInspectionUncoatingSurface*)GetProcessingItem(ProcItemCategory::surface));
	TimeCheck.Reset();
    
	m_CellAllDefect.Init(m_cellID, &m_inspectionImageBright, &m_inspectionImageDark);
	m_resultValue = II_RESULT_VALUE::PASS;
	if (cellAlign->m_para.m_bUseInspection)
	{
		cellAlign->m_cellID = m_cellID;
		IPVM::TimeCheck tc;
		for (auto* process : m_processingList)
		{
			//if (auto* item = dynamic_cast<CInspectionConfiguration *>(process))
			//{
			//	// 아직 작업되지 않았는지 실행되어 있지 않게 되어 있어서 일단 continue 해 놓음
			//	// 실행에 문제없다면 이 if문은 제거할 것
			//	continue;
			//}
			auto processSuccess = process->DoInspection(debugInfo);


			m_CellAllDefect.SetCandidate(process->m_CellAllDefect.m_vecCandidate);

			if (auto* item = dynamic_cast<CInspectionCellAlign *>(process))
			{
				//m_cellResult.isNoTab = item->m_isNoTab;

				if (!processSuccess)
				{
					m_cellResult.m_DetialResult = (true == m_cellResult.isUp) ? OtherDefect::OD_UP_ALIGN_FAIL : OtherDefect::OD_DOWN_ALIGN_FAIL;
					m_resultValue = II_RESULT_VALUE::INVALID;

					if (m_bInlineTest)
					{
						m_cellResult.bInvalid = TRUE;
					}

					m_bInlineTest = true;

					return false;
				}
			}
			else if (auto* item = dynamic_cast<CInspectionAbnormal*>(process))
			{
				//m_cellResult.isNoTab = item->m_isNoTab;

				if (!processSuccess)
				{
					m_cellResult.m_DetialResult =  OtherDefect::OD_ABNORMAL;
					m_resultValue = II_RESULT_VALUE::INVALID;

					if (m_bInlineTest)
					{
						m_cellResult.bInvalid = TRUE;
					}

					m_bInlineTest = true;

					return false;
				}
			}

			
			vecTimeTable.push_back(TimeCheck.Elapsed_ms());
			TimeCheck.Reset();
		}
	}
	else
	{
		ResetResult(II_RESULT_VALUE::PASS);
	}

	SetTotalResult();

	/*if (true == m_CellAllDefect.m_bUse)
	{
		m_isiSpecTable = &Inspection::Accessor::Get().m_isi_spec;

		CISIClassification cfct(m_isiSpecTable);
		cfct.Start(&m_CellAllDefect);
	}*/
	
	// 이걸로 속도 줄이기
	GetSurfaceResult_ISI();

	// DefectType, ResultCode 넣어줌
	m_ISICellAllDefect.SetResultData();
	GetTotalResultInfo(m_cellResult);

	endTime = TimeCheck.Elapsed_ms();
    m_ISICellAllDefect.m_dInspTotalTime_ms = endTime;

	CString strText;
	for (int i = 0; i < vecTimeTable.size(); i++)
	{
		CString str;
		str.Format(_T("(%s)%.2f ms, "), GetProcItemModuleName((ProcItemCategory)i),vecTimeTable[i]);
		strText.Append(str);
	}
	
	m_logger.Log(0, _T("[%s - %d]%s(%.2f,%.2f)"), this->m_isUp == true ? _T("UP") : _T("DOWN"), this->m_cellID, strText,preTime, endTime);
	m_bInlineTest = true;

	
	return true;
}

BOOL CInspectionCell::GetTotalResultInfo(RESULT_INFO& result)
{
	result.Clear();

	for (auto* process : m_processingList)
	{
		if (auto* item = dynamic_cast<CInspectionCellAlign*>(process))
		{
			result.GetSub(process->m_itemCategory).Append(item->m_rejectInfo);
		}
		if (auto* inspection = dynamic_cast<CInspectionBase *>(process))
		{
			result.GetSub(process->m_itemCategory).Append(inspection->m_rejectInfo);
		}
	}


	MergeResultInfo();

	result.bInvalid = FALSE;
	result.imagePosition = m_imagePosition;
	result.isUp = m_isUp;
	result.cellID = m_cellID;

	result.m_grabTime_ms = m_grabTime_ms;
	result.cellOffset = m_cellOffset;
	result.cellSizePixel = m_cellSizePixel;

	result.cellBoundaryLine = m_unionResult.m_lineCellBoundaryRight4Shoulder;
	result.shoulderLine = m_unionResult.m_lineSegShoulder;
	result.shoulderLineRect = m_unionResult.m_rectShoulder;
	result.shoulderPoint1 = m_unionResult.m_ptShoulder1;
	result.shoulderPoint2 = m_unionResult.m_ptShoulder2;
	result.shoulderRect1 = m_unionResult.m_rtShoulder1;
	result.shoulderRect2 = m_unionResult.m_rtShoulder2;
	result.m_measureShoulderLine = m_unionResult.m_measureShoulderLine;

	auto& cellAlign = (CInspectionCellAlign&)(*GetProcessingItem(ProcItemCategory::cellAlign));
//	auto& moduleShoulderLine = (CInspectionShoulderLine&)(*GetProcessingItem(ProcItemCategory::shoulder));
	auto& moduleMeasure = (CInspectionMeasure&)(*GetProcessingItem(ProcItemCategory::measure));

	long nEnd = (long)ProcItemCategory::END;
	if ((long)ProcItemCategory::cellAlign >= nEnd || (long)ProcItemCategory::shoulder >= nEnd || (long)ProcItemCategory::measure >= nEnd)
		return FALSE;

//	result.shoulderLineCoatingUm = moduleShoulderLine.m_ShoulderCoatingValueUm;

	result.m_measureTabHeight = moduleMeasure.m_measureTabHeight;
	result.m_measureTabHeight2 = moduleMeasure.m_measureTabHeight2;
	result.m_measureTabPitch = moduleMeasure.m_measureTabPitch;
	//if(moduleMeasure.m_para.m_nTabSideCalcPos == 0)
	result.m_measureTabSide = moduleMeasure.m_measureTabSide;
	//else
	//	result.m_measureTabSide = moduleMeasure.m_measureTabSide2;
	result.m_measureTabWidth = moduleMeasure.m_measureTabWidth;
	result.m_measureCellLengthTop = moduleMeasure.m_measureCellLengthTop;
	result.m_measureCellLengthBot = moduleMeasure.m_measureCellLengthBot;
	result.m_measureCellLength2_Top = moduleMeasure.m_measureCellLength2_Top;
	result.m_measureCellLength2_Bot = moduleMeasure.m_measureCellLength2_Bot;
	
	result.m_measureCuttingHeightTop = moduleMeasure.m_measureCuttingHeightTop;
	result.m_measureCuttingHeightBot = moduleMeasure.m_measureCuttingHeightBot;
	result.cellLengthTopUmOffset = moduleMeasure.m_fCellLengthTopUmOffset;
	result.cellLengthBotUmOffset = moduleMeasure.m_fCellLengthBotUmOffset;
	result.m_measureMisalign = moduleMeasure.m_measureMisalign;
	result.m_measureVHomeTiltTop = moduleMeasure.m_measureVHomeTiltTop;
	result.m_measureVHomeTiltBot = moduleMeasure.m_measureVHomeTiltBot;

	result.cellPitchErrorUm = moduleMeasure.m_fCellPitchErrorUm;
	result.cellLengthTopErrorUm = moduleMeasure.m_fCellLengthTopErrorUm;
	result.cellLengthBotErrorUm = moduleMeasure.m_fCellLengthBotErrorUm;
	result.tabWidthErrorUm = moduleMeasure.m_fTabWidthErrorUm;
	result.tabHeightErrorUm = moduleMeasure.m_fTabHeightErrorUm;
	result.misalignErrorUm = moduleMeasure.m_fMisalignErrorUm;
	result.vhomeTiltBotErrorUm = moduleMeasure.m_fVhomeTiltBotErrorUm;
	result.vhomeTiltTopErrorUm = moduleMeasure.m_fVhomeTiltTopErrorUm;
	result.tabSideErrorUm = moduleMeasure.m_fTabSideErrorUm;
	result.vhomeHeightLeftUm = moduleMeasure.m_fVhomeHeightLeftUm;
	result.vhomeHeightRightUm = moduleMeasure.m_fVhomeHeightRightUm;

	result.vhomeHeightLeftRect = moduleMeasure.m_rectVhomeLeftTop;
	result.vhomeHeightRightRect = moduleMeasure.m_rectVhomeRightTop;

	//result.cellPitchPixel = m_inspectionCellAlign.cellPitchPixel;
	//result.cellHeightPixel = m_inspectionCellAlign.cellHeightPixel;
	//result.tabWidthPixel = m_inspectionTabFolding.m_rectTabRough.Height();
	//result.tabHeightPixel = m_inspectionTabFolding.m_rectTabRough.Width();
	//result.misalignPixel = m_inspectionCellAlign.misalignPixel;
	//result.tabSidePixel = abs(m_inspectionTabFolding.m_rectTabRough.m_top - m_inspectionCellAlign.vhomeRightStartPixel.m_y);

	

	result.cuttingHeightTopRectPixel = moduleMeasure.m_rectCuttingHeightTop;
	result.cuttingHeightBotRectPixel = moduleMeasure.m_rectCuttingHeightBot;

	IPVM::Point_32f_C2 pt1;
	IPVM::Point_32f_C2 pt2;
	pt1 = cellAlign.m_unionResult.m_ptVHomeInner[II_CORNER_VALUE::II_RIGHT_TOP];
	pt2 = cellAlign.m_unionResult.m_ptVHomeOuter[II_CORNER_VALUE::II_RIGHT_TOP];
	result.vhomeTiltTopRightRect.m_left = min(pt1.m_x - 1, pt2.m_x - 1);
	result.vhomeTiltTopRightRect.m_right = max(pt1.m_x + 1, pt2.m_x + 1);
	result.vhomeTiltTopRightRect.m_top = min(pt1.m_y, pt2.m_y);
	result.vhomeTiltTopRightRect.m_bottom = max(pt1.m_y, pt2.m_y);
	result.vhomeTiltTopRightInner = pt1;
	result.vhomeTiltTopRightOuter = pt2;

	pt1 = cellAlign.m_unionResult.m_ptVHomeInner[II_CORNER_VALUE::II_LEFT_TOP];
	pt2 = cellAlign.m_unionResult.m_ptVHomeOuter[II_CORNER_VALUE::II_LEFT_TOP];
	result.vhomeTiltTopLeftRect.m_left = min(pt1.m_x - 1, pt2.m_x - 1);
	result.vhomeTiltTopLeftRect.m_right = max(pt1.m_x + 1, pt2.m_x + 1);
	result.vhomeTiltTopLeftRect.m_top = min(pt1.m_y, pt2.m_y);
	result.vhomeTiltTopLeftRect.m_bottom = max(pt1.m_y, pt2.m_y);
	result.vhomeTiltTopLeftInner = pt1;
	result.vhomeTiltTopLeftOuter = pt2;

	result.vhomeTiltTopLeftInnerFloat = pt1;
	result.vhomeTiltTopLeftOuterFloat = pt2;

	pt1 = cellAlign.m_unionResult.m_ptVHomeInner[II_CORNER_VALUE::II_RIGHT_BOTTOM];
	pt2 = cellAlign.m_unionResult.m_ptVHomeOuter[II_CORNER_VALUE::II_RIGHT_BOTTOM];
	result.vhomeTiltBotRightRect.m_left = min(pt1.m_x - 1, pt2.m_x - 1);
	result.vhomeTiltBotRightRect.m_right = max(pt1.m_x + 1, pt2.m_x + 1);
	result.vhomeTiltBotRightRect.m_top = min(pt1.m_y, pt2.m_y);
	result.vhomeTiltBotRightRect.m_bottom = max(pt1.m_y, pt2.m_y);
	result.vhomeTiltBotRightInner = pt1;
	result.vhomeTiltBotRightOuter = pt2;

	pt1 = cellAlign.m_unionResult.m_ptVHomeInner[II_CORNER_VALUE::II_LEFT_BOTTOM];
	pt2 = cellAlign.m_unionResult.m_ptVHomeOuter[II_CORNER_VALUE::II_LEFT_BOTTOM];
	result.vhomeTiltBotLeftRect.m_left = min(pt1.m_x - 1, pt2.m_x - 1);
	result.vhomeTiltBotLeftRect.m_right = max(pt1.m_x + 1, pt2.m_x + 1);
	result.vhomeTiltBotLeftRect.m_top = min(pt1.m_y, pt2.m_y);
	result.vhomeTiltBotLeftRect.m_bottom = max(pt1.m_y, pt2.m_y);
	result.vhomeTiltBotLeftInner = pt1;
	result.vhomeTiltBotLeftOuter = pt2;

	result.vhomeTiltBotLeftInnerFloat = pt1;
	result.vhomeTiltBotLeftOuterFloat = pt2;

	result.tabRect = m_unionResult.m_rectTabRough;
	result.tabRectForHeight = m_unionResult.m_rectTabForHeight;

	//result.isNoTab = cellAlign.m_isNoTab;


	//result.inkAutoCalibError = (cellAlign.m_para.m_nCellBoundaryBaseLine - cellAlign.m_unionResult.m_lineCellBoundaryRight4Shoulder.CenterPoint().m_x) * m_pixelToUmX;	// um

	result.m_isRejectTape = m_unionResult.m_isRejectTape;
	result.m_isRejectUncoatingBoundary = m_unionResult.m_isRejectUncoatingBoundary;

	return TRUE;
}
SURFACE_RESULT_INFO CInspectionCell::GetSurfaceResult_ISI(BOOL bView)
{
	IPVM::TimeCheck TimeCheck;
	TimeCheck.Reset();
	//auto* surface_isi = (CInspectionISI*)GetProcessingItem(ProcItemCategory::ISI);

	SURFACE_RESULT_INFO surfaceResultInfo;

	auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
	CISIClassification classfication(&isi_spec);
	
	classfication.Start(&m_ISICellAllDefect,bView);

    SYSTEMTIME tmModify;
    ::GetLocalTime(&tmModify);
    m_ISICellAllDefect.m_strTime.Format(_T("%04d%02d%02d%02d%02d%02d"), tmModify.wYear, tmModify.wMonth, tmModify.wDay,
        tmModify.wHour,
        tmModify.wMinute, tmModify.wSecond);

	//ㅇ블랍으로 검색 .. 
	auto& Defects = m_ISICellAllDefect.m_vecCandidate;

	std::vector<int> vecIndex; //일단 크기별로 정렬

	std::sort(Defects.begin(), Defects.end(), [](const CISIDefectProperty& a, const CISIDefectProperty& b) {return a.SIZE_X_mm* a.SIZE_Y_mm > b.SIZE_X_mm * b.SIZE_Y_mm; });

	float hhtime = TimeCheck.Elapsed_ms();
	int nRejectCount = 0;
	int nIndex = 0;
	for (auto& Defect : Defects)
	{
		if (nRejectCount < MAX_FULL_DEFECT_COUNT)
		{
			switch (Defect.GetJudge())
			{
			case II_RESULT_VALUE::PASS:
				Defect.strTempUI = _T("OK");
				break;
			case II_RESULT_VALUE::REJECT:
				Defect.strTempUI = _T("NG");
                m_resultValue = II_RESULT_VALUE::REJECT;
				/*for (int i = 0; i < nIndex; i++)
				{
					if (_T("NG") == m_ISICellAllDefect.m_vecCandidate[i].strTempUI)
					{
						IPVM::Rect overlapRect = IPVM::Rect(0, 0, 0, 0);
						overlapRect.IntersectRect(Defect.rtRect_pixel, m_ISICellAllDefect.m_vecCandidate[i].rtRect_pixel);
						if (!(overlapRect.IsRectEmpty() || overlapRect.IsRectNull()))
						{
							Defect.strTempUI = _T("SKIP");
							break;
						}
					}
				}*/
				break;
			case II_RESULT_VALUE::NOT_CANDIDATE:
				Defect.strTempUI = _T("SKIP");
				break;
			default:
				ASSERT(1); //판정이 다 나와야함
				break;
			}

			if (Defect.strTempUI == _T("NG"))
			{
				surfaceResultInfo.m_strFullDefectName[nRejectCount] = GetDefectName(Defect.GetDefectResultCode());
				surfaceResultInfo.m_dFullSizeX[nRejectCount][2] = Defect.SIZE_X_mm;			//불량사이즈X
				surfaceResultInfo.m_dFullSizeY[nRejectCount][2] = Defect.SIZE_Y_mm;			//불량사이즈Y
				surfaceResultInfo.m_rtRawDefect[nRejectCount] = IPVM::ToMFC(Defect.rtRect_pixel);  //detail 영역
				surfaceResultInfo.m_rtFullDefect[nRejectCount] = IPVM::ToMFC(Defect.rtRect_pixel);	//불량영역
				surfaceResultInfo.m_nFullDefectType[nRejectCount] = (int)Defect.GetDefectResultCode();										//불량코드
				nRejectCount++;
			}
		}
		else
		{
			Defect.strTempUI = _T("SKIP");
		}
		nIndex++;
	}

	surfaceResultInfo.m_nFullDefectCount = nRejectCount;
	float time = TimeCheck.Elapsed_ms();

	return surfaceResultInfo;
}

bool CInspectionCell::Filter_ISI(CISIDefectProperty& Defect, CString num, bool bisSkip)
{
	//host로 보내지 않을거면 false(OK, SKIP)
	CString strDefectName = GetDefectName(Defect.GetDefectResultCode());
	Defect.strTempUI = num;

	if (true == bisSkip)
	{
		m_vecResult_SKIP.push_back(Defect);
		return true;
	}

	if (II_RESULT_VALUE::PASS == Defect.GetJudge())
	{
		m_vecResult_OK.push_back(Defect);
		return false;
	}
	
	if (0 != m_vecResult_NG.size())
	{
		bool res = false;
		for (auto& ROI : m_vecResult_NG)
		{
			IPVM::Rect overlapRect = IPVM::Rect(0, 0, 0, 0);
			overlapRect.IntersectRect(Defect.rtRect_pixel, ROI.rtRect_pixel);
			if (!(overlapRect.IsRectEmpty() || overlapRect.IsRectNull()))
			{
				res = true;//겹치는부분이 있다.
				//원래는 면적 비교
				if (Defect.AREA > ROI.AREA)  //새로 비교하는 항목이 더 크다.
				{
					m_vecResult_SKIP.push_back(ROI); //교체
					ROI = Defect;
					return true;
				}
				else
				{
					m_vecResult_SKIP.push_back(Defect);
					return true;
				}
			}
			
		}
		m_vecResult_NG.push_back(Defect); // 겹치는게 없으니 NG
		return true;
	}
	else
	{
		m_vecResult_NG.push_back(Defect);
		return true;
	}


}

SURFACE_RESULT_INFO CInspectionCell::GetSurfaceResult()
{
	SURFACE_RESULT_INFO surfaceResultInfo;
	//auto* surface = (CInspectionUncoatingSurface*)GetProcessingItem(ProcItemCategory::surface);
	//auto* uncoating = (CInspectionUncoating*)GetProcessingItem(ProcItemCategory::uncoating);
	//auto* scrab = (CInspectionScrab*)GetProcessingItem(ProcItemCategory::scrab);

	//int camID = m_isUp;

	//CString text, temp;
	//text = m_isUp ? _T("UP") : _T("DOWN");

	//temp.Format(_T("Cell id %d, "), m_cellID);
	//text += temp;
	//surfaceResultInfo.cellID = m_cellID;
	//
	//if (uncoating->m_rectInsulation.size()) {
	//	surfaceResultInfo.m_rtInsulate = IPVM::ToMFC(uncoating->m_rectInsulation.front());
	//	surfaceResultInfo.m_rectInsulate = uncoating->m_rectInsulation.front();
	//}
	//else {
	//	surfaceResultInfo.m_rtInsulate = CRect(0, 0, 0, 0);
	//	surfaceResultInfo.m_rectInsulate = IPVM::Rect(0, 0, 0, 0);

	//}

	//if (uncoating->m_rectOverlay.size()) {
	//	surfaceResultInfo.m_rtOverlay = IPVM::ToMFC(uncoating->m_rectOverlay.front());
	//	surfaceResultInfo.m_rectOverlay = uncoating->m_rectOverlay.front();
	//}
	//else {
	//	surfaceResultInfo.m_rtOverlay = CRect(0, 0, 0, 0);
	//	surfaceResultInfo.m_rectOverlay = IPVM::Rect(0,0,0,0);
	//}

	//if (uncoating->m_rectOverlay.size() > 1) {
	//	surfaceResultInfo.m_rectBotOverlay = uncoating->m_rectOverlay[1];	// m_rectOverlay[0] : Top Overlay Rect
	//}
	//else {
	//	surfaceResultInfo.m_rectBotOverlay = IPVM::Rect(0, 0, 0, 0);
	//}

	//if(uncoating->m_insulationWidth > 0)
	//	surfaceResultInfo.m_dInsulWidth = surfaceResultInfo.m_dInsulate = uncoating->m_insulationWidth / 1000.0f;
	//else
	//	surfaceResultInfo.m_dInsulWidth = surfaceResultInfo.m_dInsulate = uncoating->m_insulationWidth;
	//if (uncoating->m_overlayWidth > 0)
	//	surfaceResultInfo.m_dOverlayWidth = surfaceResultInfo.m_dOverlay = uncoating->m_overlayWidth / 1000.0f;
	//else
	//	surfaceResultInfo.m_dOverlayWidth = surfaceResultInfo.m_dOverlay = uncoating->m_overlayWidth;
	//if (uncoating->m_overlayWidth_Bot > 0)
	//	surfaceResultInfo.m_dBotOverlay = uncoating->m_overlayWidth_Bot / 1000.0f;
	//else
	//	surfaceResultInfo.m_dBotOverlay = uncoating->m_overlayWidth_Bot;
	//surfaceResultInfo.isUp = m_isUp;
	//temp.Format(_T("InsulationWidth%.3f, OverlayWidth%.3f"), surfaceResultInfo.m_dInsulWidth, surfaceResultInfo.m_dOverlayWidth);
	//text += temp;


	//for (long index = 0; index < 10; index++) surfaceResultInfo.m_dInsTime[index] = 0;
	//int imageSizeX = m_inspectionImageBright.GetWidthBytes();
	//int imageSizeY = m_inspectionImageBright.GetSizeY();
	//CRect rectReverse;
	//surfaceResultInfo.m_inputTime = m_inputTime;
	//surfaceResultInfo.fullImage = m_inspectionImageFull;
	//surfaceResultInfo.brightImage = m_inspectionImageBright;
	//surfaceResultInfo.darkImage = m_inspectionImageDark;
	//surfaceResultInfo.isUp = m_isUp;
	//surfaceResultInfo.cellID = m_cellID;
	//surfaceResultInfo.rejectCount = 0;
	//surfaceResultInfo.m_nFullDefectCount = surface->m_nFullDefectCount;
	//surfaceResultInfo.m_rectCellArea = IPVM::FromMFC(m_rtCell);
	//surfaceResultInfo.m_rectTabArea = IPVM::FromMFC(m_rtCellTab);
	//if (scrab->m_scrabRect.size())
	//	surfaceResultInfo.m_rectScrab = scrab->m_scrabRect.front();	// sjj - 0412 변수 수정
	//else
	//	surfaceResultInfo.m_rectScrab = IPVM::Rect(0, 0, 0, 0);

	//int nIndex = 0;

	////BOOL  bIsMiniCraterNG = FALSE;  //미니항목들 초기화
	////BOOL  bIsMiniDentNG = FALSE;
	////BOOL  bIsMiniPinholeNG = FALSE;
	////BOOL  bIsMiniProtrutionNG = FALSE;


	////if (surface->m_para.m_bMiniCrater)
	////{
	////	int nMiniCraterCount = 0;

	////	for (int j = 0; j < surface->m_nFullDefectCount; j++)
	////	{
	////		if (3000 == m_nFullDefectType[j]) {
	////			nMiniCraterCount++;
	////			if (nMiniCraterCount >= surface->m_para.m_nMiniCrater_Count)
	////			{
	////				bIsMiniCraterNG = TRUE;
	////				break;
	////			}
	////		}
	////	}
	////}
	////if (surface->m_para.m_bMiniPinhole)
	////{
	////	int nMiniPinholeCount = 0;
	////	for (int j = 0; j < surface->m_nFullDefectCount; j++)
	////	{
	////		if (2000 == m_nFullDefectType[j]) {
	////			nMiniPinholeCount++;
	////			if (nMiniPinholeCount >= surface->m_para.m_nMiniPinhole_Count)
	////			{
	////				bIsMiniPinholeNG = TRUE;
	////				break;
	////			}
	////		}
	////	}
	////}
	////if (surface->m_para.m_bMiniProtrution)
	////{
	////	int nMiniProtrutionCount = 0;
	////	for (int j = 0; j < surface->m_nFullDefectCount; j++)
	////	{
	////		if (2102 == m_nFullDefectType[j]) {
	////			nMiniProtrutionCount++;
	////			if (nMiniProtrutionCount >= surface->m_para.m_nMiniProtrution_Count)
	////			{
	////				bIsMiniProtrutionNG = TRUE;
	////				break;
	////			}
	////		}
	////	}
	////}

	////if (surface->m_para.m_bMiniDent)
	////{
	////	int nMiniDentCount = 0;
	////	for (int j = 0; j < surface->m_nFullDefectCount; j++)
	////	{
	////		if (2005 == m_nFullDefectType[j]) {
	////			nMiniDentCount++;
	////			if (nMiniDentCount >= surface->m_para.m_nMiniDent_Count)
	////			{
	////				bIsMiniDentNG = TRUE;
	////				break;
	////			}
	////		}
	////	}
	////}
	//for (int i = 0; i < surfaceResultInfo.m_nFullDefectCount; i++)
	//{
	//	rectReverse = m_rtFullDefect[i];
	//	rectReverse.top = imageSizeY - m_rtFullDefect[i].bottom;
	//	rectReverse.bottom = imageSizeY - m_rtFullDefect[i].top;
	//	surfaceResultInfo.m_rtFullDefect[i] = m_rtFullDefect[i];								//불량영역
	//	surfaceResultInfo.m_fmDefect[i] = m_fmDefect[i];//origin							//불량이미지
	//	surfaceResultInfo.m_nFullDefectType[i] = m_nFullDefectType[i];							//불량코드
	//	surfaceResultInfo.m_cpDefectCenter[i] = CPoint(0, 0);
	//	surfaceResultInfo.m_rtRawDefect[i] = m_rtFullDefect[i];
	//		if (surfaceResultInfo.m_nFullDefectType[i] <= 0)
	//		{
	//			continue;
	//		}

	//		surfaceResultInfo.rejectCount++;
	//	/*	if (2102 == m_nFullDefectType[i])
	//		{
	//			if (FALSE == bIsMiniProtrutionNG)
	//			{
	//				m_strFullDefectName[i] = _T("OK");
	//				for (int i = 0; i < surface->m_vecstSurfaceResult.size(); i++)
	//				{
	//					if (GetDefectNameWithCode(2102) == surface->m_vecstSurfaceResult[i].strSurfaceResult)
	//						surface->m_vecstSurfaceResult[i].strSurfaceResult = _T("OK");
	//				}
	//			}
	//			else
	//				m_strFullDefectName[i] = STR_Delamination_Crack;
	//		}*/

	//		//if (2000 == m_nFullDefectType[i])
	//		//{
	//		//	if (FALSE == bIsMiniPinholeNG)
	//		//	{
	//		//		m_strFullDefectName[i] = _T("OK");
	//		//		for (int i = 0; i < surface->m_vecstSurfaceResult.size(); i++)
	//		//		{
	//		//			if (GetDefectNameWithCode(2000) == surface->m_vecstSurfaceResult[i].strSurfaceResult)
	//		//				surface->m_vecstSurfaceResult[i].strSurfaceResult = _T("OK");
	//		//		}
	//		//	}
	//		//	else
	//		//		m_strFullDefectName[i] = STR_PINHOLE;
	//		//}

	//		//if (3000 == m_nFullDefectType[i])
	//		//{
	//		//	if (FALSE == bIsMiniCraterNG)
	//		//	{
	//		//		m_strFullDefectName[i] = _T("OK");
	//		//		for (int i = 0; i < surface->m_vecstSurfaceResult.size(); i++)
	//		//		{
	//		//			if (GetDefectNameWithCode(3000) == surface->m_vecstSurfaceResult[i].strSurfaceResult)
	//		//				surface->m_vecstSurfaceResult[i].strSurfaceResult = _T("OK");
	//		//		}
	//		//	}
	//		//	else
	//		//		m_strFullDefectName[i] = STR_CRATER;
	//		//}

	//		//if (2003 == m_nFullDefectType[i])
	//		//{
	//		//	if (false == bIsMiniDentNG)
	//		//	{
	//		//		m_strFullDefectName[i] = _T("ok");
	//		//		for (int i = 0; i < surface->m_vecstSurfaceResult.size(); i++)
	//		//		{
	//		//			if (GetDefectNameWithCode(2003) == surface->m_vecstSurfaceResult[i].strSurfaceResult)
	//		//				surface->m_vecstSurfaceResult[i].strSurfaceResult = _T("ok");
	//		//		}
	//		//	}
	//		//	else
	//		//		m_strFullDefectName[i] = STR_DENT;
	//		//}

	//		//surface->MiniDefectJudgment(surface->m_nFullDefectCount);

	//		if (6004 == m_nFullDefectType[i])
	//		{
	//			if (m_site == (int)CIS_Site::ESOC)
	//				surfaceResultInfo.m_strFullDefectName[i] = STR_HEATING_WRINKLE;
	//			else
	//				surfaceResultInfo.m_strFullDefectName[i] = STR_INSULATION_ISLAND;
	//		}
	//		else
	//			surfaceResultInfo.m_strFullDefectName[i] = m_strFullDefectName[i];						//불량명
	//	for (int k = 0; k < 2; k++)
	//	{																		//0백색1흑색
	//		surfaceResultInfo.m_dFullStdValue[i][k] = surface->m_dFullStdValue[i][k];						//불량편차값
	//		surfaceResultInfo.m_dFullCompactness[i][k] = surface->m_dFullCompactness[i][k];					//불량진원도
	//		surfaceResultInfo.m_dFullDistance[i][k] = surface->m_dFullDistance[i][k];						//흑백간의거리
	//		surfaceResultInfo.m_dFullPercent[i][k] = surface->m_dFullPercent[i][k];						//불량영역내 퍼센트
	//		surfaceResultInfo.m_dFullPeakDataW[i][k] = surface->m_dFullPeakDataW[i][k];					//백색Value [0] - OriginValue 1-Preprocessing value
	//		surfaceResultInfo.m_dFullPeakDataB[i][k] = surface->m_dFullPeakDataB[i][k];					//흑색Value [0] - OriginValue 1-Preprocessing value
	//	}
	//	
	//	for (int k = 0; k < 3; k++)
	//	{
	//		if (surface->m_dFullAngle[i][k] > 0)
	//		{
	//			surfaceResultInfo.m_dFullAngle[i] = surface->m_dFullAngle[i][k];
	//			break;
	//		}
	//		else
	//			surfaceResultInfo.m_dFullAngle[i] = 0;
	//	}//불량각도
	//	for (long type = 0; type < 3; type++)
	//	{
	//		surfaceResultInfo.m_dFullSizeX[i][type] = surface->m_dFullSizeX[i][type];			//불량사이즈X
	//		surfaceResultInfo.m_dFullSizeY[i][type] = surface->m_dFullSizeY[i][type];			//불량사이즈Y
	//	}
	//}


	//surfaceResultInfo.isPetVinyl = false;
	return surfaceResultInfo;
}

void CInspectionCell::MergeResultInfo()
{
	m_cellResult.m_totalRejectInfo.Clear();

	for (long categoryIndex = 0; categoryIndex < (long)ProcItemCategory::END; categoryIndex++)
	{
		m_cellResult.m_totalRejectInfo.Append(m_cellResult.GetSub((ProcItemCategory)categoryIndex));
	}
}

void CInspectionCell::LinkDataBase(bool bSave, Database &dbJob)
{
	Base::FUNCTION_PARACOLLECTOR_CAMERA collector(m_isUp ? _T("Top") : _T("Bottom"));

	for (auto* process : m_processingList)
	{
		CString moduleName = GetProcItemModuleName(process->m_itemCategory);

		Base::FUNCTION_PARACOLLECTOR_MODULE collector_module(moduleName);
		process->LinkDataBase(bSave, dbJob[moduleName]);
	}
}

bool CInspectionCell::CopyDataBase(CProcessingBase* src)
{
	auto *ptr = dynamic_cast<CInspectionCell *>(src);
	if (ptr == nullptr)
	{
		ASSERT(!_T("같은 타입의 Inspection만 복사가 가능하다"));
		return false;
	}

	auto& rhs = *ptr;

	m_isUp = rhs.m_isUp;
    m_nSide = rhs.m_nSide;

	m_pixelToUmX = rhs.m_pixelToUmX;
	m_pixelToUmY = rhs.m_pixelToUmY;
	m_cellDesign = rhs.m_cellDesign;

	m_site = rhs.m_site;
	m_generationversion = rhs.m_generationversion;
	m_nCellCuttingMargin = rhs.m_nCellCuttingMargin;

	SetPixelToUmX(m_pixelToUmX);
	SetPixelToUmY(m_pixelToUmY);
	SetCellCuttingMargin(m_nCellCuttingMargin);
	SetCellDesign(m_cellDesign);

	for (long procIndex = 0; procIndex < (long)ProcItemCategory::END; procIndex++)
	{
		m_processingList[procIndex]->CopyDataBase(rhs.m_processingList[procIndex]);
	}

	return true;
}

void CInspectionCell::ResetResult(II_RESULT_VALUE result)
{
	m_resultValue = result;

	for (auto* process : m_processingList)
	{
		process->ResetResult(result);
	}

	// 어깨선, mismatch 값 초기화
	m_cellResult.m_measureShoulderLine.Reset();
	m_cellResult.mismatchUm = 0;
	for (int i = 0; i < MAX_FULL_DEFECT; i++) {
		m_nFullDefectType[i]=0;
		m_strFullDefectName[i] = _T("");
		m_rtFullDefect[i] = CRect(0, 0, 0, 0);
		
		m_rtRawDefect[i][0] = IPVM::Rect_32f(0, 0, 0, 0);
		m_rtRawDefect[i][1] = IPVM::Rect_32f(0, 0, 0, 0);
		m_rtRawDefect[i][2] = IPVM::Rect_32f(0, 0, 0, 0);
		m_rtRawDefect[i][3] = IPVM::Rect_32f(0, 0, 0, 0);

	}
	/*auto* Surface = ((CInspectionUncoatingSurface*)GetProcessingItem(ProcItemCategory::surface));
	Surface->ResetSurface();*/
}

void CInspectionCell::ResetISI()
{
	m_vecResult_NG.clear();
	m_vecResult_OK.clear();
	m_vecResult_SKIP.clear();
	m_ISICellAllDefect.Init(m_cellID, &m_inspectionImageBright, &m_inspectionImageDark); //변수 재활용

//	auto& ISI = (CInspectionISI&)(*GetProcessingItem(ProcItemCategory::ISI));
	//ISI.CommonReset();
}

void CInspectionCell::SetPixelToUmX(double x)
{
	m_pixelToUmX = x;

	for (auto* process : m_processingList)
	{
			process->SetPixelToUmX(x);
	}
}

void CInspectionCell::SetVersion(int nGenerationversion, int nSite)
{
	m_generationversion = nGenerationversion;
	m_site = nSite;
}

void CInspectionCell::SetSurfaceOption(const bool& bUseISI)
{
	m_bUseISI = bUseISI;

	for (auto* process : m_processingList)
	{
		process->m_CellAllDefect.m_bUse = bUseISI;
	}
}

void CInspectionCell::SetPixelToUmY(double y)
{
	m_pixelToUmY = y;

	for (auto* process : m_processingList)
	{
			process->SetPixelToUmY(y);
	}
}

void CInspectionCell::SetCellCuttingMargin(long nMargin)
{
	m_nCellCuttingMargin = nMargin*0.5;

	for (auto* process : m_processingList)
	{
		process->SetCellCuttingMargin(nMargin * 0.5);
	}
}

void CInspectionCell::SetImagePosition(long p)
{
	m_imagePosition = p;
}

void CInspectionCell::SetCellDesign(const CELL_DESIGN& design)
{
	m_cellDesign = design;

	for (auto* process : m_processingList)
	{
		process->SetCellDesign(design);
	}
}

void CInspectionCell::SetTotalResult()
{
	II_RESULT_VALUE resultValue = II_RESULT_VALUE::PASS;

	for (auto* process : m_processingList)
	{

		// Test
		/*if (auto* item = dynamic_cast<CInspectionTape*>(process))
		{
			continue;
		}
		if (auto* item = dynamic_cast<CInspectionUncoating*>(process))
		{
			continue;
		}*/

		if (auto* inspection = dynamic_cast<CInspectionBase *>(process))
		{
			if (inspection->IsInspectionEnabled())
			{
				resultValue = max(resultValue, inspection->m_resultValue);
			}
		}
	}

	m_resultValue = resultValue;
}


CString CInspectionCell::GetDefectNameWithCode(int nCode)
{
	CString str = _T("OK");

	if (nCode == 1001)		str.Format(_T("%s"), _T("Line_1001"));
	else if (nCode == 1002)	str.Format(_T("%s"), _T("Line_1002"));
	else if (nCode == 1003)	str.Format(_T("%s"), _T("Scratch_1003"));
	else if (nCode == 1004)	str.Format(_T("%s"), _T("Line_1004"));
	else if (nCode == 1005)	str.Format(_T("%s"), _T("Heating Wrinkle_1005"));
	else if (nCode == 1006)	str.Format(_T("%s"), _T("Coating Wrinkle_1006"));
	else if (nCode == 2000)	str.Format(_T("%s"), _T("MiniPinhole_2000"));
	else if (nCode == 2001)	str.Format(_T("%s"), _T("Pinhole_2001"));
	else if (nCode == 2002)	str.Format(_T("%s"), _T("Delamination_Crack_2002"));//돌출
	else if (nCode == 2003)	str.Format(_T("%s"), _T("Dent_2003"));
	else if (nCode == 2004)	str.Format(_T("%s"), _T("Dent_2004"));
	else if (nCode == 2005)	str.Format(_T("%s"), _T("MiniDent_2005"));
	else if (nCode == 2102)	str.Format(_T("%s"), _T("MiniDelamination_Crack_2102"));
	else if (nCode == 3000)	str.Format(_T("%s"), _T("MiniCrate_3000"));
	else if (nCode == 3001)	str.Format(_T("%s"), _T("Crate_3001"));
	else if (nCode == 3002)	str.Format(_T("%s"), _T("Crater_3002"));
	else if (nCode == 3003)	str.Format(_T("%s"), _T("Crack_3003"));
	else if (nCode == 4001)	str.Format(_T("%s"), _T("None Coating_4001"));
	else if (nCode == 5001)	str.Format(_T("%s"), _T("Island_5001"));
	else if (nCode == 5002)	str.Format(_T("%s"), _T("Wrinkle_5002"));
	else if (nCode == 6001)	str.Format(_T("%s"), _T("Connection Tape_6001"));
	else if (nCode == 6002)	str.Format(_T("%s"), _T("Delamination_Crack_6002"));
	else if (nCode == 6003)	str.Format(_T("%s"), _T("Crater_6003"));
	else if (nCode == 6004)	str.Format(_T("%s"), _T("NGTag_6004"));
	else if (nCode == 6005)	str.Format(_T("%s"), _T("Barcode Marks_6005")); // 검은색 테이프 자국
	else if (nCode == 7001)	str.Format(_T("%s"), _T("Pinhole_7001"));//절연 핀홀
	else if (nCode == 7002)	str.Format(_T("%s"), _T("Line_7002"));//절연 라인
//	else if (nCode == 7003)	str.Format(_T("%s"), _T("절연갭_7003"));
	//else if (nCode == 7004)	str.Format(_T("%s"), _T("Insulation Island_7002"));//절연 라인
	else if (nCode == 8001)	str.Format(_T("%s"), _T("Burr_8001"));
	else if (nCode == 8002) str.Format(_T("%s"), _T("Dross_8002"));
	else if (nCode == 8003) str.Format(_T("%s"), _T("Spatter_8003"));
	else if (nCode == 9001) str.Format(_T("%s"), _T("Scrab_9001"));//0930
	else if (nCode == 9001) str.Format(_T("%s"), _T("Scrab"));//0930
	else if (nCode == -1) str.Format(_T("%s"), _T("White OK"));
	else if (nCode == -2) str.Format(_T("%s"), _T("Black OK"));
	else if (nCode == -3) str.Format(_T("%s"), _T("Island OK"));
	else					str.Format(_T("%s"), _T("OK"));

	return str;
}
CString CInspectionCell::GetDefectName_G(int nCode)
{
	CString str;

	if (nCode == 1001)		str.Format(_T("%s"), _T("Line"));
	else if (nCode == 1002)	str.Format(_T("%s"), _T("Line"));
	else if (nCode == 1003)	str.Format(_T("%s"), _T("Scratch"));
	else if (nCode == 1004)	str.Format(_T("%s"), _T("Line"));
	else if (nCode == 1005)	str.Format(_T("%s"), _T("Heating Wrinkle"));
	else if (nCode == 1006)	str.Format(_T("%s"), _T("Coating Wrinkle"));
	else if (nCode == 2000)	str.Format(_T("%s"), _T("MiniPinhole"));
	else if (nCode == 2001)	str.Format(_T("%s"), _T("Pinhole"));
	else if (nCode == 2002)	str.Format(_T("%s"), _T("Delamination_Crack"));//돌출
	else if (nCode == 2003)	str.Format(_T("%s"), _T("Dent"));
	else if (nCode == 2004)	str.Format(_T("%s"), _T("Dent"));
	else if (nCode == 2005)	str.Format(_T("%s"), _T("MiniDent"));
	else if (nCode == 2102)	str.Format(_T("%s"), _T("MiniDelamination_Crack"));
	else if (nCode == 3000)	str.Format(_T("%s"), _T("MiniCrater"));
	else if (nCode == 3001)	str.Format(_T("%s"), _T("Crater"));
	else if (nCode == 3002)	str.Format(_T("%s"), _T("Crater"));
	else if (nCode == 3003)	str.Format(_T("%s"), _T("Crack"));
	else if (nCode == 4001)	str.Format(_T("%s"), _T("None Coating"));
	else if (nCode == 5001)	str.Format(_T("%s"), _T("Insulation Island"));
	else if (nCode == 5002)	str.Format(_T("%s"), _T("Wrinkle"));
	else if (nCode == 6001)	str.Format(_T("%s"), _T("Connection Tape"));
	else if (nCode == 6002)	str.Format(_T("%s"), _T("Delamination_Crack"));
	else if (nCode == 6003)	str.Format(_T("%s"), _T("Crater"));
	else if (nCode == 6004)	str.Format(_T("%s"), _T("NGTag"));
	else if (nCode == 6005)	str.Format(_T("%s"), _T("Barcode Marks")); // 검은색 테이프 자국
#ifdef _INSULATESURFACE_
	else if (nCode == 7001)	str = GetDefectName(DefectType::InsulationPinhole);//절연 핀홀
	else if (nCode == 7002)	str = GetDefectName(DefectType::InsulationLine);//절연 라인
	//else if (nCode == 7004)	str = GetDefectName(DefectType::InsulationIsland);//절연 아일랜드
#else
	else if (nCode == 7001)	str.Format(_T("%s"), _T("Pinhole"));//절연 핀홀
	else if (nCode == 7002)	str.Format(_T("%s"), _T("Line"));//절연 라인
#endif
//	else if (nCode == 7003)	str.Format(_T("%s"), _T("절연갭"));
	else if (nCode == 8001)	str.Format(_T("%s"), _T("Burr"));
	else if (nCode == 8002) str.Format(_T("%s"), _T("Dross"));
	else if (nCode == 8003) str.Format(_T("%s"), _T("Spatter"));
	else if (nCode == 9001) str.Format(_T("%s"), _T("Scrab"));//0930
	else if (nCode == 9001) str.Format(_T("%s"), _T("Scrab"));//0930
	else if (nCode == -1) str.Format(_T("%s"), _T("White OK"));
	else if (nCode == -2) str.Format(_T("%s"), _T("Black OK"));
	else if (nCode == -3) str.Format(_T("%s"), _T("Island OK"));
	else					str.Format(_T("%s"), _T("OK"));

	return str;
}

int CInspectionCell::CheckColor(int nType)
{
	int nret = 0;
	if (nType == 1001)		nret = 0;//	  거대라인");
	else if (nType == 1002)	nret = 0;//  "백색라인");
	else if (nType == 1003)	nret = 1;//  "라인긁힘");
	else if (nType == 1004)	nret = 0;//  "바운드라인");
	else if (nType == 1005)	nret = 0;//  "열주름");
	else if (nType == 1006)	nret = 0;//  "코팅주름");
	else if (nType == 2000)	nret = 0;//  "미니핀홀");
	else if (nType == 2001)	nret = 0;//  "핀홀");
	else if (nType == 2002)	nret = 0;//  "반짝이");//돌출
	else if (nType == 2003)	nret = 1;//  "찍힘");
	else if (nType == 2004)	nret = 0;//  "파임");
	else if (nType == 2005)	nret = 1;//  "미니찍힘");
	else if (nType == 2102)	nret = 0;//  "미니돌출");
	else if (nType == 3000)	nret = 1;//  "미니분화구");
	else if (nType == 3001)	nret = 1;//  "핀홀성분화구");
	else if (nType == 3002)	nret = 1;//  "흑색분화구");
	else if (nType == 3003)	nret = 1;//  "흑색크랙");
	else if (nType == 4001)	nret = 0;//  "미코팅");
	else if (nType == 5001)	nret = 1;//  "아일랜드");
	else if (nType == 5002)	nret = 1;//  "무지부주름");
	else if (nType == 6001)	nret = 0;//  "Tape");
	else if (nType == 6002)	nret = 0;//  "거대백불량");
	else if (nType == 6003)	nret = 1;//  "거대흑불량");
	else if (nType == 6004)	nret = 0;//  "엔지태그");
	else if (nType == 6005)	nret = 0;//  "엔지태그");
	else if (nType == 7001)	nret = 0;//  "절연핀홀");
	else if (nType == 7002)	nret = 0;//  "절연라인");
	else if (nType == 7003)	nret = 0;//  "절연갭");
	//else if (nType == 7004)	nret = 1;//  "절연아일랜드");
	else if (nType == 8001)	nret = 0;//  "Burr");
	else if (nType == 9001)	nret = 0;//  "Scrab");//0930
	else if (nType == -2)	nret = 1;//  "흑색");//220221
	return nret;
}


CRect CInspectionCell::SetBoundary2(CRect rt, CRect rt2)
{
	if (rt.left < rt2.left)
		rt.OffsetRect((rt2.left - rt.left), 0);
	if (rt.top < rt2.top)			rt.OffsetRect(0, rt2.top - rt.top);
	if (rt.right >= rt2.right)		rt.OffsetRect(rt2.right - rt.right, 0);
	if (rt.bottom >= rt2.bottom)	rt.OffsetRect(0, rt2.bottom - rt.bottom);
	return rt;
}

void CInspectionCell::ImageSelectCopy(unsigned char* fmSrc, unsigned char* fmDest, int nSrcW, int nSrcH, CRect rtArea, int nDestW, int nDestH, int nDestSttX, int nDestSttY)
{
	int x, y;
	CPoint ptOffset(0, 0);
	//	rtArea = pData->m_rectDefect[0][pData->m_nRealDefectCount];
	CRect rt1 = rtArea;
	if (rt1.Width() > nDestW || rt1.Height() > nDestH)	return;
	if (rt1.left < 0)			ptOffset.x = abs(rt1.left);
	if (rt1.right >= nSrcW)		ptOffset.x = nSrcW - rt1.right - 1;
	if (rt1.top < 0)			ptOffset.y = abs(rt1.top);
	if (rt1.bottom >= nSrcH)	ptOffset.y = nSrcH - rt1.bottom - 1;
	rt1.OffsetRect(ptOffset);

	if (rt1.bottom >= nSrcH) rt1.OffsetRect(0, nSrcH - rt1.bottom - 1);//0611
	for (int j = rt1.top; j < rt1.bottom && j >= 0 && j < m_imageSizeY && j < nSrcH; j++)//211103
	{
		for (int i = rt1.left; i < rt1.right && i >= 0 && i < m_imageSizeX && i < nSrcW; i++)//211103
		{
			x = (nDestSttX + i - rtArea.left);
			y = (nDestSttY + j - rtArea.top);
			if (0 <= x && x < nDestW && 0 <= y && y < nDestH)
				fmDest[x + nDestW * y] = fmSrc[i + nSrcW * j];
		}
	}
}
int CInspectionCell::AreaAve1(BYTE* fm, CRect rt, int nw, int* nave)
{
	double dsum = 0, dc = 0, dsum1 = 0, dc1 = 0;

	for (int y = rt.top, y1 = 0; y < rt.bottom; y++, y1++) {
		for (int x = rt.left, x1 = 0; x < rt.right; x++, x1++) {
			dsum += *(fm + y * nw + x); dc++;
			if (x1 > 0 && x1 < 4 && y1>0 && y1 < 4) { dsum1 += *(fm + y * nw + x); dc1++; }
		}
	}
	*nave = int(dsum1 / dc1);
	return int(dsum / dc);
}

void CInspectionCell::PrePareInsp()
{
#ifdef RESET_IMAGE_BUFFER_WHEN_START_INSPECTION
		ResetImageBuffer(true);
#endif
		m_cellResult.imagePosition = m_imagePosition;
		m_cellResult.isUp = m_isUp;
		m_cellResult.cellID = m_cellID;
		m_cellResult.ptOrigin = IPVM::Point_32f_C2(0, 0);
	
		m_cellResult.m_grabTime_ms = m_grabTime_ms;
		m_cellResult.m_inputTime = m_inputTime;
		m_cellResult.cellOffset = m_cellOffset;
		m_cellResult.cellSizePixel = m_cellSizePixel;
	
		m_cellResult.imageFull = m_inspectionImageFull;
		m_cellResult.imageBright = m_inspectionImageBright;
		m_cellResult.imageDark = m_inspectionImageDark;
	
		m_cellResult.m_inspectionStartTime_ms = m_inspectionStartTime_ms;
	
		m_cellResult.Clear();
	
		// result value reset
		m_cellResult.m_measureShoulderLine.Reset();
		m_cellResult.mismatchUm = 0;
	
		m_cellResult.inkAutoCalibError = 0;
		ResetResult();
#ifdef _ISI_
		ResetISI();		
#endif
}