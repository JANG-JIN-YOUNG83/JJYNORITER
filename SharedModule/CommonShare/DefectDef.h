#pragma once
#include "basedef.h"

/***************************************************************************************
*
*	2021년 05월 30일 불량 항목 정의(표)
*   2021년 06월 02일 절연폭, 오버레이 상부하부 추가
*   2021년 06월 04일 전장 좌우측 추가
*   2021년 08월 09일 Long Cell 항목 추가 및 오버레이 변경
*   -주의사항-
*   오류가 있어도 절대 수정 금지
*   수정이 필요할 경우 관리자(4사업부 담당)에게 이야기하여 수정 및 배포 되도록 해주세요.
*
***************************************************************************************/

enum OtherDefect
{
	OD_NONE = 0,
	OD_PET = 1,
	OD_TEAR = 2,
	OD_BURR = 3,
	OD_UP_ALIGN_FAIL = 4,
	OD_UP_TIME_OUT = 5,
	OD_DOWN_ALIGN_FAIL = 6,
	OD_DOWN_TIME_OUT = 7,
	OD_ABNORMAL = 8,

};
// 현식 불량 타입
//승만
enum class DefectType
{

	START_Coater_Dimension,

#if INSPECTION_MACHINE_UC1
    // UC 코터 dimension 검사 항목 순서
    Width_FullLength, // A
    Width_LeftCoating, // B?
    Width_RightCoating, // C
    Width_LeftFoil, // D
    Width_MiddleFoil, // E
    Width_MiddleFoilLeftSlitting, // E1
    Width_MiddleFoilRightSlitting, // E2
    Width_RightFoil, // F
    //Width_Mismatch, // Mismatch
    Point_EdgeDetectPoint,

#else
    Width_FullLength, // A
    Width_Coating_1, // B?
    Width_Coating_2, // C
    Width_NonCoating_1, // D
    Width_MiddleFoil, // E
    Width_MiddleFoilLeftSlitting, // E1
    Width_MiddleFoilRightSlitting, // E2
    Width_NonCoating_2, // F
	Width_Insulation_Coating_Lane1_L,
    Width_Insulation_Coating_Lane1_R,
    Width_Insulation_Coating_Lane2_L,
    Width_Insulation_Coating_Lane2_R,
    Width_Insulation_Overlay_Lane1_L,
    Width_Insulation_Overlay_Lane1_R,
    Width_Insulation_Overlay_Lane2_L,
    Width_Insulation_Overlay_Lane2_R,
    //Width_Mismatch, // Mismatch
    Point_EdgeDetectPoint,

#endif // INSPECTION_MACHINE_UC1
	
    End_Coater_Dimension,

	// UC 표면 검사 항목 순서
	START_Coater_Surface,
 //   White_Line,
 //   White_Pinhole,
 //   Dark_Line,
 //   Scratch,
 //   Crack,
 //   Dark_Crater,
 //   Crater,
 //   Protrusion,
 //   Dent,
 //   Balck_Dot,
 //   Delami,
 //   Bangking,

 //   ETC_Defect,
 //   Edge_ChippingPinhole,
 //   Edge_Drag,
 //   Edge_Line,
 //   Edge_Defect,
 //   Foil_Island,
 //   Foil_Defect,
 //   END_Coater_Surface,


	//START_Coater_Surface,
 //   Line, // White_Line
 //   Pinhole, // White_Pinhole
 //   Horizontal_Line, // Dark_Line
 //   Scratch, //Scratch
 //   Crack, //Crack
 //   Stain, // Dark_Crater
    
#if INSPECTION_MACHINE_UC1
    White_Line,
    White_Pinhole,
    Dark_Line,
    Scratch,
    Crack,
    Dark_Crater,
    Crater,
    Protrusion,
    Dent,
    Balck_Dot,
    Delami,
    Banking,

