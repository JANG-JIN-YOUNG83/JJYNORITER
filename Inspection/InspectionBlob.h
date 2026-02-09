#pragma once

#include "basedef.h"

#include "InspectionBase.h"
#include "InspectionBlobPara.h"

#include "Types/Point_32f_C2.h"
#include "Types//LineSeg_32f.h"
#include "Algorithm/AlgorithmObjectHandle.h"
#include "Algorithm/EdgeDetection.h"
#include "Algorithm/EdgeDetectionFilter.h"
#include "Algorithm/EdgeDetectionPara.h"
#include <vector>
#include <map>
#include "Gadget/TimeCheck.h"


class Database;


struct BlobData_C
{
	IPVM::Image_8u_C1* m_Originimage;				// 원본 이미지
	IPVM::Image_8u_C1* m_bufferimage;				// 이진화 이미지
	long				m_BlobNum;					// 블랍 카운트
													   
	//IPVM::Image_32s_C1	m_bloblabelimage;		// 블랍 라벨 이미지
	IPVM::Image_16u_C1	m_bloblabelimage;			// 블랍 라벨 이미지
	IPVM::BlobInfo* m_bufferBlobInfo;				// 블랍 인포
	IPVM::Rect			m_Rect;						// 검사 Rect?
	float				m_fAverage;					// 검사영역의 평균값

	long	m_InspectionArea;
	ISICF::enCategory m_category;
	ISICF::enDefectBrightness m_DefectColor;
	ISICF::enInspectionImageType m_ImageType;

	std::map<long, IPVM::Rect>* m_InspectionAreaRect;

public:
	BlobData_C() : m_bufferimage(nullptr), m_BlobNum(0), m_bufferBlobInfo(nullptr), m_fAverage(0.0f), m_InspectionAreaRect(nullptr) {}
	~BlobData_C() {}
	
	
	//void BlobData_C::SetBlobDataInfo(IPVM::Rect Rect, ISICF::enInspectionArea InspectionArea, ISICF::enDefectBrightness DefectColor, ISICF::enInspectionImageType ImageType, ISICF::enCategory category = ISICF::enCategory::CTG_NONE)
	void BlobData_C::SetBlobDataInfo(IPVM::Rect Rect, long InspectionArea, ISICF::enDefectBrightness DefectColor, ISICF::enInspectionImageType ImageType, ISICF::enCategory category = ISICF::enCategory::CTG_NONE)
	{
		m_Rect = Rect;
		m_InspectionArea = InspectionArea;
		m_DefectColor = DefectColor;
		m_ImageType = ImageType;
		m_category = category;
	}

	void BlobData_C::SetMap(std::map<long, IPVM::Rect>* map)
	{
		m_InspectionAreaRect = map;
	}
};
class CInspectionBlobDebugInfo
{
public:

	IPVM::Rect m_rtVHome[4];

	//자세히 찾은 Vhome point
	std::vector<IPVM::Rect> m_vecrtVhomeSearch;
	std::vector<IPVM::Point_32f_C2> m_vecVhomePoint;
	std::vector<IPVM::Point_32f_C2> m_vecVhomeCenterPoint;
	std::vector<IPVM::Point_32f_C3> m_vecDetialVhomePoint;
	std::vector<IPVM::Point_32f_C2> m_vecVHomeHeightDetailPoint;
	std::vector<IPVM::Rect> m_vecVHomeHeightDetailSearchArea;

	IPVM::Rect m_rtInsulationCuttingHeightTop;
	IPVM::Rect m_rtInsulationCuttingHeightBot;

	std::vector<IPVM::Rect> m_vecSeartchErrorVhome;	//2중 타발되어서 불필요한 영역에 있는 경우
	std::vector<IPVM::Rect> m_vecDoubleVhome;	//2중 타발되어서 불필요한 영역에 있는 경우
	std::vector<IPVM::Rect> m_vecErrorVhome;	//v home 검색이 잘못된 경우

	std::vector<IPVM::Rect> m_vecrtBoundrySearch;
	std::vector<IPVM::Rect> m_vecrtCellBoundrySearch;
	std::vector<IPVM::Rect> m_vecrtTabDetailSearch;

