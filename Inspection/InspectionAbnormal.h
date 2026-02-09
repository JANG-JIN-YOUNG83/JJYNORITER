#pragma once
#include "basedef.h"

#include "InspectionBase.h"
#include "InspectionAbnormal.h"
#include "InspectionCell.h"
#include "MeasureValue.h"
#include "../VisionModule/Language/LanguageDef.h"
#include "InspectionAbnormalPara.h"

class CInspectionCell;
class Database;

class __INSPECTOIN_CLASS__ CInspectionAbnormal : public CInspectionBase
{
public:
	CInspectionAbnormal(CProcessingBase* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);
	virtual ~CInspectionAbnormal();

public:
	virtual bool IsInspectionEnabled() const override;
	virtual void ResetInspection();

	virtual void PrepareDebugInfo(Inspection::DebugInfo* debugInfo) override;
	virtual bool ProcInspection(Inspection::DebugInfo* debugInfo) override;
	virtual void ProcInspectionEnd(Inspection::DebugInfo* debugInfo) override;

	virtual void LinkDataBase(bool bSave, Database& dbJob) override;
	virtual bool CopyDataBase(CProcessingBase* src) override;


public:
	float GetProfilePeriod_FFT(const std::vector<float>& profile);
	CInspectionAbnormalPara m_para;
    float m_fPeriod;
    BOOL m_bTwoROI;

private:
	CInspectionCell* m_parent;
};

