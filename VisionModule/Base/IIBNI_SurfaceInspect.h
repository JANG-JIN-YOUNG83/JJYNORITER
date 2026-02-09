#pragma once
//========================================================================================================================
// IBNI 용 Interface Surface Inspect 용
//
// IBNI의 제어권을 통채로 주는 대신 Surface Inspect 용 몇개 함수만을 외부에서 쓸수 있도록 제공함
//========================================================================================================================

struct IIBNI_InspectPara
{
	double m_pinHoleSizeX_mm;
	double m_pinHoleSizeY_mm;
	double m_lineSizeX_mm;
	double m_lineSizeY_mm;
	double m_islandSizeX_mm;
	double m_islandSizeY_mm;

	long	m_targetBrightness;
	long	m_islandThreshold;
	long	m_surfaceWhiteThreshold;
	long	m_surfaceBlackThreshold;
	long	m_surfaceWhiteMargin;
	long	m_surfaceBlackMargin;
	double	m_surfaceLineRatio;
};

class IIBNI_SurfaceInspect
{
public:
	virtual bool I_SInspect_ROIInspect(bool isUpCamera, IPVM::Image_8u_C1& bright, const IPVM::Rect& roi, const IIBNI_InspectPara& para) = 0;
	virtual bool I_SInspect_ROIOverlay(bool isUpCamera, IPVM::Image_8u_C1& bright, IPVM::Image_8u_C1& dark, const IPVM::Rect& roi, const IIBNI_InspectPara& para) = 0;
	virtual bool I_SInspect_ManualIsland(bool isUpCamera, IPVM::Image_8u_C1& bright, IPVM::Image_8u_C1& dark, const IPVM::Rect& roi, const IIBNI_InspectPara& para) = 0;

	virtual long	I_SInspect_GetManualDefectCount() const = 0;
	virtual void	I_SInspect_GetManualDefectRoi(long defectIndex, IPVM::Rect& o_roi) const = 0;
	virtual long	I_SInspect_GetManualDefectType(long defectIndex) const = 0;
	virtual double	I_SInspect_GetManualDefectSizeX(long defectIndex) const = 0;
	virtual double	I_SInspect_GetManualDefectSizeY(long defectIndex) const = 0;
	virtual double	I_SInspect_GetManualInsulate_mm() const = 0;
	virtual double	I_SInspect_GetManualOverlay_mm() const = 0;
};
