#pragma once

#include "basedef.h"
#include "InspMergeOptions.h"
#include "InspectionCriteriaList.h"
#include "InspectionRoot.h"
#include "OptionCriteriaList.h"
#include "UrgentVariable.h"
#include "MeasureOffsets.h"
#include "CISISpecTable.h"

namespace Inspection
{
	class __INSPECTOIN_CLASS__ Accessor
	{
	public:
		~Accessor();

		static void DelInstance();
		static void InitInstance(IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);

		static Accessor& Get();
		static Root& GetRoot();

		bool m_isMSA_Mode;					// 현재 Inspection은 MSA 모드로 동작중이다
		UrgentVariable m_urgentVariable;	// 요청에 의해 긴급히 추가된 임시변수 (추후 없어졌으면 좋겠다)

		// Recipe에 저장되는 파라메터 (Root가 로드될때 로드된다)
		MeasureOffsets			m_measureOffsets;
		InspMergeOptions		m_inspMergeOptions;
		InspectionCriteriaList	m_criteriaList;
		OptionCriteriaList		m_optionCriteriaList;

		CISISpecTable			m_isi_spec;
		CString m_strPrevJobFolder;
		CString m_strPrevImageFolder;

	private:
		Accessor(IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);

		Root m_root;

		static Accessor* m_instance;
	};
}
