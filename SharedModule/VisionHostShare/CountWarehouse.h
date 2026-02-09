#pragma once

#include "basedef.h"
#include <map>
#include <vector>

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ DefectCountInfo : public CommonShare::BaseObject
	{
	public:
		DefectCountInfo();

		long m_ninspType;
		long m_npairType;	// 검사 항목이 쌍으로 이루어져있는지 .... (DefectType::END 이면 Pair 없음)
		BOOL bPairCheck;	// 임시 변수 처럼 사용
		long nAINGCount;

		long NG_COUNT[2][2];	// 0 no ink marking, 1 ink marking
		long PAIR_NG_COUNT[2][2];	// 0 no ink marking, 1 ink marking
		
		DefectCountInfo& operator = (const DefectCountInfo& src);
		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

	};

	class __VISION_HOST_SHARE_CLASS__ CountWarehouse : public CommonShare::BaseObject
	{
	public:
		CountWarehouse();
		~CountWarehouse();

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
		CountWarehouse& operator = (const CountWarehouse& src);



		std::vector<Share::DefectCountInfo> vecDefectCount;

		long nTotal;
		long nReject;
		long nGood;
		long nInvalid;

		long nRejectDimension;	//치수
		long nRejectDefect;	//roi
		long nRejectTotal;	//

		CString strLot;
		CString strRoll;

	};
}
