#pragma once

#include "basedef.h"
#include "ProcessingItemDef.h"
#include "RejectInfo.h"

#include "Blob/IBlob.h"

#include <vector>
#include "Types/BlobInfo.h"
#include "Types/Image_8u_C1.h"
#include "Types/Image_32s_C1.h"
#include "Types/Rect_32f.h"
#include "Types/Quadrangle_32f.h"

#include "CISICellAllDefect.h"
#include "CISIDefectProperty.h"
#include "CISISpecTable.h"

#define BLOB_NUM_MAX			10000
#define	MAX_HUGEDEFECT		15
#define MOD_NUMBER			8
#define BAD_WIDTH			256
#define BAD_HEIGHT			256
#define MAX_SPLIT_DEFECT	5
#define MAX_FULL_DEFECT_COUNT	80

#define DT_WHITE	0
#define DT_DARK		1
#define DT_TOTAL	2

class Database;
class UnionCellResult;


class __INSPECTOIN_CLASS__ CProcessingBase
{
public:
	CProcessingBase(CProcessingBase* parent, const ProcItemCategory itemCategory, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);
	CProcessingBase(UnionCellResult& result, const ProcItemCategory itemCategory, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);
	virtual ~CProcessingBase();

public:
	virtual void PrepareDebugInfo(Inspection::DebugInfo* debugInfo) = 0;
	virtual bool ProcInspection(Inspection::DebugInfo* debugInfo) = 0;
	virtual void ProcInspectionEnd(Inspection::DebugInfo* debugInfo) = 0;
	virtual void LinkDataBase(bool bSave, Database &dbJob) = 0;
	virtual bool CopyDataBase(CProcessingBase* src) = 0;

	bool DoInspection(Inspection::DebugInfo* debugInfo = nullptr);
	void InitDebugInfo(Inspection::DebugInfo* debugInfo);
	void ResetImageBuffer(bool resetTapThreshold);
	void SetUpDown(bool isUp);
	void SetCellID(long cellID);
	void SetStartInspectionTime(float time_ms);
	void SetInspectionSplitImage(IPVM::Image_8u_C1 brightImage, IPVM::Image_8u_C1 darkImage);
	void SetBufferThreshold(IPVM::Image_8u_C1 image);
	void SetBufferThreshold2(IPVM::Image_8u_C1 image);
	void SetBufferThreshold3(IPVM::Image_8u_C1 image);
	void SetBufferThreshold4(IPVM::Image_8u_C1 image);
	void SetBufferThreshold5(IPVM::Image_8u_C1 image);
	void SetBufferThreshold6(IPVM::Image_8u_C1 image);
	void SetBufferLabel(IPVM::Image_32s_C1 label);
	//void SetBufferLabel_Burr_Only(IPVM::Image_32s_C1 label1, IPVM::Image_32s_C1 label2, IPVM::Image_32s_C1 label3, IPVM::Image_32s_C1 label4, IPVM::Image_32s_C1 label5, IPVM::Image_32s_C1 label6, IPVM::Image_32s_C1 label7, IPVM::Image_32s_C1 label8);
	void SetBufferBlob(IPVM::BlobInfo* blob);
	void SetTapeThreshold(IPVM::Image_8u_C1 image);
	bool MakeThresholdImage(const IPVM::Image_8u_C1& i_GrayImage, BYTE threshold, const IPVM::Rect& rtROI, IPVM::Image_8u_C1& i_ThresImage, bool removeBackground, bool origBackground);
	bool MakeReverseThresholdImage(const IPVM::Image_8u_C1& i_GrayImage, BYTE threshold, const IPVM::Rect& rtROI, IPVM::Image_8u_C1& i_ThresImage, bool removeBackground, bool origBackground);
	bool MakeBothThresholdImage(const IPVM::Image_8u_C1& i_GrayImage, BYTE lowThreshold, BYTE highThreshold, const IPVM::Rect& rtROI, IPVM::Image_8u_C1& i_ThresImage, bool removeBackground, bool origBackground);
	bool GetBlobs(const IPVM::Image_8u_C1& i_ThresImage, const IPVM::Rect& rtROI, const long minSize, const long maxSize, const long mergeDist, IPVM::Image_32s_C1& o_LabelImage, IPVM::BlobInfo* o_blobInfos, long& nBlobNum);
	bool GetBlobs_ISI(const IPVM::Image_8u_C1& i_ThresImage,
		const IPVM::Rect& rtROI,
		const long minSizeX,
		const long minSizeY,
		const long mergeDist,
		IPVM::Image_16u_C1& o_LabelImage,
		IPVM::BlobInfo* o_blobInfos,
		long& nBlobNum,
		const long& MaxBlobCount,
		float* fWidth,
		float* fLength,
		float* fLocus,
		float* fThickness,
		float* fMaxThickness,
		float* MinGV,
		float* MaxGV,
		float* AvgGV,
		IPVM::Point_32f_C2* massCenter,
		float* Compact,
		const IPVM::Image_8u_C1& origin,
		const bool bResize);
    float m_fCalcCos[180];
    float m_fCalcSin[180];
    float m_fCalcCosMin[180];
    float m_fCalcSinMin[180];

