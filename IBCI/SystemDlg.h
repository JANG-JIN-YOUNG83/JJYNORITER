#pragma once
#include "afxwin.h"
#include "SystemParameter.h"
#include "Types.h"
#include "../SharedModule/VisionHostShare//LoginInfo.h"

#define KOREAN 1
#define ENGLISH 2
#define CHINESE 3

// CSystemDlg 대화 상자입니다.

class CIBCIDlg;
class CSystemDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSystemDlg)

public:
	CSystemDlg(const SystemParameter &sysParam, CIBCIDlg* pParent);   // 표준 생성자입니다.
	virtual ~CSystemDlg();

	SystemParameter m_sysParam;
	Share::enLoginMode m_loginMode;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SYSTEMDLG };

private:
	CIBCIDlg* m_parent;
	int		m_nImageSizeX;

	//Object type, Normal rate, Grab time

	CString strAnodeCathode, strUseAreaCam, strCellIDBit, strIsLaserNotching, strEncoderAlarmTimer, strInterlockSleepTime, strInspectTilt, strInspectMisalign, strInspectPetVinyl, strMismatchAbsolute, strPitchUpDownUse, strSensorImageMirror, strExistGrabHardware, strInsulateType, strScrabInterlock, strTabSensorExceptionHandling, strTabCrossCheck, strTabHeightTop, strTabShapeRectCount,
		strBrightImageSend, strOverlayImage, strTabShapeImage, 
		strTab, strSurface,
		strCamSingleDual, strCamSingle, strCamDual, strCameraGain, strLedBrightness,
		strVHomeOffsetUp, strVHomeOffsetDown,
		strMultiImage, strImageCheckPos, strImageCheckBrightness,
		strInspection, strInspectionMargin,
		strCore, 
		strResultDataSavePath, strResultAName, strResultBName, strMachineNumber, strResultImageSave, strDebugImageSave, strMarkingCsvSave,
		strSendImageInfo, strUpImageLeft, strUpImageRight, strDownImageLeft, strDownImageRight,
		strConnectIP, strConnectPort, strFriendIP, strFriendPort, strRollMapRotation, strRollMapXaxisMirror, strRollMapYaxisMirror, strRollMapXOffset,
        strRollMapYOffset, strRollMapImageQuality, strMotorIORetry;
		

public:
	//int GetImageSizeX(){ return m_nImageSizeX; }

	void OnBtnClickedCfPath();
	void OnBtnClickedImageSavePath();
    void OnBtnClickedResultCSVPath();
	void SetSiblingItems(int nID, LPARAM lparam);
	void SetResolution(int nID);
	//void SetImageSizeX(int size){ m_nImageSizeX = size; }

private:
	CXTPPropertyGrid* m_systemParaGrid;

protected:
	void UpdateGridList();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSystemOk();
	afx_msg void OnBnClickedButtonSystemCancel();
	afx_msg LRESULT OnGridNotify(WPARAM wparam, LPARAM lparam);
};

class CCustomXTPCombo : public CXTPPropertyGridItem
{
public:
	CCustomXTPCombo(CString strCaption)
		:CXTPPropertyGridItem(strCaption)
	{
		m_bValueChanged = FALSE;
	}

	CStringArray m_strItemValues;
	BOOL m_bValueChanged;

protected:
	virtual void OnValueChanged(CString strValue);
	virtual void OnConstraintsChanged();
};


