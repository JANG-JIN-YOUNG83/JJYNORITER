#include "../basedef.h"
#pragma once

enum enDefectidx
{
	SCRATCH = 1003,
	PINHOLE = 2001,

	PINHOLE_MINI = 2000,
	PROTRUSION = 2002,
	PROTRUSION_MINI = 2102,

	CRATER_MINI = 3000,
	CRATER = 3002,

	DENT = 2003,
	DENT_MINI = 2005,
};

struct stSpecInfo
{
	stSpecInfo()
	{
		m_dMin_X = 0.0f;
		m_dMin_Y = 0.0f;

		m_dX = 0.0f;
		m_dY = 0.0f;

		m_bMini_Enable = false;
		m_dMini_X = 0.0f;
		m_dMini_Y = 0.0f;

		m_dCompactness = 0.0f;
		m_dBlackPercent = 0.0f;

		m_dAngle = 0.0f;
	}

	void set(double dSizeX, double dSizeY, double dMinX, double dMinY, bool bMiniEnable, double dSize_mini_X, double dSize_mini_Y, int nThreshold, int dBlackPercent, double dCompactness, double dAngle)
	{
		m_dX = dSizeX;
		m_dY = dSizeY;

		m_dMin_X = dMinX;
		m_dMin_Y = dMinY;

		m_bMini_Enable = bMiniEnable;
		m_dMini_X = dSize_mini_X;
		m_dMini_Y = dSize_mini_Y;

		m_nThreshold = nThreshold;

		m_dBlackPercent = dBlackPercent;
		m_dCompactness = dCompactness;
		m_dAngle = dAngle;
	}

	double m_dMin_X;		//최소 검출 사이즈 ( 0r 조건에서 사용됨 )
	double m_dMin_Y;

	double m_dX;			//검출 스펙
	double m_dY;

	bool   m_bMini_Enable;	//미니 검사 유무
	double m_dMini_X;		//미니 검출 스펙
	double m_dMini_Y;

	int	   m_nThreshold;

	double m_dBlackPercent;
	double m_dCompactness;
	double m_dAngle;
};

struct stSmallDefect
{
	stSmallDefect()
	{
		init();
	}

	void init()
	{
		m_bDefect_bProtrution_Mini  = m_bDefect_bPinhole_Mini = m_bDefect_bCrater_Mini = m_bDefect_bDent_Mini = false;
	}
		
	bool m_bDefect_bProtrution_Mini;
	bool m_bDefect_bPinhole_Mini;
	bool m_bDefect_bCrater_Mini;
	bool m_bDefect_bDent_Mini;

	int GetResult_Mini(int nBrightDefect)
	{
		/*if (-1 == nBrightDefect)
		{
			if (true == m_bDefect_bPinhole_Mini)
				return enDefectidx::PINHOLE_MINI;

			if (true == m_bDefect_bProtrution_Mini)
				return enDefectidx::PROTRUSION_MINI;
		}
		else
		{
			if (true == m_bDefect_bCrater_Mini)
				return enDefectidx::CRATER_MINI;

		}*/

		if (true == m_bDefect_bPinhole_Mini)
			return enDefectidx::PINHOLE_MINI;

		if (true == m_bDefect_bProtrution_Mini)
			return enDefectidx::PROTRUSION_MINI;

		if (true == m_bDefect_bCrater_Mini)
			return enDefectidx::CRATER_MINI;

		if (true == m_bDefect_bDent_Mini)
			return enDefectidx::DENT_MINI;

		return nBrightDefect;
	}
};

class __INSPECTOIN_CLASS__ Classification
{
public:
	Classification();
	CString GetDefectName(int nCode);


	//result
	void ResetMiniResult();
	stSmallDefect m_result_mini;
	
	bool GetResult_Mini(int nBrightDefect, CString& strType, int& nret);

	//핀홀
	stSpecInfo m_spec_pinhole;
	
	stSpecInfo m_spec_crater;

	stSpecInfo m_spec_Scratch;

	stSpecInfo m_spec_Dent;

	//Protrution(돌출)
	stSpecInfo m_spec_protrution;