	void RotatePoints_Min(int num, float* i_pfx, float* i_pfy, float fangle, float* o_pfx, float* o_pfy, CPoint pt);
	void RotatePoints(int num, float* i_pfx, float* i_pfy, float fangle, float* o_pfx, float* o_pfy, CPoint pt);
    void RotatePoints(int num, float* i_pfx, float* i_pfy, float fangle, float* o_pfx, float* o_pfy);
    IPVM::Status GetBlobEdgeEnergy(const IPVM::Image_16u_C1& label,  const IPVM::Image_8u_C1& DarkImage, const IPVM::BlobInfo* blobInfo, const long blobCount,
        const IPVM::Image_8u_C1& srcImage, const long dataByteInterval, float* edgeEnergy, float* edgeEnergyWhite, float* edgeEnergyBlack);
	IPVM::Status GetBlobContrast(const IPVM::Image_16u_C1& label, const IPVM::Image_8u_C1& DarkImage, 
		const IPVM::BlobInfo* blobInfo, const long blobCount, const IPVM::Image_8u_C1& srcImage, 
		float fMinAreaContrast, const long dataByteInterval, float* keyContrast, float* avgContrast, 
		float* WhiteavgContrast,
		float* BlackavgContrast, float* minContrast, long nMAvgSize,IPVM::Point_32f_C2* massCenter,  float* massCenterAVG, float* ratioMix, long* over200GVpxCount, float* fMedianGV, float* fStdevGV);

	void GetObjectBoundary(const IPVM::Image_16u_C1& srcLabel, const IPVM::Rect& srcROI, const long checkLabel, std::vector<IPVM::Point_32s_C2>& edgeList);
    
	void GetObjectBoundary2(const IPVM::Image_16u_C1& LabelImage, const IPVM::Image_8u_C1& srcImage, const IPVM::Rect& srcROI, const long checkLabel, std::vector<IPVM::Point_32s_C2>& vecEdgePoint);
    void GetObjectLabel(const IPVM::Image_16u_C1& LabelImage, const IPVM::Rect& srcROI, const long checkLabel, std::vector<IPVM::Point_32s_C2>& vecEdgePoint);

	IPVM::Status GetBlobEdgeSharpness(const IPVM::Image_16u_C1& labelImage, const IPVM::Image_8u_C1& BinaryImage,
        const IPVM::Image_8u_C1& DarkImage, const IPVM::BlobInfo* blobInfo, const long blobCount,
        const long dataByteInterval, const IPVM::Image_8u_C1& srcImage,
        float* edgeEnergySharpnessWhite, float* edgeEnergySharpnessBlack);

	IPVM::Status GetBlobWidthLength(const IPVM::Image_16u_C1& labelImage, const IPVM::Image_8u_C1& BinaryImage, const IPVM::Image_8u_C1& DarkImage, const IPVM::BlobInfo* blobInfo, const long blobCount, const long dataByteInterval, float* blobWidth, float* blobLength, float* BoundaryLength,
        float* LTx, float* LTy, float* LBx, float* LBy, float* RTx, float* RTy, float* RBx, float* RBy, const IPVM::Image_8u_C1& srcImage, float* edgeEnergySharpness,
        const float fMag = 1.0);
	
	//Calcircularity() 블랍의 중심을 기준으로 306도 방향으로 최대 거리를 측정하여 그 값들의 표준 편차를 구하여 판단한다.
	float Calcircularity(const IPVM::Point_32f_C2& massPoint, const IPVM::Image_16u_C1& labelimage, const IPVM::Rect roi, const int& LabelNum);
	
	bool MergeBlobs(const IPVM::Image_8u_C1& i_ThresImage, const IPVM::Rect& rtROI, const long maxblobSize, IPVM::Image_32s_C1& o_LabelImage, IPVM::BlobInfo* o_blobInfos, long& nBlobNum);
	void ResetResult(II_RESULT_VALUE result = II_RESULT_VALUE::NOT_MEASURED);
	void SetPixelToUmX(double x);
	void SetPixelToUmY(double y);
	void SetCellCuttingMargin(long nMargin);
	void SetCellDesign(CELL_DESIGN design);
	CRect Rect2CRect(IPVM::Rect rect) { return CRect(rect.m_left, rect.m_top, rect.m_right, rect.m_bottom); };
	IPVM::Rect CRect2Rect(CRect crect) { return IPVM::Rect(crect.left, crect.top, crect.right, crect.bottom); };


