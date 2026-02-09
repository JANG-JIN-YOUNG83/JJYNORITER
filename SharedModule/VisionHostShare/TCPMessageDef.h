#pragma once
#include "../../VersionInfo.h"
#include "TCPMSG_I2V.h"
#include "TCPMSG_V2I.h"

enum class TCPMSG_T2I	// AutoMationTest -> Interface
{
	TEST_START = 100,
	TEST_READY = 200,
	TEST_SURFACE_READY = 300,
};

enum class TCPMSG_I2T  // Interface -> AutoMationTest  
{
	TEST_INSPECTION_DONE = 100,
	TEST_SURFACEINSPECTION_DONE = 200,
};
