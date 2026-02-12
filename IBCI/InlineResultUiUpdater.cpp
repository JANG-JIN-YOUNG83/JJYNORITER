#include "pch.h"
#include "resource.h"
#include "InlineResultUiUpdater.h"
#include "IBCIDlg.h"
#include "SystemParameter.h"

#include "../SyncPci/Syncboard.h"

#include "Base/LoggerInterface.h"
#include "Gadget/TimeCheck.h"

#include "CISIViewerProperty.h"
#include <sstream>
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

InlineResultUiUpdater::InlineResultUiUpdater(IPVM::LoggerInterface& loggerIO, IPVM::LoggerInterface& loggerResult, CIBCIDlg* parent, CTime time, long nDelayMs, CString path)
    : m_loggerIO(loggerIO)
    , m_loggerResult(loggerResult)
    , m_parent(parent)
{
    m_timeRun = CTimeToChrono(time);
    m_delay = std::chrono::milliseconds(nDelayMs);
    m_path_to_watch = CT2A(path);

    __super::BeginWorker();
    __super::Trigger();
}

InlineResultUiUpdater::~InlineResultUiUpdater()
{
	__super::EndWorker();
}

std::wstring format_time(const std::chrono::system_clock::time_point& time_point)
{
    std::time_t time = std::chrono::system_clock::to_time_t(time_point);
    std::tm timeinfo;
    localtime_s(&timeinfo, &time);

    std::wostringstream oss;
    oss << std::put_time(&timeinfo, _T("%Y-%m-%d %H:%M:%S"));
    return oss.str();
}

