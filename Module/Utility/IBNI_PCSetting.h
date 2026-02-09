#pragma once
#include "basedef.h"

namespace Utility
{
	//-------------------------------------------------------------------------------------------
	// Vision 세팅과 상관 없이 현재 PC에 대해 최우선으로 동작하는 설정이다.
	// 이를 통해 나중에 Simulation 과 Real 장비 구분, Hardware Exist 옵션 등을 제공할 예정이다.
	// 
	// 옵션 : Simulation 모드
	// - 시뮬레이션 모드를 사용하면 하단의 서브옵션들이 Simulation 모드에 맞춰서 강제 Fix 된다.
	// - Enable: No Hardware
	// - Enable: Use Relative Path
	// - Enable: Use Local Network For Vision
	// 
	// 서브옵션1 : No Hardware 
	// - Vision Config와 상관없이 설정이 모두 No Hardware 옵션으로 동작
	// - 일일이 장비에서 가져온 Config의 Hardware Exist 옵션을 수정할 필요 없다.
	// 서브옵션2 : Use Relative Path
	// - Config 폴더등을 C:\Intekplus\XXX 폴더로 고정하지 않고 실행파일 경로의 상대 경로로 찾는다.
	// - 시뮬레이션 모드에서 여러개의 프로그램을 띄우기 위해 사용될 수 있다.
	// 서브옵션3 : Use Local Network For Vision
	// - 설정된 IP와 상관없이 TCP/IP Connection을 위해 Local Network (127.0.0.1)을 이용한다.
	//-------------------------------------------------------------------------------------------

	class __UTILITY_CLASS__ IBNI_PCSetting
	{
	public:
		static bool IsEnableSimulationMode();
		static bool IsEnabledSubOption_DoNotSaveVisionImage();
		static bool IsEnabledSubOption_NoHardware();
		static bool IsEnabledSubOption_NoSpcTrans();
		static bool IsEnabledSubOption_UseRelativePathForCIS();
		static bool IsEnabledSubOption_UseLocalNetworkForVision();

		static void SetEnableSimulationMode(bool enable);
		static void SetEnableSubOption_DoNotSaveVisionImage(bool enable);
		static void SetEnableSubOption_NoHardware(bool enable);
		static void SetEnableSubOption_NoSpcTrans(bool enable);
		static void SetEnableSubOption_UseRelativePathForCIS(bool enable);
		static void SetEnableSubOption_UseLocalNetworkForVision(bool enable);
	};
}
