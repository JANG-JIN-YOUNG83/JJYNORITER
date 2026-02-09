#include "stdafx.h"
#include "InspectionChattering.h"
#include "InspectionCell.h"
#include "InspectionCellAlign.h"
#include "DebugInfo.h"
#include "Accessor.h"
#include "UnionCellResult.h"

#include "Types/LineEq_32f.h"
#include "Algorithm/DataFitting.h"
#include "Types/LineEq_64f.h"
#include "Algorithm/Geometry.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "Algorithm/ImageProcessing.h"
#include <math.h>

#include "Algorithm/ImageProcessing.h"
#include "Algorithm/DataFitting.h"
#include "Algorithm/Geometry.h"
#include "Algorithm/EdgeDetection.h"
#include "Algorithm/EdgeDetectionFilter.h"
#include "Algorithm/EdgeDetectionDebugInfo.h"
#include "Algorithm/Mathematics.h"
#include "Base/LoggerInterface.h"
#include "Types/Point_32f_C2.h"
#include "Types/Point_32f_C3.h"
#include "Types/Image_8u_C1.h"
#include "Types/Point_64f_C2.h"
#include "Types/LineEq_64f.h"
#include "Types/LineEq_32f.h"

#include "Algorithm/FastFourierTransform1D.h"

#include <algorithm>
#include <complex>