    ETC_Defect,
    Edge_ChippingPinhole,
    Edge_Drag,
    Edge_Line,
    Edge_Defect,
    Foil_Island,
    Foil_Defect,
#else
    Line, // White_Line
    Pinhole, // White_Pinhole
    Horizontal_Line, // Dark_Line
    Scratch, //Scratch
    Crack, //Crack
    Stain, // Dark_Crater
    Stain_Pinhole, // Crater
    Delamination_Crack, // Protrusion
    Dent, // Dent
    Balck_Dot,
    Delamination_Pinhole, // Delami
    Banking, // Banking

    ETC_Defect,
    Chipping_Pinhole, // Edge_ChippingPinhole
    Drag, //Edge_Drag
    Edge_Line,
    Edge_Defect,
    Island, // Foil_Island
    Plain_Wrinkle, // Foil_Defect
#endif // INSPECTION_MACHINE_UC1

	//Crater,
 //   Delamination_Crack, // Protrusion
 //   Dent, // Dent
 //   Balck_Dot,
 //   Delamination_Pinhole, // Delami
 //   Banking, // Banking

 //   ETC_Defect,
 //   Chipping_Pinhole, // Edge_ChippingPinhole
 //   Drag, //Edge_Drag
 //   Edge_Line,
 //   Edge_Defect,
 //   Island, // Foil_Island
 //   Plain_Wrinkle, // Foil_Defect
    END_Coater_Surface,

    //// LGES 표면 검사 항목 순서
    //START_Coater_Surface_LGES,
    //Line, // White_Line
    //Pinhole, // White_Pinhole
    //Horizontal_Line, // Dark_Line
    //Scratch, //Scratch
    //Crack, //Crack
    //Stain, // Dark_Crater
    //Stain_Pinhole, // Crater
    //Delamination_Crack, // Protrusion
    //Dent, // Dent
    //Balck_Dot,
    //Delamination_Pinhole, // Delami
    //Banking, // Banking

    //ETC_Defect,
    //Chipping_Pinhole, // Edge_ChippingPinhole
    //Drag, //Edge_Drag
    //Edge_Line,
    //Edge_Defect,
    //Island, // Foil_Island
    //Plain_Wrinkle, // Foil_Defect
    //END_Coater_Surface_LGES,



	//// 검사 항목 순서
	////UC1
 //   START_Coater_Surface_UC1,
	//Line_UC1,
	//Boundary_Line_UC1,
	//Pinhole_UC1,
	//Pinhole_Crater_UC1,
	//Non_Coating_UC1,
 //   Insulation_Line_UC1,
 //   Insulation_GapLine_UC1,
 //   Insulation_Pinhole_UC1,
 //   Insulation_GapPinhole_UC1,
 //   Crater_UC1,
 //   Crack_UC1,
 //   Scratch_UC1,
 //   Heating_Wrinkle_UC1,
 //   Coating_Wrinkle_UC1,
 //   Dent_UC1,
 //   Protrusion_UC1,
 //   Island_UC1,
 //   Insulation_Island_UC1,
 //   Drag_UC1,
 //   NonCoating_Wrinkle_UC1,
 //   NonCoating_Wave_UC1,
 //   Pierced_Pinhole_UC1,
 //   END_Coater_Surface_UC1,

    //// 기존 검사 항목 순서 -------------- 
    //   START_Coater_Surface,
    //   White_Line,
    //   White_Pinhole,
    //   Dark_Line,
    //   Scratch,
    //   Crack,
    //   Dark_Crater,
    //   Crater,
    //   Protrusion,
    //   Dent,
    //   Balck_Dot,
    //   Delami,
    //   Bangking,

    //   ETC_Defect,
    //   Edge_ChippingPinhole,
    //   Edge_Drag,
    //   Edge_Line,
    //   Edge_Defect,
    //   Foil_Island,
    //   Foil_Defect,
    //   END_Coater_Surface,
	
