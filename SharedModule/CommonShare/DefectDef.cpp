#include "stdafx.h"
#include "DefectDef.h"

//#ifdef _DEBUG
//#	define new DEBUG_NEW
//#	undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


// 현식 크래시피케이션 이름 여기에 있음....
LPCTSTR GetDefectName(DefectType type)
{
	switch (type)
	{
#if INSPECTION_MACHINE_UC1
		case DefectType::Width_LeftCoating:					return _T("Coating Width 1");					// 왼쪽 코팅부
		case DefectType::Width_RightCoating:				return _T("Coating Width 2");					// 오른쪽 코팅부
		case DefectType::Width_LeftFoil:					return _T("Non-Coating Width 1");						// 왼쪽 포일부
		case DefectType::Width_RightFoil:					return _T("Non-Coating Width 2");					// 오른쪽 포일부
#else
		case DefectType::Width_Coating_1:					return _T("Coating Width 1");					// 왼쪽 코팅부
		case DefectType::Width_Coating_2:					return _T("Coating Width 2");					// 오른쪽 코팅부
		case DefectType::Width_NonCoating_1:				return _T("Non-Coating Width 1");						// 왼쪽 포일부
		case DefectType::Width_NonCoating_2:				return _T("Non-Coating Width 2");					// 오른쪽 포일부
        case DefectType::Width_Insulation_Coating_Lane1_L:	return _T("Insulation Coating 1Lane Left");			// 1Lane의 Left 방향 절연 코팅 폭
        case DefectType::Width_Insulation_Coating_Lane1_R:	return _T("Insulation Coating 1Lane Right");		// 1Lane의 Right 방향 절연 코팅 폭
        case DefectType::Width_Insulation_Coating_Lane2_L:	return _T("Insulation Coating 2Lane Left");			// 2Lane의 Left 방향 절연 코팅 폭
        case DefectType::Width_Insulation_Coating_Lane2_R:	return _T("Insulation Coating 2Lane Right");		// 2Lane의 Right 방향 절연 코팅 폭
		case DefectType::Width_Insulation_Overlay_Lane1_L:	return _T("Insulation Overlay 1Lane Left");			// 1Lane의 Left 방향 오버레이 폭
        case DefectType::Width_Insulation_Overlay_Lane1_R:	return _T("Insulation Overlay 1Lane Right");		// 1Lane의 Right 방향 오버레이 폭
        case DefectType::Width_Insulation_Overlay_Lane2_L:	return _T("Insulation Overlay 2Lane Left");			// 2Lane의 Left 방향 오버레이 폭
        case DefectType::Width_Insulation_Overlay_Lane2_R:	return _T("Insulation Overlay 2Lane Right");		// 2Lane의 Right 방향 오버레이 폭
#endif // INSPECTION_MACHINE_UC1

	
	case DefectType::START_Coater_Dimension:			return _T("Start Coater");
	case DefectType::Width_FullLength:					return _T("Full Length");					// 전장
	case DefectType::Width_MiddleFoil:					return _T("Middle Foil");					// 가운데 포일부
	case DefectType::Width_MiddleFoilLeftSlitting:		return _T("Middle Foil Left Slitting");		// 가운데 포일부 中 중앙 기준 왼쪽
	case DefectType::Width_MiddleFoilRightSlitting:		return _T("Middle Foil Right Slitting");	// 가운데 포일부 中 중앙 기준 오른쪽
	case DefectType::Point_EdgeDetectPoint:				return _T("Detect Edge Point");				// 미스매치 계산을 위한 Detect Edge point
	case DefectType::End_Coater_Dimension:				return _T("End Coater");

	case DefectType::START_Coater_Surface:  return _T("Start Coater Surface");
	
#if INSPECTION_MACHINE_UC1
    case DefectType::White_Line:
        return _T("White_Line");
    case DefectType::White_Pinhole:
        return _T("White_Pinhole");
    case DefectType::Dark_Line:
        return _T("Dark_Line");
    case DefectType::Scratch:
        return _T("Scratch");
    case DefectType::Crack:
        return _T("Crack");
    case DefectType::Dark_Crater:
        return _T("Dark_Crater");
    case DefectType::Crater:
        return _T("Crater");
    case DefectType::Protrusion:
        return _T("Protrusion");
    case DefectType::Dent:
        return _T("Dent");
    case DefectType::Balck_Dot:
        return _T("Black_Dot");
    case DefectType::Delami:
        return _T("Delami");
    case DefectType::Banking:
        return _T("Banking");
    case DefectType::ETC_Defect:
        return _T("ETC_Defect");
    case DefectType::Edge_ChippingPinhole:
        return _T("Edge_ChippingPinhole");
    case DefectType::Edge_Drag:
        return _T("Edge_Drag");
    case DefectType::Edge_Line:
        return _T("Edge_Line");
    case DefectType::Edge_Defect:
        return _T("Edge_Defect");
    case DefectType::Foil_Island:
        return _T("Foil_Island");
    case DefectType::Foil_Defect:
        return _T("Foil_Defect");

#else
    case DefectType::Line:
        return _T("Line");
    case DefectType::Pinhole:
        return _T("Pinhole");
    case DefectType::Horizontal_Line:
        return _T("Horizontal_Line");
    case DefectType::Scratch:
        return _T("Scratch");
    case DefectType::Crack:
        return _T("Crack");
    case DefectType::Stain:
        return _T("Stain");
    case DefectType::Stain_Pinhole:
        return _T("Stain_Pinhole");
    case DefectType::Delamination_Crack:
        return _T("Delamination_Crack");
    case DefectType::Dent:
        return _T("Dent");
    case DefectType::Balck_Dot:
        return _T("Black_Dot");
    case DefectType::Delamination_Pinhole:
        return _T("Delamination_Pinhole");
    case DefectType::Banking:
        return _T("Banking");
    case DefectType::Island:
        return _T("Island");
    case DefectType::Chipping_Pinhole:
        return _T("Chipping_Pinhole");
    case DefectType::Drag:
        return _T("Drag");
    case DefectType::Edge_Line:
        return _T("Edge_Line");
    case DefectType::ETC_Defect:
        return _T("ETC_Defect");
    case DefectType::Plain_Wrinkle:
        return _T("Plain_Wrinkle");
    case DefectType::Edge_Defect:
        return _T("Edge_Defect");

#endif

    case DefectType::END_Coater_Surface:		return _T("End Coater Surface");
	//case DefectType::White_Line:			return _T("White_Line");
	//case DefectType::White_Pinhole:			return _T("White_Pinhole");
	//case DefectType::Dark_Line:				return _T("Dark_Line");
	//case DefectType::Scratch:				return _T("Scratch");
	//case DefectType::Crack:					return _T("Crack");
	//case DefectType::Dark_Crater:			return _T("Dark_Crater");
	//case DefectType::Crater:				return _T("Crater");
	//case DefectType::Protrusion:			return _T("Protrusion");
	//case DefectType::Dent:					return _T("Dent");
	//case DefectType::Balck_Dot:				return _T("Black_Dot");
	//case DefectType::Delami:				return _T("Delami");
	//case DefectType::Banking:				return _T("Banking");
	//case DefectType::ETC_Defect:			return _T("ETC_Defect");
	//case DefectType::Edge_ChippingPinhole:	return _T("Edge_ChippingPinhole");
	//case DefectType::Edge_Drag:				return _T("Edge_Drag");
	//case DefectType::Edge_Line:				return _T("Edge_Line");
	//case DefectType::Edge_Defect:			return _T("Edge_Defect");
	//case DefectType::Foil_Island:			return _T("Foil_Island");
	//case DefectType::Foil_Defect:			return _T("Foil_Defect");

	//case DefectType::Coating_Line:						return _T("Coating Line");					// 라인
	//case DefectType::Coating_Scratch:					return _T("Coating Scratch");				// 스크래치
	//case DefectType::Coating_Dent:						return _T("Coating Dent");					// 찍힘
	//case DefectType::Coating_Pinhole:					return _T("Coating Pinhole");				// 핀홀
	//case DefectType::Coating_Delamination_Crack:				return _T("Coating Delamination_Crack");			// 돌출
	//case DefectType::Coating_EdgeUncoated:				return _T("Coating EdgeUncoated");			// 파임
	//case DefectType::Coating_Uncoated:					return _T("Coating Uncoated");				// 미코팅
	//case DefectType::Coating_Crater:					return _T("Coating Crater");				// 분화구
	//case DefectType::Coating_Crack:						return _T("Coating Crack");					// 크랙
	//case DefectType::Coating_HeatingWrinkle:			return _T("Coating HeatingWrinkle");		// 열주름
	//case DefectType::Coating_CoatingWrinkle:			return _T("Coating CoatingWrinkle");		// 코팅주름
	//case DefectType::Island:						return _T("Foil Island");					// 포일 아일랜드
	//case DefectType::Foil_Drag:							return _T("Foil Drag");						// 포일 드래그

	case DefectType::Start_Dimension_Mismatch:			return _T("Start Mismatch");				
	case DefectType::Dimension_Mismatch1:				return _T("Mismatch #1");				
	case DefectType::Dimension_Mismatch2:				return _T("Mismatch #2");				
	case DefectType::Dimension_Mismatch3:				return _T("Mismatch #3");					
	case DefectType::Dimension_Mismatch4:				return _T("Mismatch #4");		
	case DefectType::Dimension_Mismatch5:				return _T("Mismatch #5");		
	case DefectType::Dimension_Mismatch6:				return _T("Mismatch #6");				
	case DefectType::Dimension_Mismatch7:				return _T("Mismatch #7");				
	case DefectType::Dimension_Mismatch8:				return _T("Mismatch #8");					
	case DefectType::End_Dimension_Mismatch:			return _T("End Mismatch");					


	case DefectType::TabWidth:					return _T("Tab Width");								// 탭 폭 (CIS비전에서 주는 결과)
	case DefectType::TabWidth_A:				return _T("Tab Width(Area)");						// 탭 폭 (치수비전에서 주는 결과)
	case DefectType::TabPitch:					return _T("Tab Pitch");								// 탭 피치 (CIS비전에서 주는 결과)
	case DefectType::TabPitch_2:				return _T("Tab Pitch(Area)");						// 탭 피치 (치수비전에서 주는 결과)
	case DefectType::TabHeight:					return _T("Tab Height");							// 탭 높이
	case DefectType::Top_TabHeight_2:			return _T("Top Tab Height 2");						// 상부 탭 높이(ESWA 요청)
	case DefectType::Bottom_TabHeight_2:		return _T("Bottom Tab Height 2");					// 하부 탭 높이(ESWA 요청)
	case DefectType::TabSide:					return _T("Tab Side");								// 탭 사이드
	case DefectType::TopCoating:				return _T("Top Coating");							// 상부 어깨선
	case DefectType::BottomCoating:				return _T("Bottom Coating");						// 하부 어깨선
	case DefectType::Mismatch:					return _T("Mismatch");								// 어깨선 차이값
	case DefectType::TopTilt:					return _T("Top Tilt");								// V홈 단차 상부
	case DefectType::BottomTilt:				return _T("Bottom Tilt");							// V홈 단차 하부
	case DefectType::Misalign:					return _T("Misalign");								// V홈 차이값
	case DefectType::LeftLength:				return _T("Left Length");							// 좌측 전장
	case DefectType::RightLength:				return _T("Right Length");							// 우측 전장
	case DefectType::Top_Length_2:				return _T("Top Length 2");							// 좌측 전장(ESWA 요청)
	case DefectType::Bottom_Length_2:			return _T("Bottom Length 2");						// 우측 전장(ESWA 요청)
	case DefectType::TopInsulationWidth:		return _T("Top Insulation Width");					// 상부절연 폭
	case DefectType::BottomInsulationWidth:		return _T("Bottom Insulation Width");				// 하부절연 폭
	case DefectType::TopFrontOverlayWidth:		return _T("Top Front Overlay Width");				// 상부오버레이 폭 (변경)
	case DefectType::TopBackOverlayWidth:		return _T("Top Back Overlay Width");				// 하부오버레이 폭 (변경)
	case DefectType::TimeOut:					return _T("Time Out");								// 타임 아웃
	case DefectType::ConnectionTape:			return _T("Connection Tape");						// 연결 테이프
	case DefectType::DarkConnectionTape:		return _T("Barcode Marks");							// 다크 연결 테이프
	case DefectType::NoneCoating:				return _T("None Coating");							// 미코팅
	case DefectType::TabShape:					return _T("Tab Shape");								// 탭 형상
	case DefectType::TabShape_A:				return _T("Tab Shape(Area)");						// 탭 형상(AREA)
	//case DefectType::Line:						return _T("Line");									// 라인
	//case DefectType::Scratch:					return _T("Scratch");								// 스크레치
	//case DefectType::Pinhole:					return _T("Pinhole");								// 핀홀
	//case DefectType::Dent:						return _T("Dent");									// 찍힘
	//case DefectType::Stain_Pinhole:					return _T("Stain_Pinhole");								// 분화구?
	//case DefectType::Crack:						return _T("Crack");									// 크랙
	case DefectType::HeatingWrinkle:			return _T("Heating Wrinkle");						// 열주름
	case DefectType::CoatingWrinkle:			return _T("Coating Wrinkle");						// 코팅주름
	//case DefectType::Delamination_Crack:				return _T("Delamination_Crack");							// 돌출
	case DefectType::InsulationIsland:			return _T("Insulation Island");						// 절연아일랜드
	case DefectType::TabWrinkle:				return _T("Tab Wrinkle");					    	// 탭 주름 ( zipper 2 )
	case DefectType::Wrinkle:					return _T("Wrinkle");								// 표면주름
	case DefectType::Dross:						return _T("Dross");									// 드로스  0910 표면검사 항목 추가
	case DefectType::Spatter:					return _T("Spatter");								// 스패터0910 표면검사 항목 추가
	case DefectType::TopScrab:					return _T("Top Scrab");								// 스크랩 0930 ESWA 표면검사 항목 추가
	case DefectType::BotScrab:					return _T("Bot Scrab");								// 스크랩 0930 ESWA 표면검사 항목 추가
	case DefectType::VHomePitch:				return _T("V Home Pitch");							// V Home Pitch 1008 ESWA 치수검사 항목 추가
	// (LongCell 추가항목)
	case DefectType::TopFrontCuttingHeight:		return _T("Top Front Cutting Height");				// 상부 앞면 커팅 높이
	case DefectType::TopBackCuttingHeight:		return _T("Top Back Cutting Height");				// 상부 뒷면 커팅 높이
	case DefectType::BottomFrontCuttingHeight:	return _T("Bottom Front Cutting Height");			// 하부 앞면 커팅 높이
	case DefectType::BottomBackCuttingHeight:	return _T("Bottom Back Cutting Height");			// 하부 뒷면 커팅 높이
	case DefectType::BottomFrontOverlayWidth:	return _T("Bottom Front Overlay Width");			// 하부 앞면 오버레이	
	case DefectType::BottomBackOverlayWidth:	return _T("Bottom Back Overlay Width");				// 하부 뒷면 오버레이	
	case DefectType::TopVHomeHeight:			return _T("Top V Home Height");						// 상부 V홈 높이
	case DefectType::BottomVHomeHeight:			return _T("Bottom V Home Height");					// 하부 V홈 높이
	// IBNI_Interface
	case DefectType::SurfaceDefect:				return _T("Surface Defect");						// 표면불량
	case DefectType::ElectrodeDefect:			return _T("Electrode Defect");
	case DefectType::NotchingDefect:			return _T("Notching Defect");
		// 메인에서 자동보정sw csv남기기 위해서 만든 define
	case DefectType::TopMismatch:				return _T("Top Mismatch");
	case DefectType::BottomMismatch:			return _T("Bottom Mismatch");
	case DefectType::RightTabSide:				return _T("Right Tab Side");						// 탭 사이드
	case DefectType::LeftTabSide:				return _T("Left Tab Side");							// 탭 사이드
	case DefectType::END:						return _T("NONE");
	case DefectType::NG_Tag:					return _T("NG Tag");
	//case DefectType::Drag:					    return _T("Drag");
	case DefectType::Burr:						return _T("Burr");									// 버 //20231014 [jjh]
	//case DefectType::Island:					return _T("Island");
	case DefectType::InsulationLine:			return _T("InsulationLine");
	case DefectType::InsulationGapLine:			return _T("InsulationGapLine");
	case DefectType::InsulationPinhole:			return _T("InsulationPinhole");
	case DefectType::InsulationGapPinhole:		return _T("InsulationGapPinhole");
	case DefectType::Spur:						return _T("Spur");
	case DefectType::Seam:						return _T("Seam");
	case DefectType::Scrab:						return _T("Scrab");

	case DefectType::SmallDefect_CraterLevel1:	return _T("Small Defect Crater Level1");
	case DefectType::SmallDefect_CraterLevel2:	return _T("Small Defect Crater Level2");
	case DefectType::SmallDefect_CraterLevel3:	return _T("Small Defect Crater Level3");
	case DefectType::SmallDefect_CraterLevel4:	return _T("Small Defect Crater Level4");
	case DefectType::SmallDefect_CraterLevel5:	return _T("Small Defect Crater Level5");
	case DefectType::SmallDefect_PinholeLevel1:	return _T("Small Defect Pinhole Level1");
	case DefectType::SmallDefect_PinholeLevel2:	return _T("Small Defect Pinhole Level2");
	case DefectType::SmallDefect_PinholeLevel3:	return _T("Small Defect Pinhole Level3");
	case DefectType::SmallDefect_PinholeLevel4:	return _T("Small Defect Pinhole Level4");
	case DefectType::SmallDefect_PinholeLevel5:	return _T("Small Defect Pinhole Level5");
	case DefectType::SmallDefect_DentLevel1:	return _T("Small Defect Dent Level1");
	case DefectType::SmallDefect_DentLevel2:	return _T("Small Defect Dent Level2");
	case DefectType::SmallDefect_DentLevel3:	return _T("Small Defect Dent Level3");
	case DefectType::SmallDefect_DentLevel4:	return _T("Small Defect Dent Level4");
	case DefectType::SmallDefect_DentLevel5:	return _T("Small Defect Dent Level5");
	case DefectType::SmallDefect_ProtrutionLevel1:	return _T("Small Defect Protrution Level1");
	case DefectType::SmallDefect_ProtrutionLevel2:	return _T("Small Defect Protrution Level2");
	case DefectType::SmallDefect_ProtrutionLevel3:	return _T("Small Defect Protrution Level3");
	case DefectType::SmallDefect_ProtrutionLevel4:	return _T("Small Defect Protrution Level4");
	case DefectType::SmallDefect_ProtrutionLevel5:	return _T("Small Defect Protrution Level5");
	case DefectType::SmallDefect_IslandLevel1:		return _T("Small Defect Island Level1");
	case DefectType::SmallDefect_IslandLevel2:		return _T("Small Defect Island Level2");
	case DefectType::SmallDefect_IslandLevel3:		return _T("Small Defect Island Level3");
	case DefectType::SmallDefect_IslandLevel4:		return _T("Small Defect Island Level4");
	case DefectType::SmallDefect_IslandLevel5:		return _T("Small Defect Island Level5");
	case DefectType::SmallDefect_ScratchLevel1:		return _T("Small Defect Scratch Level1");
	case DefectType::SmallDefect_ScratchLevel2:		return _T("Small Defect Scratch Level2");
	case DefectType::SmallDefect_ScratchLevel3:		return _T("Small Defect Scratch Level3");
	case DefectType::SmallDefect_ScratchLevel4:		return _T("Small Defect Scratch Level4");
	case DefectType::SmallDefect_ScratchLevel5:		return _T("Small Defect Scratch Level5");
	}

	ASSERT(!_T("정의 되지 않은 Defect Type"));
	return _T("UNKNOWN");
}

