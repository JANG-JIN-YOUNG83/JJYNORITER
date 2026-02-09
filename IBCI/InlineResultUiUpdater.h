#pragma once
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1;

#include "../Inspection/ResultInfo.h"
#include "IBCIDlg.h"
#include "SurfaceInspect.h"

#include "Gadget/SimpleWorkerThread.h"

#include <afxmt.h>
#include <deque>

#include <experimental/filesystem> // C++14
namespace fs = std::experimental::filesystem::v1;
#include <chrono>
#include <deque>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>

//class CMainDlg;
class CSyncboard;


class InlineResultUiUpdater : public IPVM::SimpleWorkerThread
{
public:
    InlineResultUiUpdater(IPVM::LoggerInterface& loggerIO, IPVM::LoggerInterface& loggerResult, CIBCIDlg* parent, CTime time, long nDelayMs, CString path);
    InlineResultUiUpdater(const InlineResultCoupler& rhs) = delete;
    virtual ~InlineResultUiUpdater();

    InlineResultUiUpdater& operator=(const InlineResultUiUpdater& rhs) = delete;

    std::string m_path_to_watch;
    std::chrono::duration<int, std::milli> m_delay;
    //std::deque<CString> m_vecstrPath;
    CCriticalSection m_csInlineResultUiUpdater;
    std::chrono::system_clock::time_point m_timeRun;
    std::chrono::system_clock::time_point CTimeToChrono(const CTime& ctime);
    bool ReadCsvISIViewerProperty(std::vector<CISIViewerProperty>& properties, CString pathCsv);
    void ReadCsvFileNameCheck(CString strPath, int nExtension);

protected:
    virtual bool OnTrigger() override;
    IPVM::LoggerInterface& m_loggerIO;
    IPVM::LoggerInterface& m_loggerResult;
    CIBCIDlg* m_parent;
};


// defectMap, defectCount, defectView
class InlineResultUiUpdater_View : public IPVM::SimpleWorkerThread
{
public:
    InlineResultUiUpdater_View(IPVM::LoggerInterface& loggerIO, IPVM::LoggerInterface& loggerResult, CIBCIDlg* parent);
    InlineResultUiUpdater_View(const InlineResultCoupler& rhs) = delete;
    virtual ~InlineResultUiUpdater_View();
    InlineResultUiUpdater_View& operator=(const InlineResultUiUpdater_View& rhs) = delete;

    std::vector<CISIViewerProperty> m_vecReceiveData_View;
    CCriticalSection m_csUiUpdater_View;

protected:
    virtual bool OnTrigger() override;
    IPVM::LoggerInterface& m_loggerIO;
    IPVM::LoggerInterface& m_loggerResult;
    CIBCIDlg* m_parent;
};

class InlineResultUiUpdater_Map : public IPVM::SimpleWorkerThread
{
public:
    InlineResultUiUpdater_Map(IPVM::LoggerInterface& loggerIO, IPVM::LoggerInterface& loggerResult, CIBCIDlg* parent);
    InlineResultUiUpdater_Map(const InlineResultCoupler& rhs) = delete;
    virtual ~InlineResultUiUpdater_Map();
    InlineResultUiUpdater_Map& operator=(const InlineResultUiUpdater_Map& rhs) = delete;

    std::vector<CISIViewerProperty> m_vecReceiveData_Map;
    CCriticalSection m_csUiUpdater_Map;

protected:
    virtual bool OnTrigger() override;
    IPVM::LoggerInterface& m_loggerIO;
    IPVM::LoggerInterface& m_loggerResult;
    CIBCIDlg* m_parent;
};

class InlineResultUiUpdater_Count : public IPVM::SimpleWorkerThread
{
public:
    InlineResultUiUpdater_Count(IPVM::LoggerInterface& loggerIO, IPVM::LoggerInterface& loggerResult, CIBCIDlg* parent);
    InlineResultUiUpdater_Count(const InlineResultCoupler& rhs) = delete;
    virtual ~InlineResultUiUpdater_Count();
    InlineResultUiUpdater_Count& operator=(const InlineResultUiUpdater_Count& rhs) = delete;

    std::vector<CISIViewerProperty> m_vecReceiveData_Count;
    CCriticalSection m_csUiUpdater_Count;

protected:
    virtual bool OnTrigger() override;
    IPVM::LoggerInterface& m_loggerIO;
    IPVM::LoggerInterface& m_loggerResult;
    CIBCIDlg* m_parent;
};