	std::vector<IPVM::Point_32f_C2> m_vecvHomeMakeUpperIgnorePoint;
	std::vector<IPVM::Rect> m_vecrtvHomeMakeUpper;

	std::vector<IPVM::Rect> m_vecrtShoulderSearch;
	std::vector<IPVM::Point_32f_C2> m_vecptShoulderPoints;
};

class CInspectionCustomBlobInfo
{
public:
	// 기본 IPVM blobinfo
	IPVM::BlobInfo sObjInfo[MAX_DEFECT_COUNT];
	
	// User 추가 Result Parameter
	float fEdgeEnergy[MAX_DEFECT_COUNT]							= { 0.f, };
	float fWidth[MAX_DEFECT_COUNT]								= { 0.f, };
	float fGVMin[MAX_DEFECT_COUNT]								= { 0.f, };
	float fGVMax[MAX_DEFECT_COUNT]								= { 0.f, };
	float fBoundaryDis[MAX_DEFECT_COUNT] = { 0.f, };
	float fGVAvg[MAX_DEFECT_COUNT]								= { 0.f, };
	float fBlackGVAvg[MAX_DEFECT_COUNT] = { 0.f, };
	float fWhiteGVAvg[MAX_DEFECT_COUNT] = { 0.f, };
	float fGVMassCenterAvg[MAX_DEFECT_COUNT]					= { 0.f, };
	IPVM::Point_32f_C2 ptMassCenter[MAX_DEFECT_COUNT]			= { IPVM::Point_32f_C2(0,0), };

	float fLength[MAX_DEFECT_COUNT]								= { 0.f, };
	float fLocus[MAX_DEFECT_COUNT]								= { 0.f, };
	float fThickness[MAX_DEFECT_COUNT]							= { 0.f, };
	float fAverageThickness[MAX_DEFECT_COUNT]					= { 0.f, };
	float fRatioMix[MAX_DEFECT_COUNT]							= { 0.f, };

	long nOver200GVPxCnt[MAX_DEFECT_COUNT]						= {0, };
    float fAngle[MAX_DEFECT_COUNT]								= {0.f, };
	float fDis2Edge[MAX_DEFECT_COUNT]							= {0.f,};

	float fEdgeEnergyWhite[MAX_DEFECT_COUNT]							= {0.f,};
	float fEdgeEnergyBlack[MAX_DEFECT_COUNT]							= {0.f,};
    float fEdgeEnergySharpness[MAX_DEFECT_COUNT]						= {0.f,};
	float fEdgeEnergySharpnessWhite[MAX_DEFECT_COUNT]						= {0.f,};
	float fEdgeEnergySharpnessBlack[MAX_DEFECT_COUNT]						= {0.f,};
	float fGVMedian[MAX_DEFECT_COUNT]						= {0.f,};
	float fGVStdev[MAX_DEFECT_COUNT]						= {0.f,};
    IPVM::Quadrangle_32f quardROI[MAX_DEFECT_COUNT] = {
        IPVM::Quadrangle_32f(IPVM::Point_32f_C2(0, 0), IPVM::Point_32f_C2(0, 0), IPVM::Point_32f_C2(0, 0), IPVM::Point_32f_C2(0, 0)),};
    
};

class CInspectionCell;
class Database;

class __INSPECTOIN_CLASS__ CInspectionBlob : public CProcessingBase
{
public:
	CInspectionBlob(CInspectionCell* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);
	virtual ~CInspectionBlob();

public:

	
	
	IPVM::Image_8u_C1 m_CropImage[100];

#if INSPECTION_MACHINE_UC1

#else
    IPVM::Image_8u_C1 m_CropImage2[100];
    IPVM::Image_8u_C1 m_CropImage3[100];
    IPVM::Image_8u_C1 m_CropImage4[100];

    IPVM::Image_8u_C1 m_EqualImage_Bright[100];
    IPVM::Image_8u_C1 m_EqualImage_Dark[100];
#endif



    BOOL m_bEqualImageFlag_Bright[100];
    BOOL m_bEqualImageFlag_Dark[100];

	virtual void PrepareDebugInfo(Inspection::DebugInfo* debugInfo) override;
	virtual bool ProcInspection(Inspection::DebugInfo* debugInfo) override;
	virtual void ProcInspectionEnd(Inspection::DebugInfo* debugInfo) override;