CInspectionChattering::CInspectionChattering(CProcessingBase* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
    : CInspectionBase(parent, ProcItemCategory::Chattering, logger, loggerResult)
    , m_parent((CInspectionCell*)parent)
{
}

CInspectionChattering::~CInspectionChattering()
{
}
bool CInspectionChattering::IsInspectionEnabled() const
{
    return true;
}

void CInspectionChattering::PrepareDebugInfo(Inspection::DebugInfo* debugInfo)
{
    debugInfo->AddItem(LanguageDef::Debug_Chattering_Result_Mid);
    debugInfo->AddItem(LanguageDef::Debug_Chattering_Result_Total);
    debugInfo->AddItem(LanguageDef::Debug_TestImage1);
}
void CInspectionChattering::ResetInspection()
{
}
#include "Algorithm/ImageProcessing.h"
bool CInspectionChattering::ProcInspection(Inspection::DebugInfo* debugInfo)
{
    double sum_bright = 0.;
    long nDivCount = m_para.m_nDivCount;
    std::vector<float> vecPeriod;
    std::vector<float> vecAmp;

    /*CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    CInspectionMeasure* align = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::cellAlign);

    IPVM::Rect rect = align->m_unionResult.m_rectAreaBlob[1];

    IPVM::Rect offset = m_para.m_rtOffsetRoi;
    rect.DeflateRect(offset.m_left, offset.m_top, offset.m_right, offset.m_bottom);

    m_para.m_rtSetInspArea = rect;*/
    IPVM::Rect rtChattering;
    //std::vector < IPVM::Point_32f_C2> vecPtResult;
    std::vector<CString> vecStrResult;
    vecPeriod.clear();
    vecStrResult.clear();
    m_unionResult.m_vecRectChattering.clear();

    if (nDivCount > 0)
    {
        for (int n = 0; n < nDivCount; n++)
        {
            long nWidth = m_para.m_rtSetInspArea.Width() / nDivCount;
            long nHeight = m_para.m_rtSetInspArea.Height();

            IPVM::Rect rtDiv;
            IPVM::Point_32f_C2 ptDiv;
            
            rtDiv.m_left = m_para.m_rtSetInspArea.m_left + nWidth * n;
            rtDiv.m_top = m_para.m_rtSetInspArea.m_top;
            rtDiv.m_right = rtDiv.m_left + 20;
            rtDiv.m_bottom = m_para.m_rtSetInspArea.m_bottom;
            ptDiv.m_x = rtDiv.m_left;
            ptDiv.m_y = rtDiv.m_top;
            //vecPtResult.push_back(ptTemp);
            m_unionResult.m_vecRectChattering.push_back(rtDiv);
            IPVM::Image_8u_C1 chatteringImg;
            if (m_para.m_nSelectDarkImage == 0) // Bright
            {
                chatteringImg = m_inspectionImageBright;
            }
            else // Dark 
            {
                chatteringImg = m_inspectionImageDark;
            }
            //IPVM::ImageProcessing::Copy(m_EqualImage_Bright[index], m_vecrtCal[n], m_vecrtCal[n], m_bufferThreshold);
            /*long nHeightDiv = nHeight / 10;
            for (long nDivY=0; nDivY <10; nDivY++)
            {
                IPVM::Rect rtDiv2;
                rtDiv2 = rtDiv;
                rtDiv2.m_top = rtDiv.m_top + nHeightDiv * nDivY;
                rtDiv2.m_bottom = rtDiv.m_top + nHeightDiv * (nDivY + 1);
                IPVM::ImageProcessing::EqualizeHistogram(chatteringImg, rtDiv2, m_bufferThreshold);
            }*/

            IPVM::ImageProcessing::EqualizeHistogram(chatteringImg, rtDiv, m_bufferThreshold);
            
        
            // 주기 계산하는 코드입니다. 나중에 사용해서 계산할때 응용하세요!! to 남욱 예진 승만
            if (rtDiv.Height() > 10)
            {
                std::vector<float> fTemp;

                fTemp.resize(rtDiv.Height());

                std::vector<float> fTempOrig;

                fTempOrig.resize(rtDiv.Height());

                for (long nID = 0; nID < rtDiv.Height(); nID++)
                {
                    float fTempSub = 0;
                    float fTempOrigSub = 0;
                    for (long nX=0; nX<50; nX++)
                    {
                        fTempSub += *m_bufferThreshold.GetMem(rtDiv.m_left, rtDiv.m_top + nID);
                        fTempOrigSub += *chatteringImg.GetMem(rtDiv.m_left, rtDiv.m_top + nID);
                    }
                    fTemp[nID] = fTempSub / 50;
                    fTempOrig[nID] = fTempOrigSub / 50;
                }

                float fPeriod, b;
                

                fPeriod = GetProfilePeriod_FFT(fTemp);

                
                float testAmp=0;
               
                if (fPeriod > 10)
                {
                    long nperiod = fPeriod + 1;
                    long nTempSize = rtDiv.Height() / nperiod;
                    long nTempDiv = rtDiv.Height() / nTempSize;
                    for (long n = 0; n < nTempSize; n++)
                    {
                        float max = 0;
                        float min = 256;
                        for (long m = 0; m < nperiod; m++)
                        {
                            float fTemp2 = fTempOrig[n * nperiod + m];
                            if (max < fTemp2)
                                max = fTemp2;
                            if (min > fTemp2)
                                min = fTemp2;
                        }

                        float temp = max - min;
                        testAmp += temp;
                    }
                    testAmp = testAmp / nTempSize;
                }
                
                //long n = 0;
                CString str;


                CString strResult;
                str.Format(_T("  px Period = %.1f"), fPeriod);
                vecPeriod.push_back(fPeriod);
                strResult = str;
                str.Format(_T("  px Amp = %.1f"), testAmp);
                vecAmp.push_back(testAmp);
                strResult = strResult + _T("\n") + str;
                vecStrResult.push_back(strResult);
                //debugInfo->AddOverlayText(LanguageDef::Result, ptDiv, strResult, RGB(255, 128, 0),100);

                if (CheckPeriod(fPeriod) == false && CheckAmp(testAmp) == false)
                {
                    strResult += _T("\n  REJECT");
                    debugInfo->AddOverlay(LanguageDef::Result, rtDiv, strResult, RGB(255, 0, 0), 100, RGB(255, 0, 0));
                    //debugInfo->AddOverlayText(LanguageDef::Result, IPVM::Point_32f_C2(50, 10), _T("REJECT"), RGB(255, 0, 0), 300);
                    m_resultValue = II_RESULT_VALUE::REJECT;
                }
                else
                {
                    strResult += _T("\n  PASS");
                    debugInfo->AddOverlay(LanguageDef::Result, rtDiv, strResult, RGB(0, 255, 0), 100, RGB(0, 255, 0));
                    //debugInfo->AddOverlayText(LanguageDef::Result, IPVM::Point_32f_C2(50, 10), _T("PASS"), RGB(0, 255, 0), 300);
                    m_resultValue = II_RESULT_VALUE::PASS;
                }

            }
        }
        m_unionResult.m_vecfChatteringPeriod = vecPeriod;
        m_unionResult.m_vecfChatteringAmp = vecAmp;

        
        long nIndex;
        float fMid = 0.f;
        if (vecPeriod.size() > 0)
        {
            std::sort(vecPeriod.begin(), vecPeriod.end());
            if (vecPeriod.size() > 2)
                nIndex = vecPeriod.size() / 2;
            else
                nIndex = 0;
            fMid = vecPeriod[nIndex];
            
        }
        

        for (int n = 0; n < vecPeriod.size(); n++)
        {
            CString text = _T("Chattering Period Value");
            debugInfo->AddText(LanguageDef::Debug_Chattering_Result_Total, vecPeriod[n], text);
            debugInfo->AddText(LanguageDef::Result, vecPeriod[n], text);
            text = _T("Chattering Amp Value");
            debugInfo->AddText(LanguageDef::Debug_Chattering_Result_Total, vecAmp[n], text);
            debugInfo->AddText(LanguageDef::Result, vecAmp[n], text);
            
        }
        CString text = _T("Chattering Mid Value");
        debugInfo->AddText(LanguageDef::Debug_Chattering_Result_Mid, fMid, text);
    }
    else
    {
        float fPeriod;
        IPVM::Image_8u_C1 chatteringImg;

        if (m_para.m_nSelectDarkImage == 0) // Bright
        {
            chatteringImg = m_inspectionImageBright;
        }
        else // Dark
        {
            chatteringImg = m_inspectionImageDark;
        }
        IPVM::ImageProcessing::GetSum(chatteringImg, m_para.m_rtSetInspArea, sum_bright);
        double favg = sum_bright / (m_para.m_rtSetInspArea.Width() * m_para.m_rtSetInspArea.Height());
        CString str;
        str.Format(_T("%.2f"), favg);
        // 주기 계산하는 코드입니다. 나중에 사용해서 계산할때 응용하세요!! to 남욱 예진 승만
        if (m_para.m_rtSetInspArea.Height() > 10)
        {
            std::vector<float> fTemp;
            return true;
            fTemp.resize(m_para.m_rtSetInspArea.Height());

            for (long n = 0; n < m_para.m_rtSetInspArea.Height(); n++)
            {
                
                fTemp[n] = *chatteringImg.GetMem(m_para.m_rtSetInspArea.m_left, m_para.m_rtSetInspArea.m_top + n);
            }
            float b;

            fPeriod = GetProfilePeriod_FFT(fTemp);
            long n = 0;
            CString str;
            str.Format(_T("px Period = %f"), fPeriod);
        }
        CString text = _T("Chattering Mid Value");
        //debugInfo->AddText(LanguageDef::Debug_Chattering_Result_Mid, fPeriod, text);

        text = _T("Chattering Total Value");
        //debugInfo->AddText(LanguageDef::Debug_Chattering_Result_Total, fPeriod, text);
    }
    //debugInfo->AddOverlay(LanguageDef::Result, m_unionResult.m_vecRectChattering, RGB(255, 128, 0));
    IPVM::Point_32f_C2 temp;
    

    //m_resultValue = II_RESULT_VALUE::PASS;
    return true;    
}

void CInspectionChattering::ProcInspectionEnd(Inspection::DebugInfo* debugInfo)
{
    if (FALSE == m_parent->m_isTest)
        return;

    debugInfo->SetImage(LanguageDef::Debug_TestImage1, m_bufferThreshold);
}
void CInspectionChattering::LinkDataBase(bool bSave, Database& dbJob)
{
    m_para.LinkDataBase(bSave, m_isUp, dbJob);
}

bool CInspectionChattering::CopyDataBase(CProcessingBase* src)
{
    auto* ptr = dynamic_cast<CInspectionChattering*>(src);
    if (ptr == nullptr)
    {
        ASSERT(!_T("���� Ÿ���� Inspection�� ���簡 �����ϴ�"));
        return false;
    }

    auto& rhs = *ptr;

    m_para = rhs.m_para;

    m_isUp = rhs.m_isUp;

    return true;
}

float CInspectionChattering::GetProfilePeriod_FFT(const std::vector<float>& profile)
{
    IPVM::FastFourierTransform1D fft;
    long profileLength = profile.size();

    std::vector<long> whiteCellLength;
    std::vector<long> blackCellLength;

    long fftLength = 0;

    if (IPVM::Status::success != fft.GetRequiredLength(long(profile.size()), fftLength))
    {
        return 0.f;
    }

    float dc = 0.f;

    if (IPVM::Status::success != IPVM::Mathematics::GetAverage(&profile[0], profile.size(), dc))
    {
        return 0.f;
    }

    std::vector<IPVM::Point_32f_C2> complexResult(fftLength, {0.f, 0.f});

    for (long i = 0; i < profileLength; i++)
    {
        complexResult[i].m_x = profile[i] - dc;
    }

    if (IPVM::Status::success != fft.Forward(fftLength, &complexResult[0]))
    {
        return 0.f;
    }

    std::vector<float> absValue(fftLength);

    for (long i = 0; i < fftLength; i++)
    {
        absValue[i] = sqrt(complexResult[i].m_x * complexResult[i].m_x + complexResult[i].m_y * complexResult[i].m_y);
    }

    const long minFrequency = 3;

    float maxValue = 0;
    long maxID = 0;
    for (long i = minFrequency; i < fftLength / 2 - 1; i++)
    {
        if (absValue[i] >= absValue[i - 1] && absValue[i] >= absValue[i + 1] && absValue[i] > maxValue)
        {
            maxValue = absValue[i];
            maxID = i;
        }
    }
    if (maxID < 1)
        return -1;

    float fMessCenter = 0;
    float fTotal = 0;
    for (long i = maxID - 1; i <= maxID + 1; i++)
    {
        fMessCenter += i * absValue[i];
        fTotal += absValue[i];
    }

    const float maxValueFloat = fMessCenter / fTotal;

    float period = fftLength / maxValueFloat;

    return period;
}

bool CInspectionChattering::CheckPeriod(float Period)
{
    if (m_para.m_nPeriodMax > 0 || m_para.m_nPeriodMin > 0)
    {
        if (m_para.m_nPeriodMin == -1) // AmpMax 보다 큰 경우
        {
            if (Period >= m_para.m_nPeriodMax)
                return true;
            else
                return false;
        }
        else
        {
            if (Period <= m_para.m_nPeriodMax && Period >= m_para.m_nPeriodMin)
            {
                return true;
            }
            else if (Period > m_para.m_nPeriodMax)
            {
                return false;
            }
            else if (Period < m_para.m_nPeriodMin)
            {
                return false;
            }
        }
    }
    else
        return true;
}

bool CInspectionChattering::CheckAmp(float Amp)
{
    if (m_para.m_nAmpMax > 0 || m_para.m_nAmpMin > 0)
    {
        if (m_para.m_nAmpMin == -1) // AmpMax 보다 큰 경우
        {
            if (Amp >= m_para.m_nAmpMax)
                return true;
            else
                return false;
        }
        else
        {
            if (Amp <= m_para.m_nAmpMax && Amp >= m_para.m_nAmpMin)
            {
                return true;
            }
            else if (Amp > m_para.m_nAmpMax)
            {
                return false;
            }
            else if (Amp < m_para.m_nAmpMin)
            {
                return false;
            }
        }
    }
    else
        return true;
}