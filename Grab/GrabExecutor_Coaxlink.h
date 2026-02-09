#pragma once

#include <EuresysSharedGenTL.h>
#include <EGrabber.h>
#include <RGBConverter.h>

#include "GrabExecutor.h"

#include <Base/LoggerInterface.h>

using namespace Euresys;

#define BUFFER_CNT	10

typedef enum
{
	MODE_CONTINUOUS,
	MODE_HARDTRIGGER_FG,	//Encoder Signal into Frame Grabber
	MODE_HARDTRIGGER_CAM,	//Encoder Signal into Camera 
	MODE_SETTING,
	MODE_CUSTOM,
	MODE_CXP6_X2,
	MODE_CXP6_X4
} Grabmode_t;

typedef void FUNC_GRAB_CALLBACK(void *pUserData, const BITMAPINFO& pBitmapInfo, const BYTE* const pBuffer);
typedef struct imgInfoStruct
{
	int Index;
	int Width;
	int Height;
	void* Buffer;

	imgInfoStruct()
	{
		Index		= 0;
		Width		= 0;
		Height		= 0;
		Buffer		= NULL;
	}
} Image_t;

class CCoaxlinkGrabber;

class GrabExecutor_Coaxlink
{
public:
	GrabExecutor_Coaxlink(IPVM::LoggerInterface& logger, const int cameraIndex, const int grabberIndex);
	virtual ~GrabExecutor_Coaxlink(void);

	/***************************************************************/
	/* Attribute                                                   */
private:
	const int m_grabberIndex;
	const int m_cameraIndex;
	int									m_Width;
	int									m_Height;
	int									PixelComponentCount;
	std::string							m_PixelFormat;
	bool								m_bOpen;
	bool								m_bGrabbing;
	int									m_nContext;
	CString								m_strVenderName;
	int									m_nBuffer_cnt;
	Grabmode_t							m_Grabmode;
	EGenTL								m_GenTL;
	/* Attribute                                                   */
	/***************************************************************/

	/***************************************************************/
	/* Basic functions                                             */

public:
    BOOL								m_bReleaseGrabDoneSign;
	long								m_nCamNum;
	long								m_nToTalCameraCnt;
	int									m_nImageCnt;
	bool								Open(IPVM::LoggerInterface& logger);
	int									GetPixelComponentCount();
	void								Close(void);
	void								Grab(uint64_t grab_cnt = GENTL_INFINITE);	
	void								GenerateSoftTrigger(void);
	void								Stop(void);
	void								SetAllEvent();
	void								SetConfigure(Grabmode_t mode);
	void								SetContext(int64_t nContext);
	void								OnImageEvent(unsigned char* pBuffer);
	bool								m_bSaveImage;
	void								SetBufferCount(uint64_t buffer_cnt);	//if you change the number of buffers, must be call this function before "Grab" function

	//inline function
	bool								IsOpen(){return m_bOpen;}
	bool								IsGrabbing(){return m_bGrabbing;}
	Grabmode_t							GetGrabmode(void) { return m_Grabmode; }
	template<typename M> int			getInteger(const std::string &feature){return m_pCoaxlink_config->getInteger<M>(feature);}
	template<typename M> double			getFloat(const std::string &feature){return m_pCoaxlink_config->getFloat<M>(feature);}
	template<typename M> std::string	getString(const std::string &feature){return m_pCoaxlink_config->getString<M>(feature);}
	template<typename M> void			setInteger(const std::string &feature, int value){ m_pCoaxlink_config->setInteger<M>(feature, value);}
	template<typename M> void			setFloat(const std::string &feature, double value){ m_pCoaxlink_config->setFloat<M>(feature, value);}
	template<typename M> void			setString(const std::string &feature, const std::string &value){ m_pCoaxlink_config->setString<M>(feature, value);}
	template<typename M> void			execute(const std::string &command){ m_pCoaxlink_config->execute<M>(command);}
	template<typename M> void			enableEvent(){return m_pCoaxlink_config->enableEvent<M>();}

