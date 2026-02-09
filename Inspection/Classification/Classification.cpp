#include "stdafx.h"

#include "Classification.h"
#include "../../IBCI/SystemParameter.h"



Classification::Classification()
{
	
}


CString Classification::GetDefectName(int nCode)
{
	CString str;

	if (nCode == 1001)					str.Format(_T("%s"), _T("Line"));
	else if (nCode == 1002)				str.Format(_T("%s"), _T("Line"));
	else if (nCode == 1003)				str.Format(_T("%s"), _T("Scratch"));
	else if (nCode == 1004)				str.Format(_T("%s"), _T("Line"));
	else if (nCode == 1005)				str.Format(_T("%s"), _T("Heating Wrinkle"));
	else if (nCode == 1006)				str.Format(_T("%s"), _T("Coating Wrinkle"));
	else if (nCode == PINHOLE_MINI)		str.Format(_T("%s"), _T("Mini Pinhole"));
	else if (nCode == PINHOLE)			str.Format(_T("%s"), _T("Pinhole"));
	else if (nCode == 2002)				str.Format(_T("%s"), _T("Protrusion"));//돌출
	else if (nCode == 2003)				str.Format(_T("%s"), _T("Dent"));
	else if (nCode == 2004)				str.Format(_T("%s"), _T("Dent"));
	else if (nCode == 2005)				str.Format(_T("%s"), _T("Mini Dent"));
	else if (nCode == PROTRUSION_MINI)	str.Format(_T("%s"), _T("Mini Protrusion"));
	else if (nCode == CRATER_MINI)		str.Format(_T("%s"), _T("Mini Crater"));
	else if (nCode == 3001)				str.Format(_T("%s"), _T("Crater"));
	else if (nCode == CRATER)			str.Format(_T("%s"), _T("Crater"));
	else if (nCode == 3003)				str.Format(_T("%s"), _T("Crack"));
	else if (nCode == 4001)				str.Format(_T("%s"), _T("None Coating"));
	else if (nCode == 5001)				str.Format(_T("%s"), _T("Insulation Island"));
	else if (nCode == 5002)				str.Format(_T("%s"), _T("Wrinkle"));
	else if (nCode == 6001)				str.Format(_T("%s"), _T("Connection Tape"));
	else if (nCode == 6002)				str.Format(_T("%s"), _T("Protrusion"));
	else if (nCode == 6003)				str.Format(_T("%s"), _T("Crater"));
	else if (nCode == 6004)				str.Format(_T("%s"), _T("NGTag"));
#ifdef _INSULATESURFACE_
	else if (nCode == 7001)				str.Format(_T("%s"), _T("InsulationPinhole"));//절연 핀홀
	else if (nCode == 7002)				str.Format(_T("%s"), _T("InsulationLine"));//절연 라인
	//else if (nCode == 7004)				str.Format(_T("%s"), _T("Insulation Island"));//절연 아일랜드
#else
	else if (nCode == 7001)	str.Format(_T("%s"), _T("Pinhole"));//절연 핀홀
	else if (nCode == 7002)	str.Format(_T("%s"), _T("Line"));//절연 라인
#endif
	else if (nCode == 8001)				str.Format(_T("%s"), _T("Burr"));
	else if (nCode == 8002)				str.Format(_T("%s"), _T("Dross"));
	else if (nCode == 8003)				str.Format(_T("%s"), _T("Spatter"));
	else if (nCode == 9001)				str.Format(_T("%s"), _T("Scrab"));//0930
	else								str.Format(_T("%s"), _T("OK"));

	return str;
}

void Classification::ResetMiniResult()
{
	m_result_mini.init();
}

bool Classification::GetResult_Mini(int nBrightDefect, CString& strType, int& nret)
{
	nret = m_result_mini.GetResult_Mini(nBrightDefect);
	strType = GetDefectName(nret);

	if (0 <= nret)
		return true;
	else
		return false;

}


BOOL Classification::isValid(bool bifAND,
	double dSizeX,
	double dSizeY,
	double dSpecX,
	double dSpecY,
	double dSpec_Min_X,
	double dSpec_Min_Y,
	int nTh,
	int nSpecTh)
{
	if (true == bifAND)
	{
		return isValid_AND(dSizeX, dSizeY, dSpecX, dSpecY, nTh, nSpecTh);
	}
	else
	{
		return isValid_OR(dSizeX, dSizeY, dSpecX, dSpecY, dSpec_Min_X, dSpec_Min_Y, nTh, nSpecTh);
	}

}





