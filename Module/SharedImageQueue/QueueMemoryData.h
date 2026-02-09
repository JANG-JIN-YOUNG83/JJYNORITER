#pragma once

struct QueueMemoryData
{
	bool m_isBusy_Queue;	// iBNI_SharedImageQueue가 바쁨
	bool m_isBusy_Pusher;	// Pusher가 바쁨
	long m_queueSize;		// Queue 크기
	long m_imageSizeX;		// Image 크기 X
	long m_imageSizeY;		// Image 크기 Y
	long m_headIndex;
	long m_dataCount;

	long m_popIntervalTime_ms;	// Pop시 Delay Time
	long m_AlarmSave_Queue;
};