	CCoaxlinkGrabber*					m_pCoaxlink[4];
	CCoaxlinkGrabber*					m_pCoaxlink_config;
	/* Basic functions                                             */
	/***************************************************************/

	/***************************************************************/
	/* Display													   */
private:
	CDC*							m_pDC;
	HWND							m_hView;
	CRect							m_Rect;
	BITMAPINFO*						m_pBitmapInfo;


public:
	void							SetView(CDC* pDC);
	void							SetPictureBox(CRect rect);
	/* Display													   */
	/***************************************************************/
	
	/***************************************************************/
	/* emk   													   */
public:
	int m_nSave;
	void InitSaveNumber();
	void StandstillSaveNumber();
	
	// callback
	struct SGrabCallbackData
	{
		HWND m_hwndOwner;
		void *m_pUserData;
		FUNC_GRAB_CALLBACK *m_pFuncCallback;
	};
	SGrabCallbackData m_sCallbackGrab;

	void RegisterCallback_Grab(HWND hwndOwner, void *pUserData, FUNC_GRAB_CALLBACK *pCallback);
	/* emk   													   */
	/***************************************************************/

	long GetCamWidth();
	long GetCamHeight();
    BOOL MakeBuffer(long nToTalCamCount);
};

class CCoaxlinkGrabber : public EGrabber<CallbackMultiThread>, GrabExecutor
{
public:
	CCoaxlinkGrabber(IPVM::LoggerInterface& logger, EGenTL &GenTL, int interfaceIndex, int deviceIndex, int dataStreamIndex, void* pParent);
	~CCoaxlinkGrabber(void);


private:
	unsigned char*		m_poImgPtr;
	void*				m_pParent;
	long				m_nGrabCnt;
	RGBConverter		rgbConverter;

	virtual void SaveQueueImage(LPCTSTR targetFolder, long saveMaxCount = -1) override;

protected:
	virtual bool OnTrigger() override;		// IPVM::GrabExecutor

	virtual void OnStart() override;
	virtual void OnRequestStop() override;	// GrabExecutor

public:
    void SetGrabCount(long nCnt);
    long GetGrabCount();
	void OnImageEvent(unsigned char* pBuffer);
	static std::string formatTimestamp(uint64_t timestamp);

	virtual void onNewBufferEvent(const NewBufferData &data);
	virtual void onIoToolboxEvent(const IoToolboxData &data);
	virtual void onCicEvent(const CicData &data);
	virtual void onDataStreamEvent(const DataStreamData &data);
	virtual void onCxpInterfaceEvent(const CxpInterfaceData &data);
    virtual void SetVerticalLength(long nPageLength);
    virtual void SetSystemGain(CString strGain);
    virtual void SetExposureTime(CString strExposure, int nRow);
    virtual double GetSystemGain();
    virtual void GetExposureTime(double& dBright, double& dDark);
    virtual void SetSensorSelector(CString strSensorSel);
    virtual void SetSensorGain(CString strSensorGain);
    virtual void GetSensorInfo(int& nSensorSelector, double& dSensorGain);
    virtual void GetRowGain(double& dRowGainBright, double& dRowGainDark);
    virtual void SetRowGain(CString strRowGain, int nRow);
    virtual void ResponseLeveling();
    virtual void Reset();

	virtual void SetExposureTime_Multi(std::vector<CString> vecstrExposureValue, int nCntCh) override;
    
	virtual double GetFPGATemperature();
    virtual BOOL GetCameraConnectCheck();
	int64_t m_iWidth;
	int64_t m_iHeight;
	size_t m_stPayloadSize;
	std::string m_PixelFormat;
	const int m_grabberIndex;

	long m_nImageCnt;
    long m_nMaxImageCnt;
    BOOL m_bGrabDoneSign;

	unsigned char*		RGBConvert(unsigned char * imageptr, std::string pixelformat, int width, int height);
};