bool InlineResultUiUpdater::OnTrigger() //여러번 들어올수있지 않을까?..
{
    while (1)
    {
        DWORD dwSubRet = ::WaitForSingleObject(m_parent->m_Signal_Kill_UiUpdater, m_delay.count());
        if (dwSubRet == WAIT_OBJECT_0)
            break;
        
        for (const auto& entry1 : fs::directory_iterator(m_path_to_watch))
        {
            if (::WaitForSingleObject(m_parent->m_Signal_Kill_UiUpdater, 0) == WAIT_OBJECT_0) break;
            for (const auto& entry2 : fs::directory_iterator(entry1))
            {
                for (const auto& entry3 : fs::directory_iterator(entry2))
                {
                    for (const auto& entry4 : fs::directory_iterator(entry3))
                    {
                        for (const auto& entry5 : fs::directory_iterator(entry4))
                        {
                            for (const auto& entry6 : fs::directory_iterator(entry5))
                            {
                                if (fs::is_regular_file(entry6.path()))
                                {
                                    // 파일의 마지막 수정 시간 가져오기
                                    auto file_time = fs::last_write_time(entry6.path());

                                    // 파일의 마지막 수정 시간을 시스템 시간으로 변환
                                    auto sctp
                                        = std::chrono::time_point_cast<std::chrono::system_clock::time_point::duration>(
                                            file_time - decltype(file_time)::clock::now()
                                            + std::chrono::system_clock::now());

                                    // 시스템 시간 포맷팅
                                    std::wstring formatted_time;
                                    formatted_time = format_time(m_timeRun) + _T("\n");
                                    TRACE(formatted_time.c_str());
                                    formatted_time = format_time(sctp) + _T("\n");
                                    TRACE(formatted_time.c_str());

                                    // 시간 멤버 변수보다 나중에 수정된 파일인지 확인
                                    if (sctp < m_timeRun)
                                        continue;

                                    // "csvv"
                                    std::wstring strPC_ID = std::to_wstring(g_systemParameter.nPC_ID);
                                    wstring extension = entry6.path().extension().wstring();
                                    /*if (extension.find(strPC_ID) == std::wstring::npos)
                                        continue;*/
                                    if (g_systemParameter.bUseServer)
                                    {
                                        CString pathCsv = (CString)entry6.path().c_str();
                                        if (g_systemParameter.bServer && extension == _T(".csv12"))
                                        {
                                            ReadCsvFileNameCheck(pathCsv, 1); // 1이 포함되어있을 때
                                        }
                                        else if (!g_systemParameter.bServer && extension == _T(".csv2"))
                                        {
                                            ReadCsvFileNameCheck(pathCsv, 2); // 2가 포함되어있을 때
                                        }
                                        else
                                        {
                                            continue;
                                        }
                                    }
                                    else
                                    {
                                        if (extension.find(strPC_ID) == std::wstring::npos)
                                            continue;

                                        // csv 읽어서 벡터에 추가
                                        CString pathCsv = (CString)entry6.path().c_str();
                                        std::vector<CISIViewerProperty> vecReceiveData;
                                        if (!ReadCsvISIViewerProperty(vecReceiveData, pathCsv))
                                            continue;

                                        // 파일을 벡터에 추가
                                        CSingleLock lock(&m_csInlineResultUiUpdater, TRUE);

                                        m_parent->m_vecReceiveData.insert(m_parent->m_vecReceiveData.end(),
                                            vecReceiveData.begin(), vecReceiveData.end());
                                        lock.Unlock();

                                        // .csvv to .csv
                                        CString pathOrg = entry6.path().c_str();
                                        CString pathNew, strExt;
                                        pathNew.Format(_T("%s"), pathOrg.Left(pathOrg.ReverseFind('.')));
                                        strExt = extension.c_str();
                                        strExt.Replace(strPC_ID.c_str(), _T(""));
                                        pathNew.AppendFormat(_T("%s"), strExt);

                                        CFileFind find;
                                        if (find.FindFile(pathNew))
                                            DeleteFile(pathNew);
                                        MoveFile(pathOrg, pathNew);
                                    }
                                }
                            }
                            
                        }
                    }
                    //if (fs::is_regular_file(entry3.path()))
                    //{
                    //    // 파일의 마지막 수정 시간 가져오기
                    //    auto file_time = fs::last_write_time(entry3.path());

                    //    // 파일의 마지막 수정 시간을 시스템 시간으로 변환
                    //    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::time_point::duration>(
                    //        file_time - decltype(file_time)::clock::now() + std::chrono::system_clock::now());

                    //    // 시스템 시간 포맷팅
                    //    std::wstring formatted_time;
                    //    formatted_time = format_time(m_timeRun) + _T("\n");
                    //    TRACE(formatted_time.c_str());
                    //    formatted_time = format_time(sctp) + _T("\n");
                    //    TRACE(formatted_time.c_str());

                    //    // 시간 멤버 변수보다 나중에 수정된 파일인지 확인
                    //    if (sctp < m_timeRun)
                    //        continue;

                    //    // "csvv"
                    //    std::wstring strPC_ID = std::to_wstring(g_systemParameter.nPC_ID);
                    //    wstring extension = entry3.path().extension().wstring();
                    //    if (extension.find(strPC_ID) == std::wstring::npos)
                    //        continue;

                    //    // csv 읽어서 벡터에 추가
                    //    CString pathCsv = (CString)entry3.path().c_str();
                    //    std::vector<CISIViewerProperty> vecReceiveData;
                    //    if (!ReadCsvISIViewerProperty(vecReceiveData, pathCsv))
                    //        continue;

                    //    // 파일을 벡터에 추가
                    //    CSingleLock lock(&m_csInlineResultUiUpdater, TRUE);

                    //    m_parent->m_vecReceiveData.insert(m_parent->m_vecReceiveData.end(), vecReceiveData.begin(), vecReceiveData.end());
                    //    lock.Unlock();

                    //    // .csvv to .csv
                    //    CString pathOrg = entry3.path().c_str();
                    //    CString pathNew, strExt;
                    //    pathNew.Format(_T("%s"), pathOrg.Left(pathOrg.ReverseFind('.')));
                    //    strExt = extension.c_str();
                    //    strExt.Replace(strPC_ID.c_str(), _T(""));
                    //    pathNew.AppendFormat(_T("%s"), strExt);

                    //    CFileFind find;
                    //    if (find.FindFile(pathNew))
                    //        DeleteFile(pathNew);
                    //    MoveFile(pathOrg, pathNew);
                    //}
                }//entry3
            }//entry2
        }//entry1
    }

    return true;
}