	bool ProcInspection(bool isLeft);
	
	virtual void LinkDataBase(bool bSave, Database &dbJob) override;
	virtual bool CopyDataBase(CProcessingBase* src) override;
	
	//long InspBlob(IPVM::Image_8u_C1 CopyImage,IPVM::Image_8u_C1 InspImage, IPVM::BlobInfo* psObjInfo, long nInspBlob, long nInspIllum);
    long InspBlob(IPVM::Image_8u_C1 CopyImage, IPVM::Image_8u_C1 InspImage, IPVM::Image_8u_C1 CopyImage2, IPVM::Image_8u_C1 InspImage2, CInspectionCustomBlobInfo* psObjInfo,
        long nInspBlob, long nInspIllum,  IPVM::Rect rtROI);
	
	std::vector<std::vector<HANDLE>> m_vecFlag_EqualizationItemReady;
	HANDLE m_Flag_EqualizationItemsReady[160];
	struct SEqualizationItems
    {
        CInspectionBlob* pMain;
        IPVM::Image_8u_C1 srcImage;
        IPVM::Rect rtInspArea;
        unsigned char target;
        float weight;
        long thesholdMax;
        long thesholdMin;
        long nCheckNum;
        SEqualizationItems(CInspectionBlob* _pMain, IPVM::Image_8u_C1& _srcimage, IPVM::Rect& _rtInspArea, const unsigned char& _target, const float& _weight,
            const long _thresholdMax, const long _thresholdMin, long _nCheckNum)
        {
            pMain = _pMain;
            srcImage = _srcimage;
            rtInspArea = _rtInspArea;
            target = _target;
            weight = _weight;
            thesholdMax = _thresholdMax;
            thesholdMin = _thresholdMin;
            nCheckNum = _nCheckNum;
		}
	};

	std::vector< std::vector<HANDLE>> m_vecFlag_InspectionItemsReady;
	std::vector<long> m_vecnBlobNum;
	HANDLE m_Flag_InspectionItemsReady[160];
	struct SInspectionItemsItem
	{
        
		CInspectionBlob* pMain;
		long nInspBlob;
		long nInspIllum;
		IPVM::Image_8u_C1 CopyImage;
		IPVM::Image_8u_C1 InspImage;

		IPVM::Image_8u_C1 CopyImage2;
		IPVM::Image_8u_C1 InspImage2;
		
		//IPVM::BlobInfo* psObjInfo;
		CInspectionCustomBlobInfo* psObjinfo;
		
		long nCheckNum;
        IPVM::Rect rtROI;
        SInspectionItemsItem(CInspectionBlob* _pMain, IPVM::Image_8u_C1 _CopyImage, IPVM::Image_8u_C1 _InspImage, IPVM::Image_8u_C1 _CopyImage2, IPVM::Image_8u_C1 _InspImage2,
            CInspectionCustomBlobInfo* _psObjInfo, long _nInspBlob, long _nInspIllum, long _nCheckNum, IPVM::Rect _rtROI)
		{
			pMain = _pMain;
			nInspBlob = _nInspBlob;
			nInspIllum = _nInspIllum;
			CopyImage = _CopyImage;
			InspImage = _InspImage;
			CopyImage2 = _CopyImage2;
			InspImage2 = _InspImage2;

			psObjinfo = _psObjInfo;
			nCheckNum = _nCheckNum;
            rtROI = _rtROI;
		}
	};

	//std::vector<IPVM::BlobInfo*> m_pvecsObjInfo;
	std::vector < CInspectionCustomBlobInfo*> m_pvecsObjInfo;