	//// 코터 surface
	//Coating_Line,
	//Coating_Scratch,
	//Coating_Dent, // 일단
	//Coating_Pinhole,
	//Coating_Delamination_Crack,
	//Coating_EdgeUncoated, //(파임)
	//Coating_Uncoated, //(미코팅)
	//Coating_Crater,
	//Coating_Crack,
	//Coating_HeatingWrinkle,
	//Coating_CoatingWrinkle,
	//Island,
	//Foil_Drag,
	// 
	// Mismatch 정의 : 이거 자동으로 늘고 줄어야 하는데...
	Start_Dimension_Mismatch,
	Dimension_Mismatch1,
	Dimension_Mismatch2,
	Dimension_Mismatch3,
	Dimension_Mismatch4,
	Dimension_Mismatch5,
	Dimension_Mismatch6,
	Dimension_Mismatch7,
	Dimension_Mismatch8,
	End_Dimension_Mismatch,
	// end 코터
	//Pinhole,
	TabWidth,
	TabWidth_A,
	TabPitch,
	TabPitch_2,
	TabHeight,
	Top_TabHeight_2,
	Bottom_TabHeight_2,
	TabSide,
	TopCoating,
	BottomCoating,
	Mismatch,
	TopTilt,
	BottomTilt,
	Misalign,
	LeftLength,
	RightLength,
	Top_Length_2,
	Bottom_Length_2,
	TopInsulationWidth,
	BottomInsulationWidth,
	TopFrontOverlayWidth,
	TopBackOverlayWidth,
	TimeOut,
	ConnectionTape,
	DarkConnectionTape,
	NoneCoating,
	TabShape,
	TabShape_A,

	//Line,
	

	HeatingWrinkle,
	CoatingWrinkle,


	InsulationIsland,

	Wrinkle,
	Dross, //0910 표면검사 항목 추가
	Spatter, //0910 표면검사 항목 추가
	TopScrab, //0930 표면검사 ESWA 항목 추가
	BotScrab,

	VHomePitch,
	// LongCell 추가항목
	TopFrontCuttingHeight,
	TopBackCuttingHeight,
	BottomFrontCuttingHeight,
	BottomBackCuttingHeight,
	BottomFrontOverlayWidth,
	BottomBackOverlayWidth,
	TopVHomeHeight,
	BottomVHomeHeight,
	//---------------------------------------------------------------
	// IBNI_Interface
	//---------------------------------------------------------------
	SurfaceDefect,
	ElectrodeDefect,
	NotchingDefect,
	// 자동보정sw csv남기기 위해서 만든 define
	TopMismatch,
	BottomMismatch,
	//9.14 음극(GMLV)
	RightTabSide,
	LeftTabSide,

#ifdef _INSULATESURFACE_
	NG_Tag,
	//Drag,
	Burr,
	//Island,
	InsulationLine,
	InsulationGapLine,
	InsulationPinhole,
	InsulationGapPinhole,
	Spur,
	Seam,
	Scrab,
	TabWrinkle,

	END,
#else
	END,

	NG_Tag,
	Drag,
	Burr,
	Island,
	InsulationLine,
	InsulationGapLine,
	InsulationPinhole,
	InsulationGapPinhole,
	Spur,
	Seam,
	Scrab,
	TabWrinkle,
#endif
	SmallDefect_CraterLevel1,
	SmallDefect_CraterLevel2,
	SmallDefect_CraterLevel3,
	SmallDefect_CraterLevel4,
	SmallDefect_CraterLevel5,
	SmallDefect_PinholeLevel1,
	SmallDefect_PinholeLevel2,
	SmallDefect_PinholeLevel3,
	SmallDefect_PinholeLevel4,
	SmallDefect_PinholeLevel5,
    SmallDefect_DentLevel1,
    SmallDefect_DentLevel2,
    SmallDefect_DentLevel3,
    SmallDefect_DentLevel4,
    SmallDefect_DentLevel5,
    SmallDefect_ProtrutionLevel1,
    SmallDefect_ProtrutionLevel2,
    SmallDefect_ProtrutionLevel3,
    SmallDefect_ProtrutionLevel4,
    SmallDefect_ProtrutionLevel5,
	SmallDefect_IslandLevel1,
	SmallDefect_IslandLevel2,
	SmallDefect_IslandLevel3,
	SmallDefect_IslandLevel4,
	SmallDefect_IslandLevel5,
    SmallDefect_ScratchLevel1,
    SmallDefect_ScratchLevel2,
    SmallDefect_ScratchLevel3,
    SmallDefect_ScratchLevel4,
    SmallDefect_ScratchLevel5,