#define READ_CSV_FIELD(stream, var, convertFunc) \
    std::getline(stream, cell, ','); \
    var = convertFunc(cell)
bool InlineResultUiUpdater::ReadCsvISIViewerProperty(std::vector<CISIViewerProperty>& properties, CString pathCsv)
{
    // 파일 저장.
    CStdioFile file;
    CFileFind find;
    CFileException ex;
    UINT nCheck = CFile::modeRead | CFile::typeText;

    if (!file.Open(pathCsv, nCheck, &ex))
    {
        //ex.ReportError();
        return false;
    }

    setlocale(LC_ALL, "korean");
    //file.SeekToEnd();

    CString line;
    file.ReadString(line);
    while (file.ReadString(line))
    {
        CISIViewerProperty propertyView;
        propertyView.bDisplayDefectView = false;
        if (line.IsEmpty())
            break;

        // CSV 행을 벡터로 변환
        std::vector<std::string> row;
        std::string strLine = CStringA(line).GetString();
        std::istringstream ss(strLine);
        std::string cell;

        std::getline(ss, cell, ',');
        READ_CSV_FIELD(ss, propertyView.nAVG_GV, std::stol);
        READ_CSV_FIELD(ss, propertyView.nMIN_GV, std::stol);
        READ_CSV_FIELD(ss, propertyView.nMAX_GV, std::stol);
        READ_CSV_FIELD(ss, propertyView.nBRIGHT_AVG_GV, std::stol);
        READ_CSV_FIELD(ss, propertyView.nDARK_AVG_GV, std::stol);
        READ_CSV_FIELD(ss, propertyView.fSIZE_X_mm, std::stof);
        READ_CSV_FIELD(ss, propertyView.fSIZE_Y_mm, std::stof);
        READ_CSV_FIELD(ss, propertyView.fAREA_Ratio, std::stof);
        READ_CSV_FIELD(ss, propertyView.fRATIO_YX, std::stof);
        READ_CSV_FIELD(ss, propertyView.fRATIO_XY, std::stof);
        READ_CSV_FIELD(ss, propertyView.fCOMPACT, std::stof);
        READ_CSV_FIELD(ss, propertyView.fRATIO_WHITE, std::stof);
        READ_CSV_FIELD(ss, propertyView.fRATIO_DARK, std::stof);
        READ_CSV_FIELD(ss, propertyView.fEDGE_ENERGY, std::stof);
        READ_CSV_FIELD(ss, propertyView.fMASS_CENTER_GV_GV, std::stof);
        READ_CSV_FIELD(ss, propertyView.strLotID,            [](const std::string& s)            {                return CString(s.c_str());            });
        READ_CSV_FIELD(ss, propertyView.nCellIndex, std::stol);
        READ_CSV_FIELD(ss, propertyView.strTime,            [](const std::string& s)            {                return CString(s.c_str());            });
        READ_CSV_FIELD(ss, propertyView.m_nCandidateDefectCode, std::stol);
        READ_CSV_FIELD(ss, propertyView.nUp, std::stol);
        READ_CSV_FIELD(ss, propertyView.nDefectX, std::stol);
        READ_CSV_FIELD(ss, propertyView.nDefectY, std::stol);
        READ_CSV_FIELD(ss, propertyView.m_nJudge, std::stol);
        READ_CSV_FIELD(ss, propertyView.pathDirectBright,            [](const std::string& s)            {                return CString(s.c_str());            });
        READ_CSV_FIELD(ss, propertyView.pathDirectDark,            [](const std::string& s)            {                return CString(s.c_str());            });
        READ_CSV_FIELD(ss, propertyView.pathCropBright,            [](const std::string& s)            {                return CString(s.c_str());            });
        READ_CSV_FIELD(ss, propertyView.pathCropDark,            [](const std::string& s)            {                return CString(s.c_str());            });
        READ_CSV_FIELD(ss, propertyView.pathCropOversizeBright,            [](const std::string& s)            {                return CString(s.c_str());            });
        READ_CSV_FIELD(ss, propertyView.pathCropOversizeDark,            [](const std::string& s)            {                return CString(s.c_str());            });

        CString pathBright, pathDark;
        pathBright.Format(_T("%s%s"), g_systemParameter.pathResultDataCsv, propertyView.pathDirectBright);
        pathDark.Format(_T("%s%s"), g_systemParameter.pathResultDataCsv, propertyView.pathDirectDark);
        propertyView.brightImage.LoadFrom(pathBright);
        propertyView.darkImage.LoadFrom(pathDark);

        properties.push_back(propertyView);
    }

    file.Close();
    return true;
}

