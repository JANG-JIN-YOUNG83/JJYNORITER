#pragma once

#include "basedef.h"

#include <afxmt.h>

namespace Share
{

	enum enOPerror
	{
		OP_NO_ERROR = 1,
		OP_ERROR_RECIPE_NAME_MISMATCH = 2,
		OP_ERROR_GRAB_OUTLINE = 3,
	};

	class __VISION_HOST_SHARE_CLASS__ OperatorInfo : public CommonShare::BaseObject
	{
	public:
		OperatorInfo();
		~OperatorInfo();

		void DataCountSetZero();
		void GetResultCount(int* TotalCount, int* ngCount, int* okcount, int* Invalid, int* DDTotalCount, int* DimensionCount, int* DefectCount);

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		void SetNGReasonCount(CString judge, BOOL bDimensionNG, BOOL bDefectNG, BOOL bInvalid, BOOL bBurr);

		CMutex m_mutex_JDCount;		//저지 카운트
		CMutex m_mutex_DD_Count;		//데이터

		int nOperatorMode;		//

		//비전과 통신에 사용하는 변수
		CString strReceipeName;		//RUN_AUTO
		CString strEquipmentID;		//RUN_AUTO

		CString strLotID;			//LOT_START
		CString strRollID;			//LOT_START
		CString strMapDataPath;			//LOT_START

		int nEnableNgMarkPrint;

		BOOL bFeeder;	//0:A, 1:B

		int nStabilizationCount;
		int nContinuePitchCount;
		int nContinueWidthCount;
		int nContinueSideRightCount;
		int nContinueSideLeftCount;
		//int nContinueVHomePitchCount;
		int nContinueLengthCount;

		double dImageSaveRatioX; //X축 이미지 저장비율
		double dImageSaveRatioY; //Y축 이미지 저장비율

		//비전과 사용하지 않고 혼자만 보는 변수
		int nModel_index;
		CString strReceipePath;

		//작업수량은 여기에 추가하고 비전과 통신에서는 사용하지 않는다.
		int		nTotalCount;
		int		nokCount;
		int 	nngCount;

		int		nDimensionNgCount;
		int		nDefectNgCount;
		int		nDDTotalCount;
		int		nInvalid;

		int		nDummyCount;

		int		nBurrCount;

		CMutex mutexJD;
	};
}