BOOL Classification::isValid_AND(double dSizeX, double dSizeY, double dSpecX, double dSpecY, int nTh, int nSpecTh)
{
	if ((dSizeX >= dSpecX && 
		dSizeY >= dSpecY) &&
		nTh >= nSpecTh)
	{
		return true;
	}
	
	return false;
}

BOOL Classification::isValid_OR(double dSizeX, double dSizeY, double dSpecX, double dSpecY, double dSpec_Min_X, double dSpec_Min_Y, int nTh, int nSpecTh)
{
	if ((dSizeX >= dSpec_Min_X && dSizeY >= dSpec_Min_Y) &&
		(dSizeX >= dSpecX || dSizeY >= dSpecY) &&
		nTh >= nSpecTh)
	{
		return true;
	}

	return false;
}


BOOL Classification::isProtrution(double dSizeX, double dSizeY, int threshold, BOOL bifAND, BOOL bifAND_Mini, CString& strType, int& nret)
{

	if (true == isValid(bifAND, dSizeX, dSizeY, m_spec_protrution.m_dX, m_spec_protrution.m_dY, m_spec_protrution.m_dMin_X, m_spec_protrution.m_dMin_Y, threshold, m_spec_protrution.m_nThreshold))
	{
		nret = PROTRUSION;
		strType = GetDefectName(nret);

		return true;
	}

	if (true == m_spec_protrution.m_bMini_Enable)
	{
		if (true == isValid(
			bifAND_Mini,
			dSizeX,
			dSizeY,
			m_spec_protrution.m_dMini_X,
			m_spec_protrution.m_dMini_Y,
			min(m_spec_protrution.m_dMin_X, m_spec_protrution.m_dMini_X),
			min(m_spec_protrution.m_dMin_Y, m_spec_protrution.m_dMini_Y),
			threshold,
			m_spec_protrution.m_nThreshold))
		{
			//마지막에 아무것도 없을때 미니로 판정해야 해서 nret에 값을 넣지 않는다. 
			m_result_mini.m_bDefect_bProtrution_Mini = true;
			return true;
		}
	}

	return false;
}

void Classification::SetScratchSpec(double dSpecX, double dSpecY, double dSpec_Min_X, double dSpec_Min_Y, int nThreshold, double dangle)
{
	m_spec_Scratch.set(dSpecX, dSpecY, dSpec_Min_X, dSpec_Min_Y, 0, 0, 0, nThreshold, 0, 0.0f, dangle);
}

void Classification::SetProtrutionSpec(double dSpecX, 
	double dSpecY, 
	double dSpec_Min_X, 
	double dSpec_Min_Y, 
	double dSpec_Mini_X, 
	double dSpec_Mini_Y, 
	int ProtrusionThreshold, 
	BOOL bMiniEnable)
{
	m_spec_protrution.set(dSpecX, dSpecY, dSpec_Min_X, dSpec_Min_Y, bMiniEnable, dSpec_Mini_X, dSpec_Mini_Y, ProtrusionThreshold, 0, 0.0f, 0);
}

void Classification::SetPinholeSpec(double dSpecX,
	double dSpecY,
	double dSpec_Min_X,
	double dSpec_Min_Y,
	double dSpec_Mini_X,
	double dSpec_Mini_Y,
	int ProtrusionThreshold,
	BOOL bMiniEnable)
{
	m_spec_pinhole.set(dSpecX, dSpecY, dSpec_Min_X, dSpec_Min_Y, bMiniEnable, dSpec_Mini_X, dSpec_Mini_Y, ProtrusionThreshold, 0, 0.0f, 0);
}

void Classification::SetCraterSpec(double dSpecX,
	double dSpecY,
	double dSpec_Min_X,
	double dSpec_Min_Y,
	double dSpec_Mini_X,
	double dSpec_Mini_Y,
	int ProtrusionThreshold,
	BOOL bMiniEnable,
	double dBlackPercent,
	double dCompactness)
{
	m_spec_crater.set(dSpecX, dSpecY, dSpec_Min_X, dSpec_Min_Y, bMiniEnable, dSpec_Mini_X, dSpec_Mini_Y, ProtrusionThreshold, dBlackPercent, dCompactness, 0);
}

void Classification::SetDentSpec(double dSpecX,
	double dSpecY,
	double dSpec_Min_X,
	double dSpec_Min_Y,
	double dSpec_Mini_X,
	double dSpec_Mini_Y,
	int DentThreshold,
	BOOL bMiniEnable)
{
	m_spec_Dent.set(dSpecX, dSpecY, dSpec_Min_X, dSpec_Min_Y, bMiniEnable, dSpec_Mini_X, dSpec_Mini_Y, DentThreshold, 0, 0.0f, 0);
}

