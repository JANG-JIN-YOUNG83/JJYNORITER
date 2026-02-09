#pragma once
#include "basedef.h"

#include "InspectionBase.h"
#include "InspectionCell.h"
#include "MeasureValue.h"
#include "../VisionModule/Language/LanguageDef.h"
#include "InspectionChatteringPara.h"

class Database;

class __INSPECTOIN_CLASS__ CInspectionChattering : public CInspectionBase
{
public:
    CInspectionChattering(CProcessingBase* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);
    virtual ~CInspectionChattering();

public:
    virtual bool IsInspectionEnabled() const override;
    virtual void PrepareDebugInfo(Inspection::DebugInfo* debugInfo) override;
    virtual void ResetInspection();
    virtual bool ProcInspection(Inspection::DebugInfo* debugInfo) override;
    virtual void ProcInspectionEnd(Inspection::DebugInfo* debugInfo) override;
    virtual void LinkDataBase(bool bSave, Database& dbJob) override;
    virtual bool CopyDataBase(CProcessingBase* src) override;
    CInspectionChatteringPara m_para;
    float GetProfilePeriod_FFT(const std::vector<float>& profile);
    bool CheckPeriod(float Period);
    bool CheckAmp(float Amp);

private:
    CInspectionCell* m_parent;
};