	std::vector<IPVM::Rect> m_vecrtCal;
	std::vector<SInspectionItemsItem*> m_item;
    std::vector<SEqualizationItems*> m_EqualItem;
	static UINT ThreadInspectionSubFunc(LPVOID pParam);
	static UINT ThreadEqualizationSubFunc(LPVOID pParam);
	void ThreadInspectionSub(SInspectionItemsItem* pInspItem);
    void ThreadEqualizationSub(SEqualizationItems* pEqualItem);
	void GetDefectProperty(const int& nCount, const bool bResize = false, const int nRate = 1, const IPVM::Image_8u_C1& ResizeImage = IPVM::Image_8u_C1(0, 0));
	void CalcAngle(const long SizeX_px, const long SizeY_px, const float fWidth_px, const float fLength_px, float& fRealAngle, float& fHalfImageAngle);
	void CalcPixel2mm(const IPVM::Rect& Rect, const float fHalfImageAngle, const float fLocus, const float fThickness, float& SIZE_X_mm, float& SIZE_Y_mm, float& WIDTH_mm, float& LENGTH_mm, float& RATIO_YX, float& RATIO_XY, float& RATIO_LW, float& RATIO_WL); // IPVM 짧은것은 Width, 긴것은 Length로 나오기 때문에 맞게 다시 계산하는 함수
	
	float GetSTD(const IPVM::Image_8u_C1& OriginImage, const IPVM::Rect& rtElectrode, const IPVM::Rect& Rect);

	void ImageProcessing_BoxFilter(IPVM::Image_8u_C1& imgSrc, IPVM::Rect& Rect, IPVM::Image_8u_C1& imgDst);

	std::vector<IPVM::Point_32s_C2> m_vecmasspt;
	//UI변수
	std::vector<IPVM::Point_32f_C2> vec_tempPoints;
	std::vector<IPVM::Rect> m_vecInspAreas;
	std::vector<float> m_vecTagDeviation;
	std::vector<IPVM::Rect> m_vecTagRect;


	//blob 위한것
	float* m_Width[BLOB_COUNT];
	float* m_Length[BLOB_COUNT];
	float* m_Locus[BLOB_COUNT];
	float* m_Thickness[BLOB_COUNT];
	float* m_MaxThickness[BLOB_COUNT];

	float* m_MaxGV[BLOB_COUNT];
	float* m_MinGV[BLOB_COUNT];
	float* m_AvgGV[BLOB_COUNT];
	float* m_Compact[BLOB_COUNT];
	IPVM::Point_32f_C2* m_massPoint[BLOB_COUNT];
	//BlobData m_BlobDatas[BLOB_COUNT];
public:

	CInspectionBlobPara m_para;
	CInspectionBlobDebugInfo	m_debugInfo;


	void BinarizationGreater_Mean(const IPVM::Image_8u_C1& srcimage, const IPVM::Rect& roi, const int& Th, IPVM::Image_8u_C1& dstimag, float& fAverage);

	void BinarizationLess_Mean(const IPVM::Image_8u_C1& srcimage, const IPVM::Rect& roi, const int& Th, IPVM::Image_8u_C1& dstimag, float& fAverage);
    void BinarizationLess_Mean_noRange(const IPVM::Image_8u_C1& srcimage, const IPVM::Rect& roi, const int& Th, IPVM::Image_8u_C1& dstimag, float& fAverage);
    void BinarizationGreater_Mean_noRange(const IPVM::Image_8u_C1& srcimage, const IPVM::Rect& roi, const int& Th, IPVM::Image_8u_C1& dstimag, float& fAverage);
	BlobData_C m_BlobDatas[20*4];
    void Equalization_TargetWeighted(IPVM::Image_8u_C1& srcimage, IPVM::Rect& rtInspArea, IPVM::Image_8u_C1& maskimage, const unsigned char& target, const float& weight, const unsigned char* arAverage);
    void Equalization_TargetWeighted(IPVM::Image_8u_C1& srcimage, IPVM::Rect& rtInspArea, const unsigned char& target, const float& weight, const unsigned char* arAverage);
    void Equalization_TargetWeighted(IPVM::Image_8u_C1& srcimage, IPVM::Rect& rtInspArea, const unsigned char& target, const float& weight);
	void Equalization_TargetWeighted(IPVM::Image_8u_C1& srcimage, IPVM::Rect& rtInspArea, const unsigned char& target, const float& weight, const long thresholdMax, const long thresholdMin);

private:
	CInspectionCell* m_parent;

	IPVM::Rect m_rectImageValid;
	IPVM::EdgeDetectionPara m_edgePara_Tilt[8];

	void FindShoulderLinePoints(const IPVM::Rect& roughTabRoi);

	std::vector<float> m_vecBlobTime;
    Inspection::DebugInfo* m_DebugInfo;
};