BOOL Classification::isPinhole_ratio(double dSizeX, double dSizeY, double dratio, double dSpec_ratio, CString& strType, int& nret)
{
	if(dSizeX >= m_spec_pinhole.m_dX && dSizeY >= m_spec_pinhole.m_dY && dratio < dSpec_ratio)
	{
		nret = PROTRUSION;
		strType = GetDefectName(nret);

		return true;
	}

	if (true == m_spec_pinhole.m_bMini_Enable)
	{
		if (dSizeX >= m_spec_pinhole.m_dMini_X && dSizeY >= m_spec_pinhole.m_dMini_Y && dratio < dSpec_ratio)
		{
			//마지막에 아무것도 없을때 미니로 판정해야 해서 nret에 값을 넣지 않는다. 
			m_result_mini.m_bDefect_bPinhole_Mini = true;

			return true;
		}
	}

	return false;
}

BOOL Classification::isPinhole(double dSizeX, double dSizeY, int threshold, BOOL bifAND, BOOL bifAND_Mini, CString& strType, int& nret)
{
	if (true == isValid(bifAND, dSizeX, dSizeY, m_spec_pinhole.m_dX, m_spec_pinhole.m_dY, m_spec_pinhole.m_dMin_X, m_spec_pinhole.m_dMin_Y, threshold, m_spec_pinhole.m_nThreshold))
	{
		nret = PINHOLE;
		strType = GetDefectName(nret);

		return true;
	}

	if (true == m_spec_pinhole.m_bMini_Enable)
	{
		if (true == isValid(
			bifAND_Mini,
			dSizeX,
			dSizeY,
			m_spec_pinhole.m_dMini_X,
			m_spec_pinhole.m_dMini_Y,
			min(m_spec_pinhole.m_dMin_X, m_spec_pinhole.m_dMini_X),
			min(m_spec_pinhole.m_dMin_Y, m_spec_pinhole.m_dMini_Y),
			threshold,
			m_spec_pinhole.m_nThreshold))
		{
			//마지막에 아무것도 없을때 미니로 판정해야 해서 nret에 값을 넣지 않는다. 
			m_result_mini.m_bDefect_bPinhole_Mini = true;

			return true;
		}
	}

	return false;
}
BOOL Classification::isDent(double dSizeX, double dSizeY, int threshold, BOOL bifAND, BOOL bifAND_Mini, CString& strType, int& nret)
{
	if (true == isValid(bifAND, dSizeX, dSizeY, m_spec_Dent.m_dX, m_spec_Dent.m_dY, m_spec_Dent.m_dMin_X, m_spec_Dent.m_dMin_Y, threshold, m_spec_Dent.m_nThreshold))
	{
		nret = DENT;
		strType = GetDefectName(nret);

		return true;
	}

	if (true == m_spec_Dent.m_bMini_Enable)
	{
		if (true == isValid(
			bifAND_Mini,
			dSizeX,
			dSizeY,
			m_spec_Dent.m_dMini_X,
			m_spec_Dent.m_dMini_Y,
			min(m_spec_Dent.m_dMin_X, m_spec_Dent.m_dMini_X),
			min(m_spec_Dent.m_dMin_Y, m_spec_Dent.m_dMini_Y),
			threshold,
			m_spec_Dent.m_nThreshold))
		{
			//마지막에 아무것도 없을때 미니로 판정해야 해서 nret에 값을 넣지 않는다. 
			m_result_mini.m_bDefect_bDent_Mini = true;

			return true;
		}
	}

	return false;
}

BOOL Classification::isCrater(double dSizeX, double dSizeY, int threshold, double dBlackPercent, BOOL bifAND, BOOL bifAND_Mini, CString& strType, int& nret)
{
	
	if (true == isValid(bifAND, dSizeX, dSizeY, m_spec_crater.m_dX, m_spec_crater.m_dY, m_spec_crater.m_dMin_X, m_spec_crater.m_dMin_Y, threshold, m_spec_crater.m_nThreshold))
	{
		if (dBlackPercent > m_spec_crater.m_dBlackPercent)
		{
			nret = CRATER;
			strType = GetDefectName(nret);

			return true;
		}
	}

	if (true == m_spec_crater.m_bMini_Enable)
	{
		if (true == isValid(
			bifAND_Mini,
			dSizeX,
			dSizeY,
			m_spec_crater.m_dMini_X,
			m_spec_crater.m_dMini_Y,
			min(m_spec_crater.m_dMin_X, m_spec_crater.m_dMini_X),
			min(m_spec_crater.m_dMin_Y, m_spec_crater.m_dMini_Y),
			threshold,
			m_spec_crater.m_nThreshold))
		{
			if (dBlackPercent > m_spec_crater.m_dBlackPercent)
			{
				//마지막에 아무것도 없을때 미니로 판정해야 해서 nret에 값을 넣지 않는다. 
				m_result_mini.m_bDefect_bCrater_Mini = true;
				return true;
			}
		}
	}

	return false;
}