std::chrono::system_clock::time_point InlineResultUiUpdater::CTimeToChrono(const CTime& ctime)
{
    // CTime 객체를 time_t로 변환
    time_t tt = ctime.GetTime();

    // time_t를 std::chrono::system_clock::time_point로 변환
    return std::chrono::system_clock::from_time_t(tt);
}


///////////////////////////////////view
InlineResultUiUpdater_View::InlineResultUiUpdater_View(IPVM::LoggerInterface& loggerIO, IPVM::LoggerInterface& loggerResult, CIBCIDlg* parent)
    : m_loggerIO(loggerIO)
    , m_loggerResult(loggerResult)
    , m_parent(parent)
{
    __super::BeginWorker();
    //__super::Trigger();
}

InlineResultUiUpdater_View::~InlineResultUiUpdater_View()
{
    __super::EndWorker();
}

bool InlineResultUiUpdater_View::OnTrigger() //여러번 들어올수있지 않을까?..
{
    std::vector<CISIViewerProperty> vecReceiveData;

    {
        CSingleLock lock(&m_csUiUpdater_View, true);
        //swap(vecReceiveData, m_vecReceiveData_View);
        vecReceiveData = std::move(m_vecReceiveData_View);
    }
    if (vecReceiveData.size() > 9)
        vecReceiveData.erase(vecReceiveData.begin(), vecReceiveData.begin() + vecReceiveData.size() - 9);

    for (int nIdx = 0; nIdx < vecReceiveData.size(); nIdx++)
    {
        CISIViewerProperty propertyView = vecReceiveData[nIdx];

        //if ((II_RESULT_VALUE)propertyView.m_nJudge != II_RESULT_VALUE::PASS)
        {
        }
        HWND hWnd = m_parent->GetSafeHwnd();
        if (hWnd != NULL && ::IsWindow(hWnd))
            SendMessage(hWnd, UWM_UPDATE_UI, (WPARAM)&propertyView, (LPARAM)0);
        //UpdateDefectView(propertyView);
        //UpdateDefectCount(propertyView);
        //UpdateDefectMap(propertyView);
    }

    return true;
}

///////////////////////////////////map
InlineResultUiUpdater_Map::InlineResultUiUpdater_Map(IPVM::LoggerInterface& loggerIO, IPVM::LoggerInterface& loggerResult, CIBCIDlg* parent)
    : m_loggerIO(loggerIO)
    , m_loggerResult(loggerResult)
    , m_parent(parent)
{
    __super::BeginWorker();
    //__super::Trigger();
}

InlineResultUiUpdater_Map::~InlineResultUiUpdater_Map()
{
    __super::EndWorker();
}