LPCTSTR GetDefectName_Before(DefectType type)
{
    
#if INSPECTION_MACHINE_UC1
    return _T("");
    
#else
    switch (type)
    {
        case DefectType::Line:
            return _T("White_Line");
        case DefectType::Pinhole:
            return _T("White_Pinhole");
        case DefectType::Horizontal_Line:
            return _T("Dark_Line");
        case DefectType::Stain:
            return _T("Dark_Crater");
        case DefectType::Balck_Dot:
            return _T("Black_Dot");
        case DefectType::Island:
            return _T("Foil_Island");
        case DefectType::Chipping_Pinhole:
            return _T("Edge_ChippingPinhole");
        case DefectType::Drag:
            return _T("Edge_Drag");
        case DefectType::Edge_Line:
            return _T("Edge_Line");
        case DefectType::Banking:
            return _T("Banking");
        case DefectType::Delamination_Pinhole:
            return _T("Delami");
        case DefectType::Plain_Wrinkle:
            return _T("Foil_Defect");
        case DefectType::Edge_Defect:
            return _T("Edge_Defect");
        case DefectType::ETC_Defect:
            return _T("ETC_Defect");
        case DefectType::Stain_Pinhole:
            return _T("Crater");
        case DefectType::Delamination_Crack:
            return _T("Protrusion");
        case DefectType::Dent:
            return _T("Dent");
        case DefectType::Crack:
            return _T("Crack");
        case DefectType::Scratch:
            return _T("Scratch");
    }
#endif
        
    
}

