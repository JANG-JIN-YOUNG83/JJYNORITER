#pragma once
#include "basedef.h"

#include "ParaBool.h"
#include "ParaFloat.h"
#include "ParaLong.h"
#include "ParaByte.h"

#include <array>

enum enParaEdgeDetection{
	HalfFilterLength,
	GaussianDeviation,
	EdgeType,
	SearchDirection,
	FindFirstEdge,
	EdgeThreshold,
	RansacThreshold,
	DetectCount,
	ImageTypeDark,
};
namespace Base
{
	class __BASE_CLASS__ ParaEdgeDetection
	{
	public:
		ParaEdgeDetection(LPCTSTR name);
		ParaEdgeDetection(LPCTSTR name, long nHalfFilterLength, float fGaussianDeviation, bool bFirstEdge, long nDetectType, long nSearchDirection, float fEdgeThresholdfloat ,float fRansacThreshold=999, long nDetectCount=1, long m_ImageTypeDark = 1, long nEdgeDetectMode = 0);
		~ParaEdgeDetection();

		//bool Link(bool save, Database& db);
		bool Link(bool save, Database& db);

		//Base::ParaLong* m_HalfFilterLength;
		//Base::ParaFloat* m_GaussianDeviation;
		//Base::ParaBool* m_FindFirstEdge;		// Default : true
		//Base::ParaLong* m_DetectType;			// Default : IPVM::EdgeType::Rising (1)
		//Base::ParaLong* m_SearchDirection;		// Default : SearchDirection::ToRight (1)
		//Base::ParaFloat* m_EdgeThreshold;		// Default : 1.f

		long  m_HalfFilterLength;
		float m_GaussianDeviation;
		bool  m_FindFirstEdge;
		long  m_DetectType;
		long  m_SearchDirection;
		float m_EdgeThreshold;

		float m_RansacThreshold;
		long m_DetectCount;

		long m_ImageTypeDark;

		long m_EdgeDetectMode;

		void SetParameter(IPVM::EdgeDetectionPara& edgePara,IPVM::EdgeDetectionFilter& edgeFilter) const;
	private:
		int nCount;
		CString m_name;
		std::array<CString,9> m_ArrName;
	};
}
