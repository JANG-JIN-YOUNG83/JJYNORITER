#pragma once
#include "basedef.h"

//----------------------------------------------------------------------------------------------------
// ExpirationHours: 만료시간
// Free Rate: 드라이브의 여유용량 % 설정
// ExpirationHours와 Free Rate는 AND 연산이다.
// 만료일이 넘은 파일만 후보가 되며, 여유 용량이 부족할시 후보중 파일들을 삭제한다
//----------------------------------------------------------------------------------------------------

namespace AutoCleaner
{
	class __AUTO_CLEANER_CLASS__ Module
	{
	public:
		Module(LPVOID userData, CALLBACK_CLEANER_STATE_CHANGED* callbackStateChanged);
		~Module();

		long FindNotEnoughDrive(float freeRate) const;
		void MonitoringFolder_Reset();
		void MonitoringFolder_Add(long expirationHours, LPCTSTR folderPath, BOOL bOnOff, BOOL bAlways);
		void RemoveConditionsForDeletion();
		void SetDriveFreeRate(char drive, float freeRate);

		void Start();
		void Stop();

	private:
		class Impl;
		Impl* m_impl;
	};
}