bool InlineResultUiUpdater_Map::OnTrigger() //여러번 들어올수있지 않을까?..
{
    std::vector<CISIViewerProperty> vecReceiveData;

    {
        CSingleLock lock(&m_csUiUpdater_Map, true);
        //swap(vecReceiveData, m_vecReceiveData_Map);
        vecReceiveData = std::move(m_vecReceiveData_Map);
    }

    for (int nIdx = 0; nIdx < vecReceiveData.size(); nIdx++)
    {
        CISIViewerProperty propertyView = vecReceiveData[nIdx];

        //if ((II_RESULT_VALUE)propertyView.m_nJudge != II_RESULT_VALUE::PASS)
        {
        }
        //SendMessage(m_parent->GetSafeHwnd(), UWM_UPDATE_UI, (WPARAM)&propertyView, (LPARAM)0);
        //UpdateDefectView(propertyView);
        //UpdateDefectCount(propertyView);
        m_parent->UpdateDefectMap(propertyView);
    }

    return true;
}

///////////////////////////////////map
InlineResultUiUpdater_Count::InlineResultUiUpdater_Count(IPVM::LoggerInterface& loggerIO, IPVM::LoggerInterface& loggerResult, CIBCIDlg* parent)
    : m_loggerIO(loggerIO)
    , m_loggerResult(loggerResult)
    , m_parent(parent)
{
    __super::BeginWorker();
    //__super::Trigger();
}

InlineResultUiUpdater_Count::~InlineResultUiUpdater_Count()
{
    __super::EndWorker();
}

bool InlineResultUiUpdater_Count::OnTrigger() //여러번 들어올수있지 않을까?..
{
    std::vector<CISIViewerProperty> vecReceiveData;

    {
        CSingleLock lock(&m_csUiUpdater_Count, true);
        //swap(vecReceiveData, m_vecReceiveData_Count);
        vecReceiveData = std::move(m_vecReceiveData_Count);
    }

    for (int nIdx = 0; nIdx < vecReceiveData.size(); nIdx++)
    {
        CISIViewerProperty propertyView = vecReceiveData[nIdx];

        //if ((II_RESULT_VALUE)propertyView.m_nJudge != II_RESULT_VALUE::PASS)
        {
        }
        //SendMessage(m_parent->GetSafeHwnd(), UWM_UPDATE_UI, (WPARAM)&propertyView, (LPARAM)0);
        //UpdateDefectView(propertyView);
        m_parent->UpdateDefectCount(propertyView);
        //m_parent->UpdateDefectMap(propertyView);
    }

    return true;
}

void InlineResultUiUpdater::ReadCsvFileNameCheck(CString strPath, int nExtension)
{
    //CString pathCsv = (CString)entry6.path().c_str();
    std::vector<CISIViewerProperty> vecReceiveData;
    if (!ReadCsvISIViewerProperty(vecReceiveData, strPath))
        return;

    // 파일을 벡터에 추가
    CSingleLock lock(&m_csInlineResultUiUpdater, TRUE);

    m_parent->m_vecReceiveData.insert(m_parent->m_vecReceiveData.end(), vecReceiveData.begin(), vecReceiveData.end());
    lock.Unlock();

    // .csvv to .csv
    CString pathOrg = strPath;
    CString pathNew, strExt, strErase;
    pathNew.Format(_T("%s"), pathOrg.Left(pathOrg.ReverseFind('.')));
    if (nExtension == 1)
    {
        strErase.Format(_T("%d"), nExtension);
    }
    else
    {
        strErase.Format(_T("%d"), nExtension);
    }
    strExt = strPath.Mid(strPath.ReverseFind('.'));
    strExt.Replace(strErase, _T(""));
    pathNew.AppendFormat(_T("%s"), strExt);

    CFileFind find;
    if (find.FindFile(pathNew))
        DeleteFile(pathNew);
    MoveFile(pathOrg, pathNew);
}