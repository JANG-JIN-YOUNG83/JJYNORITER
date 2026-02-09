#pragma once

#include "basedef.h"
#include "MeasureValue.h"
#include "Types/LineSeg_32f.h"
#include "Types/Point_32f_C2.h"
#include "Types/Point_32f_C3.h"
#include "Types/Point_32s_C2.h"
#include "Types/Rect.h"
#include "Types/Rect_32f.h"

#include <vector>

class __INSPECTOIN_CLASS__ UnionCellResult
{
public:
	UnionCellResult();
	~UnionCellResult();

	//////////////////////////////////////////////////////////////////////////////////


	IPVM::Rect_32f		m_rectAreaMeasure[21];		// 
	IPVM::Rect_32f		m_rectAreaBlob[21];			//vector로 변경 Align 영역과 Additional 영역을 계속 넣을 수있게? nwk
	//std::vector<IPVM::Point_32f_C2> m_vecPtMSA;

	std::vector<IPVM::Rect> m_rectResult;

	//std::vector<IPVM::LineEq_64f> m_vecLineMeasure;

	IPVM::Rect_32f		m_rectTab;
	IPVM::Rect_32f		m_rectTab_Left;
	IPVM::Rect_32f		m_rectTab_Right;
	IPVM::Rect_32f		m_rectCell;
	// 지울 것

	//-----------------------------------------------------------------------------
	// Cell Align ���� ����� ����
	//-----------------------------------------------------------------------------

	IPVM::Point_32f_C2	m_ptVHomeInitial[4];
	IPVM::Point_32f_C2	m_ptVHome[4];
	IPVM::Point_32f_C2	m_ptDetialVHome[4];
	//IPVM::Rect_32f		m_rectTab;
	IPVM::Point_32f_C2	m_ptVHomeOuter[4];
	IPVM::Point_32f_C2	m_ptVHomeInner[4];
	IPVM::Rect_32f		m_rectShoulderTab;		///<<< unit:pixel ( Cutting Height Top )
	IPVM::Rect_32f		m_rectShoulderNoTab;	///<<< unit:pixel ( Cutting Height Bot )

	IPVM::Rect			m_rectInsulationCuttingHeightTop;		///<<< unit:pixel ( Cutting Height Top )
	IPVM::Rect			m_rectInsulationCuttingHeightBot;		///<<< unit:pixel ( Cutting Height Bot )

	IPVM::LineSeg_32f	m_lineCellBoundaryLeft;
	IPVM::LineSeg_32f	m_lineCellBoundaryRight;
	IPVM::LineSeg_32f	m_lineSegShoulder;
	IPVM::LineSeg_32f	m_lineCellBoundaryLeft4Shoulder;
	IPVM::LineSeg_32f	m_lineCellBoundaryRight4Shoulder;

	std::vector<IPVM::Point_32f_C2> m_ptsShoulderOrig;
	std::vector<IPVM::Point_32f_C2> m_ptsShoulderMargin;


	std::vector<IPVM::Point_32f_C2> m_ptsCellBoundaryTop;
	std::vector<IPVM::Point_32f_C2> m_ptsCellBoundaryBot;
	std::vector<IPVM::Point_32f_C2> m_ptsCellBoundary;

	//-----------------------------------------------------------------------------
	// Tape ���� ����� ����
	//-----------------------------------------------------------------------------

	//bool m_tapeIsReject;
	bool m_isRejectTape;
	bool m_isRejectUncoatingBoundary;

	//-----------------------------------------------------------------------------
	// Shoulder Line ���� ����� ����
	//-----------------------------------------------------------------------------

	Inspection::MeasureValue m_measureShoulderLine;
	IPVM::Rect m_rectShoulder;
	IPVM::Point_32f_C2 m_ptShoulder1;
	IPVM::Point_32f_C2 m_ptShoulder2;
	IPVM::Rect_32f	m_rtShoulder1;
	IPVM::Rect_32f	m_rtShoulder2;

	//-----------------------------------------------------------------------------
	// Tab Folding ���� ����� ����
	//-----------------------------------------------------------------------------
	IPVM::Rect_32f m_rectTabRough;
	IPVM::Rect_32f m_rectTabForHeight;
	//-----------------------------------------------------------------------------
	// Tab Folding ���� ����� ����
	//-----------------------------------------------------------------------------
	IPVM::Point_32f_C3	m_ptDetialVhomeCenterPoint[4];
	float m_fMisalign_pixel;

	// overlay, insulation
	IPVM::Point_32f_C2 m_ptOverlay;
	IPVM::Point_32f_C2 m_ptInsulation;

	std::vector<float> m_vecfChatteringPeriod;
    std::vector<float> m_vecfChatteringAmp;
    std::vector<IPVM::Rect> m_vecRectChattering;
};