	ISI_END,
	Defect_Info,	//ISI_END 뒤에 있어야함.
};

enum ensection
{
	NONE = 0,
	NOTCHING = 1,
	ELECTRODE = 2,
	SECTION_UNKOWN = 3,
	END_SECTION,
};

LPCTSTR		__COMMON_SHARE_CLASS__ GetDefectName(DefectType type);
LPCTSTR		__COMMON_SHARE_CLASS__ GetDefectName_Before(DefectType type);
LPCTSTR		__COMMON_SHARE_CLASS__ GetDefectName_UC(DefectType type);
LPCTSTR		__COMMON_SHARE_CLASS__ GetDefectCode(DefectType type); //221108
DefectType	__COMMON_SHARE_CLASS__ GetDefectType(LPCTSTR defectName);
CString		__COMMON_SHARE_CLASS__ GetDefectType2String(ensection section);
ensection   __COMMON_SHARE_CLASS__ GetDefectType2enum(CString strType);

//long		__COMMON_SHARE_CLASS__ GetDefectRank(DefectType type);
//ensection	__COMMON_SHARE_CLASS__ GetDefectSection(DefectType type);
//CString		__COMMON_SHARE_CLASS__ GetDefectSection2String(DefectType type);

#define STR_TAB_WIDTH								GetDefectName(DefectType::TabWidth)
#define STR_TAB_WIDTH_A								GetDefectName(DefectType::TabWidth_A)
#define STR_TAB_PITCH								GetDefectName(DefectType::TabPitch)
#define STR_TAB_PITCH_2								GetDefectName(DefectType::TabPitch_2)
#define STR_TAB_HEIGHT								GetDefectName(DefectType::TabHeight)
#define STR_TOP_TAB_HEIGHT_2						GetDefectName(DefectType::Top_TabHeight_2)
#define STR_BOTTOM_TAB_HEIGHT_2						GetDefectName(DefectType::Bottom_TabHeight_2)
#define STR_TAB_SIDE								GetDefectName(DefectType::TabSide)
#define STR_TOP_COATING								GetDefectName(DefectType::TopCoating)
#define STR_BOTTOM_COATING							GetDefectName(DefectType::BottomCoating)
#define STR_MISMATCH								GetDefectName(DefectType::Mismatch)
#define STR_TOP_TILT								GetDefectName(DefectType::TopTilt)
#define STR_BOTTOM_TILT								GetDefectName(DefectType::BottomTilt)
#define STR_MISALIGN								GetDefectName(DefectType::Misalign)
#define STR_LEFT_LENGTH								GetDefectName(DefectType::LeftLength)
#define STR_RIGHT_LENGTH							GetDefectName(DefectType::RightLength)
#define STR_TOP_LENGTH_2							GetDefectName(DefectType::Top_Length_2)
#define STR_BOTTOM_LENGTH_2							GetDefectName(DefectType::Bottom_Length_2)
#define STR_TOP_INSULATION_WIDTH					GetDefectName(DefectType::TopInsulationWidth)
#define STR_BOTTOM_INSULATION_WIDTH					GetDefectName(DefectType::BottomInsulationWidth)
#define STR_TOP_FRONT_OVERLAY_WIDTH					GetDefectName(DefectType::TopFrontOverlayWidth)
#define STR_TOP_BACK_OVERLAY_WIDTH					GetDefectName(DefectType::TopBackOverlayWidth)
#define STR_BURR									GetDefectName(DefectType::Burr) //20231014 [jjh]
#define STR_TIMEOUT									GetDefectName(DefectType::TimeOut)
#define STR_CONNECTION_TAPE							GetDefectName(DefectType::ConnectionTape)
#define STR_BARCORD_MARKS							GetDefectName(DefectType::DarkConnectionTape)
#define STR_NONE_COATING							GetDefectName(DefectType::NoneCoating)
#define STR_TAB_SHAPE								GetDefectName(DefectType::TabShape)
#define STR_TAB_SHAPE_A								GetDefectName(DefectType::TabShape_A)
#define STR_INSULATION_LINE							GetDefectName(DefectType::InsulationLine)   //20230918 [jjh]
#define STR_INSULATION_PINHOLE						GetDefectName(DefectType::InsulationPinhole)   //20230918 [jjh]
#define STR_LINE									GetDefectName(DefectType::Line)
#define STR_SCRATCH									GetDefectName(DefectType::Scratch)
#define STR_PINHOLE									GetDefectName(DefectType::Pinhole)
#define STR_DENT									GetDefectName(DefectType::Dent)
#define STR_CRATER									GetDefectName(DefectType::Stain_Pinhole)
#define STR_CRACK									GetDefectName(DefectType::Crack)
#define STR_HEATING_WRINKLE							GetDefectName(DefectType::HeatingWrinkle)
#define STR_COATING_WRINKLE							GetDefectName(DefectType::CoatingWrinkle)
#define STR_Delamination_Crack								GetDefectName(DefectType::Delamination_Crack)
#define STR_INSULATION_ISLAND						GetDefectName(DefectType::InsulationIsland)
#define STR_WRINKLE									GetDefectName(DefectType::Wrinkle)
#define STR_DROSS									GetDefectName(DefectType::Dross) //0910 표면검사 항목 추가
#define STR_SPATTER									GetDefectName(DefectType::Spatter) //0910 표면검사 항목 추가
#define STR_TOP_SCRAB								GetDefectName(DefectType::TopScrab) //0930 표면검사 ESWA 항목 추가
#define STR_BOT_SCRAB								GetDefectName(DefectType::BotScrab) //0930 표면검사 ESWA 항목 추가
#define STR_VHOME_PITCH								GetDefectName(DefectType::VHomePitch)
#define	STR_TOP_FRONT_CUTTING_HEIGHT				GetDefectName(DefectType::TopFrontCuttingHeight)
#define STR_TOP_BACK_CUTTING_HEIGHT					GetDefectName(DefectType::TopBackCuttingHeight)
#define STR_BOTTOM_FRONT_CUTTING_HEIGHT				GetDefectName(DefectType::BottomFrontCuttingHeight)
#define STR_BOTTOM_BACK_CUTTING_HEIGHT				GetDefectName(DefectType::BottomBackCuttingHeight)
#define STR_BOTTOM_FRONT_OVERLAY_WIDTH				GetDefectName(DefectType::BottomFrontOverlayWidth)
#define STR_BOTTOM_BACK_OVERLAY_WIDTH				GetDefectName(DefectType::BottomBackOverlayWidth)
#define STR_TOP_VHOME_HEIGHT						GetDefectName(DefectType::TopVHomeHeight)
#define STR_BOTTOM_VHOME_HEIGHT						GetDefectName(DefectType::BottomVHomeHeight)

#define STR_SURFACE_DEFECT							GetDefectName(DefectType::SurfaceDefect)
#define STR_ELECTRODE_DEFECT						GetDefectName(DefectType::ElectrodeDefect)
#define STR_NOTCHING_DEFECT							GetDefectName(DefectType::NotchingDefect)

#define STR_TOP_MISMATCH							GetDefectName(DefectType::TopMismatch)
#define STR_BOTTOM_MISMATCH							GetDefectName(DefectType::BottomMismatch)

#define STR_RIGHT_TAB_SIDE							GetDefectName(DefectType::RightTabSide)   //9.14 추가(음극 GMLV)
#define STR_LEFT_TAB_SIDE							GetDefectName(DefectType::LeftTabSide)   //9.14 추가(음극 GMLV)