//LPCTSTR GetDefectName_UC(DefectType type)
//{
//    switch (type)
//    {
//        case DefectType::Line:
//            return _T("White_Line");
//        case DefectType::Pinhole:
//            return _T("White_Pinhole");
//        case DefectType::Horizontal_Line:
//            return _T("Dark_Line");
//        case DefectType::Stain:
//            return _T("Dark_Crater");
//        case DefectType::Balck_Dot:
//            return _T("Black_Dot");
//        case DefectType::Island:
//            return _T("Foil_Island");
//        case DefectType::Chipping_Pinhole:
//            return _T("Edge_ChippingPinhole");
//        case DefectType::Drag:
//            return _T("Edge_Drag");
//        case DefectType::Edge_Line:
//            return _T("Edge_Line");
//        case DefectType::Banking:
//            return _T("Bangking");
//        case DefectType::Delamination_Pinhole:
//            return _T("Delami");
//        case DefectType::Plain_Wrinkle:
//            return _T("Foil_Defect");
//        case DefectType::Edge_Defect:
//            return _T("Edge_Defect");
//        case DefectType::ETC_Defect:
//            return _T("ETC_Defect");
//        case DefectType::Stain_Pinhole:
//            return _T("Crater");
//        case DefectType::Delamination_Crack:
//            return _T("Protrusion");
//        case DefectType::Dent:
//            return _T("Dent");
//        case DefectType::Crack:
//            return _T("Crack");
//        case DefectType::Scratch:
//            return _T("Scratch");
//    }
//}

