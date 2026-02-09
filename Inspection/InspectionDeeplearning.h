#pragma once

#include "basedef.h"

#include "InspectionBase.h"
#include "InspectionDeeplearningPara.h"
#include "InspectionCell.h"
#include "MeasureValue.h"
#include "../VisionModule/Language/LanguageDef.h"
class Database;

class __INSPECTOIN_CLASS__ CInspectionDeeplearning : public CInspectionBase
{
public:
	CInspectionDeeplearning(CProcessingBase* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);
	virtual ~CInspectionDeeplearning();

public:
	virtual bool IsInspectionEnabled() const override;
	virtual void ResetInspection();

	virtual void PrepareDebugInfo(Inspection::DebugInfo* debugInfo) override;
	virtual bool ProcInspection(Inspection::DebugInfo* debugInfo) override;
	virtual void ProcInspectionEnd(Inspection::DebugInfo* debugInfo) override;

	virtual void LinkDataBase(bool bSave, Database &dbJob) override;
	virtual bool CopyDataBase(CProcessingBase* src) override;


public:
	CInspectionDeeplearningPara m_para;

private:
	CInspectionCell* m_parent;
};