	void SetProtrutionSpec(double dSpecX, double dSpecY, double dSpec_Min_X, double dSpec_Min_Y, double dSpec_Mini_X, double dSpec_Mini_Y, int ProtrusionThreshold, BOOL bMiniEnable);
	void SetPinholeSpec(double dSpecX, double dSpecY, double dSpec_Min_X, double dSpec_Min_Y, double dSpec_Mini_X, double dSpec_Mini_Y, int ProtrusionThreshold, BOOL bMiniEnable);
	void SetDentSpec(double dSpecX, double dSpecY, double dSpec_Min_X, double dSpec_Min_Y, double dSpec_Mini_X, double dSpec_Mini_Y, int DentThreshold, BOOL bMiniEnable);
	void SetCraterSpec(double dSpecX, double dSpecY, double dSpec_Min_X, double dSpec_Min_Y, double dSpec_Mini_X, double dSpec_Mini_Y, int ProtrusionThreshold, BOOL bMiniEnable, double dBlackPercent, double dCompactness);
	void SetScratchSpec(double dSpecX, double dSpecY, double dSpec_Min_X, double dSpec_Min_Y, int nThreshold, double dangle);



	BOOL isProtrution(double dSizeX, double dSizeY, int threshold, BOOL bifAND, BOOL bifAND_Mini, CString& strType, int& nret);
	BOOL isPinhole(double dSizeX, double dSizeY, int threshold, BOOL bifAND, BOOL bifAND_Mini, CString& strType, int& nret);
	BOOL isPinhole_ratio(double dSizeX, double dSizeY, double dratio, double dSpec_ratio, CString& strType, int& nret);

	BOOL isDent(double dSizeX, double dSizeY, int threshold, BOOL bifAND, BOOL bifAND_Mini, CString& strType, int& nret);
	BOOL isCrater(double dSizeX, double dSizeY, int threshold, double dBlackPercent, BOOL bifAND, BOOL bifAND_Mini, CString& strType, int& nret);
	BOOL isCrater_Compactness(double dSizeX, double dSizeY, int threshold, double dBlackPercent, double dCompactness, BOOL bifAND, BOOL bifAND_Mini, CString& strType, int& nret);
	
	BOOL isTinyScratch(const double dWidth, const double dSpecWidth, const int nMinimumGrayValue, const int nSpec_MinimumGrayValue, CString& strType, int& nret);
	
	BOOL CraterFilterMinimumGrayValue(const int SITE, const double dWidth, const double dSpecWidth, const int nMinimumGrayValue, const int nSpec_MinimumGrayValue);

	BOOL ScratchFilter_WidthLength(const int SITE, const double dWidth, const double dLength, const double dSpec_Ratio);
	BOOL isScratch(const int SITE, double dSizeX, double dSizeY, double dSpecTinyWidth,int threshold, double dAngle, int check, BOOL bifAND, CString& strType, int& nret);


	BOOL isValid(bool bifAND, 
				double dSizeX, 
				double dSizeY, 
				double dSpecX, 
				double dSpecY, 
				double dSpec_Min_X, 
				double dSpec_Min_Y, 
				int nTh, 
				int nSpecTh);


	//2023년 5월 10일까지는 아래처럼 th값 보다 큰것들만 보고 만들었다. 그러나 다른 형태가 나타나기 시작했다.
	BOOL isValid_AND(double dSizeX, double dSizeY, double dSpecX, double dSpecY, int nTh, int nSpecTh);
	BOOL isValid_OR(double dSizeX, double dSizeY, double dSpecX, double dSpecY, double dSpec_Min_X, double dSpec_Min_Y, int nTh, int nSpecTh);


	template <typename T>
	BOOL OR(T dSizeX, T dSizeY, T dSpecX, T dSpecY, T dSpec_Min_X, T dSpec_Min_Y)
	{
		if ((dSizeX >= dSpec_Min_X && dSizeY >= dSpec_Min_Y) &&
			(dSizeX >= dSpecX || dSizeY >= dSpecY))
		{
			return true;
		}

		return false;
	}

	template <typename T>
	BOOL MINIMUM(T fMinimum, T fSpec_Minimum)
	{
		if (fMinimum < fSpec_Minimum)
		{
			return true;
		}

		return false;
	}

};