BOOL Classification::isCrater_Compactness(double dSizeX, double dSizeY, int threshold, double dBlackPercent, double dCompactness, BOOL bifAND, BOOL bifAND_Mini, CString& strType, int& nret)
{

	if (true == isValid(bifAND, dSizeX, dSizeY, m_spec_crater.m_dX, m_spec_crater.m_dY, m_spec_crater.m_dMin_X, m_spec_crater.m_dMin_Y, threshold, m_spec_crater.m_nThreshold))
	{
		if (dBlackPercent > m_spec_crater.m_dBlackPercent && dCompactness > m_spec_crater.m_dCompactness)
		{
			nret = CRATER;
			strType = GetDefectName(nret);

			return true;
		}
	}

	if (true == m_spec_crater.m_bMini_Enable)
	{
		if (true == isValid(
			bifAND_Mini,
			dSizeX,
			dSizeY,
			m_spec_crater.m_dMini_X,
			m_spec_crater.m_dMini_Y,
			min(m_spec_crater.m_dMin_X, m_spec_crater.m_dMini_X),
			min(m_spec_crater.m_dMin_Y, m_spec_crater.m_dMini_Y),
			threshold,
			m_spec_crater.m_nThreshold))
		{
			if (dBlackPercent > m_spec_crater.m_dBlackPercent && dCompactness > m_spec_crater.m_dCompactness)
			{
				//마지막에 아무것도 없을때 미니로 판정해야 해서 nret에 값을 넣지 않는다. 
				m_result_mini.m_bDefect_bCrater_Mini = true;
				return true;
			}
		}
	}

	return false;
}

BOOL Classification::CraterFilterMinimumGrayValue(const int SITE, const double dWidth, const double dSpecWidth, const int nMinimumGrayValue, const int nSpec_MinimumGrayValue)
{
	if (CIS_Site::ESWA != (CIS_Site)SITE)
		return true;

	if (dWidth > dSpecWidth)
	{
		return true;
	}
	else
	{
		if (nMinimumGrayValue < nSpec_MinimumGrayValue)
		{
			return true;
		}
	}

	return false;
}

BOOL Classification::ScratchFilter_WidthLength(const int SITE, const double dWidth, const double dLength, const double dSpec_Ratio)
{
	if (CIS_Site::ESWA != (CIS_Site)SITE)
		return true;

	//if (dWidth < dSpec_Width || dLength < dSpec_Length)	//사용자 설정 필터값(0.25) 보다 작으면 길다고 본다. 
	//	return true;

	double numerator = min(dWidth, dLength);
	double denominator = max(dWidth, dLength);

	if ((numerator / denominator) < dSpec_Ratio/*0.25*/)			// 비율이 설정값보다 크면
	{
		return true;
	}

	return false;
}

BOOL Classification::isScratch(const int SITE, double dSizeX, double dSizeY, double dSpecTinyWidth, int threshold, double dAngle, int check, BOOL bifAND, CString& strType, int& nret)
{
	if (CIS_Site::ESWA == (CIS_Site)SITE)
	{
		if (dSizeY < m_spec_Scratch.m_dY/*20mm*/)
		{
			//타이니 스크래치
			//if (dSizeX < dSpecTinyWidth)
			if (dSizeX > dSpecTinyWidth)
			{
				nret = SCRATCH;
				strType = GetDefectName(nret);
				return true;
			}
		}
		else
		{
			//if (dSizeX < m_spec_Scratch.m_dX)
			if (dSizeX > m_spec_Scratch.m_dX)
			{
				nret = SCRATCH;
				strType = GetDefectName(nret);
				return true;
			}
		}
	}
	else
	{
		if (true == isValid(bifAND, dSizeX, dSizeY, m_spec_Scratch.m_dX, m_spec_Scratch.m_dY, m_spec_Scratch.m_dMin_X, m_spec_Scratch.m_dMin_Y, threshold, m_spec_Scratch.m_nThreshold))
		{
			if (2 == check && abs(dAngle) > 85 )
			{
				nret = SCRATCH;
				strType = GetDefectName(nret);

				return true;
			}
		}
	}

	return false;
}
