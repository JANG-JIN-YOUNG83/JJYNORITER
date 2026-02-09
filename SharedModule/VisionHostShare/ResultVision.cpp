#include "stdafx.h"
#include "ResultVision.h"

#include "../CommonShare/ArchiveAllType.h"
#include "../CommonShare/DefectDef.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ResultVision::ResultVision()
{
    m_nState = EMPTY;
    m_nCellindex = -1;

    m_systmVision_Input = SYSTEMTIME();
    m_systmVision_Output = SYSTEMTIME();
    m_strRecipe_ID = _T("");
    m_dInspection_Processing_Time = 0.0f;

    m_strCell_Final_Judge = _T("");
    m_nTotal_Dimension_NG_Count = 0;
    m_nTotal_Surface_NG_Count = 0;
    m_nTotal_Missmatch_NG_Count = 0;

    m_strSeqMeter_FromRW1 = _T("");
    m_strSeqMeter_FromRW2 = _T("");
}

ResultVision::~ResultVision()
{
    m_nState = EMPTY;
    m_nCellindex = -1;

    m_systmVision_Input = SYSTEMTIME();
    m_systmVision_Output = SYSTEMTIME();
    m_strRecipe_ID = _T("");
    m_dInspection_Processing_Time = 0.0f;

    m_strCell_Final_Judge = _T("");
    m_nTotal_Dimension_NG_Count = 0;
    m_nTotal_Surface_NG_Count = 0;
    m_nTotal_Missmatch_NG_Count = 0;

    m_strSeqMeter_FromRW1 = _T("");
    m_strSeqMeter_FromRW2 = _T("");

    m_vecSurface_Reason_All.clear();
    m_vecSurface_Reason_All.shrink_to_fit();

    m_vecDimension_Reason_All.clear();
    m_vecDimension_Reason_All.shrink_to_fit();

    m_vecCamera.clear();
    m_vecCamera.shrink_to_fit();

    m_vecSurface_Reason_All.clear();
    m_vecSurface_Reason_All.shrink_to_fit();

    m_vecMismatch.clear();
    m_vecMismatch.shrink_to_fit();

    m_vecDefect.clear();
    m_vecDefect.shrink_to_fit();

    m_vecOriginalImage.clear();
    m_vecOriginalImage.shrink_to_fit();
}

int ResultVision::GetNgCount()
{
    return m_nTotal_Dimension_NG_Count + m_nTotal_Surface_NG_Count + m_nTotal_Missmatch_NG_Count;
}

void ResultVision::Clear()
{
    m_nState = EMPTY;
    m_nCellindex = -1;

    m_systmVision_Input = SYSTEMTIME();
    m_systmVision_Output = SYSTEMTIME();
    m_strRecipe_ID = _T("");
    m_dInspection_Processing_Time = 0.0f;

    m_strCell_Final_Judge = _T("");
    m_nTotal_Dimension_NG_Count = 0;
    m_nTotal_Surface_NG_Count = 0;
    m_nTotal_Missmatch_NG_Count = 0;

    m_strSeqMeter_FromRW1 = _T("");
    m_strSeqMeter_FromRW2 = _T("");

    m_vecSurface_Reason_All.clear();
    m_vecSurface_Reason_All.shrink_to_fit();

    m_vecDimension_Reason_All.clear();
    m_vecDimension_Reason_All.shrink_to_fit();

    m_vecCamera.clear();
    m_vecCamera.shrink_to_fit();

    m_vecSurface_Reason_All.clear();
    m_vecSurface_Reason_All.shrink_to_fit();

    m_vecMismatch.clear();
    m_vecMismatch.shrink_to_fit();

    m_vecDefect.clear();
    m_vecDefect.shrink_to_fit();

    m_vecOriginalImage.clear();
    m_vecOriginalImage.shrink_to_fit();
}

void ResultVision::Serialize(CommonShare::ArchiveAllType& ar)
    {
    std::vector<CString> vecTextAppearance_Reason_All;
    std::vector<CString> vecTextDimension_Reason_All;

    if (ar.IsStoring())
    {
        for (auto type : m_vecSurface_Reason_All)
        {
            vecTextAppearance_Reason_All.push_back(GetDefectName(type));
        }

        for (auto type : m_vecDimension_Reason_All)
        {
            vecTextDimension_Reason_All.push_back(GetDefectName(type));
        }
    }

    ar.Serialize_Element(m_nCellindex);
    ar.Serialize_Element(m_systmVision_Input);
    ar.Serialize_Element(m_systmVision_Output);
    ar.Serialize_Element(m_strRecipe_ID);
    ar.Serialize_Element(m_dInspection_Processing_Time);

    ar.Serialize_Element(m_strCell_Final_Judge);
    ar.Serialize_Element(m_nTotal_Dimension_NG_Count);
    ar.Serialize_Element(m_nTotal_Surface_NG_Count);
    ar.Serialize_Element(m_nTotal_Missmatch_NG_Count);

    ar.Serialize_Element(vecTextAppearance_Reason_All);
    ar.Serialize_Element(vecTextDimension_Reason_All);
    ar.Serialize_Element(m_vecCamera);
    ar.Serialize_Element(m_vecDimension);
    ar.Serialize_Element(m_vecMismatch);
    ar.Serialize_Element(m_vecDefect);
    ar.Serialize_Element(m_vecOriginalImage);

    if (ar.IsLoading())
    {
        m_vecSurface_Reason_All.clear();
        m_vecDimension_Reason_All.clear();

        for (auto typeText : vecTextAppearance_Reason_All)
        {
            m_vecSurface_Reason_All.push_back(GetDefectType(typeText));
        }

        for (auto typeText : vecTextDimension_Reason_All)
        {
            m_vecDimension_Reason_All.push_back(GetDefectType(typeText));
        }
    }
}

ResultVision& ResultVision::operator=(const ResultVision& src)
{
    m_nState = src.m_nState;
    m_nCellindex = src.m_nCellindex;

    m_systmVision_Input = src.m_systmVision_Input;
    m_systmVision_Output = src.m_systmVision_Output;
    m_strRecipe_ID = src.m_strRecipe_ID;
    m_dInspection_Processing_Time = src.m_dInspection_Processing_Time;

    m_strCell_Final_Judge = src.m_strCell_Final_Judge;
    m_nTotal_Dimension_NG_Count = src.m_nTotal_Dimension_NG_Count;
    m_nTotal_Surface_NG_Count = src.m_nTotal_Surface_NG_Count;
    m_nTotal_Missmatch_NG_Count = src.m_nTotal_Missmatch_NG_Count;

    m_vecSurface_Reason_All = src.m_vecSurface_Reason_All;
    m_vecDimension_Reason_All = src.m_vecDimension_Reason_All;
    m_vecCamera = src.m_vecCamera;
    m_vecDimension = src.m_vecDimension;
    m_vecMismatch = src.m_vecMismatch;
    m_vecDefect = src.m_vecDefect;
    m_vecOriginalImage = src.m_vecOriginalImage;
    m_strSeqMeter_FromRW1 = src.m_strSeqMeter_FromRW1;
    m_strSeqMeter_FromRW2 = src.m_strSeqMeter_FromRW2;

    return *this;
}