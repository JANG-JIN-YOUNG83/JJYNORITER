#pragma once

struct InlineImageInfo
{
public:
	IPVM::Image_8u_C1 iImage;
    IPVM::Image_8u_C1 iImage_sec;
	int64_t imagePosition;
	float m_grabTime_ms;
	SYSTEMTIME m_inputTime;
};

enum class RUN_MODE : int
{
	REAL_GRAB,
	CELL_SIMULATION,
	CUTTING_SIMULATION
};