LPCTSTR GetDefectCode(DefectType type) //221108
{
	switch (type)
	{
	case DefectType::TabWidth:					return _T("TW_T1U1");								// 탭 폭 (CIS비전에서 주는 결과)
	case DefectType::TabWidth_A:				return _T("TW_T1U1");								// 탭 폭 (치수비전에서 주는 결과)
	case DefectType::TabPitch:					return _T("W_T1U1");								// 탭 피치 (CIS비전에서 주는 결과)
	case DefectType::TabPitch_2:				return _T("W_T1U1");								// 탭 피치 (치수비전에서 주는 결과)
	case DefectType::TabHeight:					return _T("TL_T1U1");								// 탭 높이
	case DefectType::Top_TabHeight_2:			return _T("UCW_M1U1");								// 상부 탭 높이(ESWA 요청)
	case DefectType::Bottom_TabHeight_2:		return _T("UCW_M1L1");								// 하부 탭 높이(ESWA 요청)
	case DefectType::TabSide:					return _T("TS_T1U1");								// 탭 사이드
	case DefectType::TopCoating:				return _T("SH_T1U");								// 상부 어깨선
	case DefectType::BottomCoating:				return _T("SH_T1L");								// 하부 어깨선
	case DefectType::Mismatch:					return _T("MIMA_A1U1");								/*_T("MIMA_A1U")//20230607 [jjh]*/ // 어깨선 차이값
	case DefectType::TopTilt:					return _T("VT_T1U1");                               // V홈 단차 상부 
	case DefectType::BottomTilt:				return _T("VT_N1U1");                               // V홈 단차 하부
	case DefectType::Misalign:					return _T("NM_A1U1");								/*_T("NM_A1U")//20230526 [jjh]*/ // V홈 차이값
	case DefectType::LeftLength:				return _T("L_O1U1");								// 좌측 전장
	case DefectType::RightLength:				return _T("L_M1U1");								// 우측 전장
	case DefectType::Top_Length_2:				return _T("CW_M1U1");								// 좌측 전장(ESWA 요청)
	case DefectType::Bottom_Length_2:			return _T("CW_M1L1");								// 우측 전장(ESWA 요청)
	case DefectType::TopInsulationWidth:		return _T("IW_T1U1");								// 상부절연 폭
	case DefectType::BottomInsulationWidth:		return _T("IW_T1L1");								// 하부절연 폭
	case DefectType::TopFrontOverlayWidth:		return _T("OW_T1U1");								// 상부오버레이 폭 (변경)
	case DefectType::TopBackOverlayWidth:		return _T("OW_T1L1");								// 하부오버레이 폭 (변경)
	case DefectType::Burr:						return _T("TBBR");									// 버 //20231014 [jjh]
	case DefectType::TimeOut:					return _T("TBBR");									// 타임 아웃
	case DefectType::ConnectionTape:			return _T("CNTT");									// 연결 테이프
	case DefectType::DarkConnectionTape:		return _T("Barcode Marks");							// 다크 연결 테이프
	case DefectType::NoneCoating:				return _T("NOCT");									// 미코팅
	case DefectType::TabShape:					return _T("TBDT");									// 탭 형상
	case DefectType::TabShape_A:				return _T("TBDT");									// 탭 형상(AREA)
	case DefectType::InsulationLine:			return _T("INLN");								//20230918 [jjh]
	case DefectType::InsulationPinhole:			return _T("INPH");									//20230918 [jjh]
#if INSPECTION_MACHINE_UC1
    
#else
    case DefectType::Banking:
        return _T("BANK");
    case DefectType::Horizontal_Line:
        return _T("HOLN");
    case DefectType::Delamination_Pinhole:
        return _T("DLPH");
    case DefectType::Stain:
        return _T("STIN");
    case DefectType::Chipping_Pinhole:
        return _T("CHIP");
    case DefectType::Drag:
        return _T("DRAG");
    case DefectType::Plain_Wrinkle:
        return _T("PWRK");
    case DefectType::Island:
        return _T("ISLD");
	case DefectType::Line:						return _T("LINE");									// 라인
    case DefectType::Pinhole:			        return _T("PINH"); // 핀홀
    case DefectType::Stain_Pinhole:		        return _T("CRAT"); // 분화구?
    case DefectType::Delamination_Crack:	    return _T("PRTS"); // 돌출
#endif
	case DefectType::Scratch:					return _T("SCRT");									// 스크레치
	
	case DefectType::Dent:						return _T("DENT");									// 찍힘
	
	case DefectType::Crack:						return _T("CRCK");									// 크랙
	case DefectType::HeatingWrinkle:			return _T("HWRK");									// 열주름
	case DefectType::CoatingWrinkle:			return _T("CWRK");									// 코팅주름
	
	case DefectType::InsulationIsland:			return _T("INID");									// 절연아일랜드
	case DefectType::Wrinkle:					return _T("PWRK");									// 표면주름
	case DefectType::Dross:						return _T("Dross");									// 드로스  0910 표면검사 항목 추가 //디펙코드 사용하지 않는 항목
	case DefectType::Spatter:					return _T("SPTT");									// 스패터0910 표면검사 항목 추가
	case DefectType::TopScrab:					return _T("SCRP");									// 스크랩 0930 ESWA 표면검사 항목 추가
	case DefectType::BotScrab:					return _T("SCRP");									// 스크랩 0930 ESWA 표면검사 항목 추가
	case DefectType::VHomePitch:				return _T("VHomePitch");							// V Home Pitch 1008 ESWA 치수검사 항목 추가
	// (LongCell 추가항목)
	case DefectType::TopFrontCuttingHeight:		return _T("CT_T1U1");								// 상부 앞면 커팅 높이
	case DefectType::TopBackCuttingHeight:		return _T("CT_T1L1");								/*_T("CT_N1U1")//20231102 [jjh]*/// 상부 뒷면 커팅 높이
	case DefectType::BottomFrontCuttingHeight:	return _T("CT_N1U1");								/*_T("CT_T1L1")//20231102 [jjh]*/// 하부 앞면 커팅 높이
	case DefectType::BottomBackCuttingHeight:	return _T("CT_N1L1");								// 하부 뒷면 커팅 높이
	case DefectType::BottomFrontOverlayWidth:	return _T("OW_N1U1");								// 하부 앞면 오버레이	
	case DefectType::BottomBackOverlayWidth:	return _T("OW_N1L1");								// 하부 뒷면 오버레이	
	case DefectType::TopVHomeHeight:			return _T("CFH_T1U1");								/*_T("CFH_T1U")//20230526 [jjh]*/	// 상부 V홈 높이
	case DefectType::BottomVHomeHeight:			return _T("CFH_N1U1");								/*_T("CFH_N1U");//20230526 [jjh]*/ // 하부 V홈 높이
	// IBNI_Interface
	case DefectType::SurfaceDefect:				return _T("Surface Defect");						// 표면불량 //디펙코드 사용하지 않는 항목
	case DefectType::ElectrodeDefect:			return _T("Electrode Defect");                      //디펙코드 사용하지 않는 항목
	case DefectType::NotchingDefect:			return _T("Notching Defect");                       //디펙코드 사용하지 않는 항목
	// 메인에서 자동보정sw csv남기기 위해서 만든 define
	case DefectType::TopMismatch:				return _T("Top Mismatch");							//디펙코드 사용하지 않는 항목
	case DefectType::BottomMismatch:			return _T("Bottom Mismatch");						//디펙코드 사용하지 않는 항목
	case DefectType::RightTabSide:				return _T("TS_T1U1");								// 탭 사이드
	case DefectType::LeftTabSide:				return _T("TS_T1U1");								// 탭 사이드
	case DefectType::END:						return _T("NONE");

	case DefectType::SmallDefect_CraterLevel1:		  return _T("SmallDefect_CraterLevel1");
	case DefectType::SmallDefect_CraterLevel2:		  return _T("SmallDefect_CraterLevel2");
	case DefectType::SmallDefect_CraterLevel3:		  return _T("SmallDefect_CraterLevel3");
	case DefectType::SmallDefect_CraterLevel4:		  return _T("SmallDefect_CraterLevel4");
	case DefectType::SmallDefect_CraterLevel5:		  return _T("SmallDefect_CraterLevel5");
	case DefectType::SmallDefect_PinholeLevel1:			  return _T("SmallDefect_PinholeLevel1");
	case DefectType::SmallDefect_PinholeLevel2:			  return _T("SmallDefect_PinholeLevel2");
	case DefectType::SmallDefect_PinholeLevel3:			  return _T("SmallDefect_PinholeLevel3");
	case DefectType::SmallDefect_PinholeLevel4:			  return _T("SmallDefect_PinholeLevel4");
	case DefectType::SmallDefect_PinholeLevel5:			  return _T("SmallDefect_PinholeLevel5");
	case DefectType::SmallDefect_DentLevel1:				return _T("Small Defect Dent Level1");
	case DefectType::SmallDefect_DentLevel2:				return _T("Small Defect Dent Level2");
	case DefectType::SmallDefect_DentLevel3:				return _T("Small Defect Dent Level3");
	case DefectType::SmallDefect_DentLevel4:				return _T("Small Defect Dent Level4");
	case DefectType::SmallDefect_DentLevel5:				return _T("Small Defect Dent Level5");
	case DefectType::SmallDefect_ProtrutionLevel1:		  return _T("SmallDefect_ProtrutionLevel1");
	case DefectType::SmallDefect_ProtrutionLevel2:		  return _T("SmallDefect_ProtrutionLevel2");
	case DefectType::SmallDefect_ProtrutionLevel3:		  return _T("SmallDefect_ProtrutionLevel3");
	case DefectType::SmallDefect_ProtrutionLevel4:		  return _T("SmallDefect_ProtrutionLevel4");
	case DefectType::SmallDefect_ProtrutionLevel5:		  return _T("SmallDefect_ProtrutionLevel5");
	case DefectType::SmallDefect_IslandLevel1:			  return _T("SmallDefect_IslandLevel1");
	case DefectType::SmallDefect_IslandLevel2:			  return _T("SmallDefect_IslandLevel2");
	case DefectType::SmallDefect_IslandLevel3:			  return _T("SmallDefect_IslandLevel3");
	case DefectType::SmallDefect_IslandLevel4:			  return _T("SmallDefect_IslandLevel4");
	case DefectType::SmallDefect_IslandLevel5:			  return _T("SmallDefect_IslandLevel5");
	case DefectType::SmallDefect_ScratchLevel1:			  return _T("SmallDefect_ScratchLevel1");
	case DefectType::SmallDefect_ScratchLevel2:			  return _T("SmallDefect_ScratchLevel2");
	case DefectType::SmallDefect_ScratchLevel3:			  return _T("SmallDefect_ScratchLevel3");
	case DefectType::SmallDefect_ScratchLevel4:			  return _T("SmallDefect_ScratchLevel4");
	case DefectType::SmallDefect_ScratchLevel5:			  return _T("SmallDefect_ScratchLevel5");
	}

	ASSERT(!_T("정의 되지 않은 Defect Type"));
	return _T("UNKNOWN");
}