	II_RESULT_VALUE	GetInspResult() const;
	LPCTSTR			GetInspResultString() const;

	void MakeLower(IPVM::Image_8u_C1& iImageSrc, IPVM::Rect rect, int threshold, IPVM::Image_8u_C1& iImageDst);
	void MakeUpper(IPVM::Image_8u_C1& iImageSrc, IPVM::Rect rect, int threshold, IPVM::Image_8u_C1& iImageDst, int nMakeUpperValue = 0);

	

public:
	IPVM::LoggerInterface& m_logger;
	IPVM::LoggerInterface& m_loggerResult;

	const ProcItemCategory	m_itemCategory;
	bool					m_isUp;
	long					m_cellID;
	float					m_inspectionStartTime_ms;
	
	//std::vector<long> m_vecOver200GVPxCount;
	//long m_over200GVPxCount;

	//isi
	CISISpecTable* m_isiSpecTable;
	CISICellAllDefect m_CellAllDefect;

	//Area
    long					m_nSide;
    void SetUpDown(long nSide);

	IPVM::Image_8u_C1		m_inspectionImageFull;		// Full Image (Mix Image)
	IPVM::Image_8u_C1		m_inspectionImageBright;	// Bright Image
	IPVM::Image_8u_C1		m_inspectionImageDark;		// Dark Image
	IPVM::Image_8u_C1*		m_inspectionImageCur;		// 선택된 영상
    IPVM::Image_8u_C1		m_inspectionImageBrightResize;
    IPVM::Image_8u_C1		m_inspectionImageDarkResize;
	IPVM::Image_8u_C1		m_inspectionBoundaryImageCur;
	IPVM::Image_8u_C1		m_inspectionElectrodeImageCur;
	IPVM::Image_8u_C1		m_inspectionScrabImageCur;

	IPVM::Image_8u_C1		m_bufferThreshold;
	IPVM::Image_8u_C1		m_bufferThreshold2;
	IPVM::Image_8u_C1		m_bufferThreshold3;
	IPVM::Image_8u_C1		m_bufferThreshold4;
	IPVM::Image_8u_C1		m_bufferThreshold5;
	IPVM::Image_8u_C1		m_bufferThreshold6;
	IPVM::Image_8u_C1		m_IslandBinaryImage;

	//IPVM::Image_8u_C1		m_bufferMerge;
	//IPVM::Image_8u_C1       m_fillImage;

	IPVM::Image_32s_C1		m_bufferLabel;
	//IPVM::Image_32s_C1		m_bufferLabelRoi1;				//Burr에서만 사용
	//IPVM::Image_32s_C1		m_bufferLabelRoi4;				//Burr에서만 사용
	//IPVM::Image_32s_C1		m_bufferLabelRoiLeft;			//Burr에서만 사용
	//IPVM::Image_32s_C1		m_bufferLabelRoiRight;			//Burr에서만 사용

	//IPVM::Image_32s_C1		m_bufferLabelRoi2;				//Burr에서만 사용
	//IPVM::Image_32s_C1		m_bufferLabelRoi1_Electrode;	//Burr에서만 사용
	//IPVM::Image_32s_C1		m_bufferLabelRoi3;				//Burr에서만 사용
	//IPVM::Image_32s_C1		m_bufferLabelRoi4_Electrode;	//Burr에서만 사용

	IPVM::BlobInfo*			m_bufferBlob;
	IPVM::Image_8u_C1		m_tapeThreshold;
	
	
	CIBlob* m_iBlob;
	SBlobInfo* m_psBlobInfo;

	long					m_imageSizeX;
	long					m_imageSizeY;

	REJECT_INFO				m_rejectInfo;
	std::vector <bool>		m_vecIsNoTabRect;
	II_RESULT_VALUE			m_resultValue;

	double					m_pixelToUmX;
	double					m_pixelToUmY;

	CELL_DESIGN				m_cellDesign;
	long					m_nCellCuttingMargin;

	UnionCellResult& m_unionResult;
};

class __INSPECTOIN_CLASS__ CInspectionBase : public CProcessingBase
{
public:
	CInspectionBase(CProcessingBase* parent, const ProcItemCategory itemCategory, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);
	virtual ~CInspectionBase();

	virtual bool IsInspectionEnabled() const = 0;
	virtual void ResetInspection() = 0;
};