CString	GetDefectType2String(ensection section)
{
	switch (section)
	{
	case NONE:					return _T("NONE");
	case NOTCHING:				return _T("NOTCHING");
	case ELECTRODE:				return _T("ELECTRODE");
	default:
		AfxMessageBox(_T("There is a problem with the code. 56756756"));
		ASSERT(1);
		break;
	}

	return _T("");
}


ensection GetDefectType2enum(CString strType)
{
	if (_T("NONE") == strType)
		return NONE;

	if (_T("NOTCHING") == strType)
		return NOTCHING;

	if (_T("ELECTRODE") == strType)
		return ELECTRODE;

	AfxMessageBox(_T("There is a problem with the code. 678678678"));
	ASSERT(1);
}




DefectType GetDefectType(LPCTSTR defectName)
{
	//if (_T("") == defectName)
	//{
	//	int i = 0;
	//}

	CString name = defectName;
#ifdef _ISI_
	for (long typeIndex = 0; typeIndex < (long)DefectType::ISI_END; typeIndex++)
#else
	for (long typeIndex = 0; typeIndex < (long)DefectType::END; typeIndex++)
#endif
	{
		if (name == GetDefectName((DefectType)typeIndex))
		{
			return (DefectType)typeIndex;
		}
	}

	return DefectType::END;
}
