#pragma once
#include "../SharedModule/VisionHostShare/basedef.h"
#include "../VisionModule/Base/ParaEdgeDetection.h"

#define THREAD_COUNT 5
#define BLOB_COUNT 24
#define IMAGE_SPLIT_COUNT 32
#define MAX_DEFECT_COUNT 1000

#define EDGE_DETECTS_COUNT 8	//절연, 오버레이 폭 엣지 검출 개수


enum II_CORNER_VALUE : long
{
	II_LEFT_TOP = 0,
	II_LEFT_BOTTOM,
	II_RIGHT_TOP,
	II_RIGHT_BOTTOM,
};

enum II_RESULT_VALUE : long
{
	PASS,
	REWORK,
	REJECT,
	INVALID,
	NOT_MEASURED,
	NOT_CANDIDATE,
};

enum II_LOGGER_VALUE : int
{
	SEQUENCE,
	RECIPE,
	INSPECTION,
	SIMULATION,
};

enum EdgeSearchDirection
{
	InnerToOuter = 0,
	OuterToInner
};

enum VhomeThresholdImageOption
{
	None,
	BrightBright,
	BrightDark,
	DarkBright,
	DarkDark
};
enum ImageChannel
{
	Bright = 0,
	Dark
};
struct CELL_DESIGN
{
	double fCellHeight;
	double fCellHeight2;
	double fCellWidth;
	double fTabHeight;
	double fTabHeight2;
	double fTabWidth;
	double fTabOffset;
	double fVHomeSize;
	double fTabSensorDelayOffset;
	double fMoldConstant;
	double fVhomeHeight;
	int nVhomeOffsetUP;
	int nVhomeOffsetDOWN;
};

namespace Inspection
{
	enum class MeasureOffsetType
	{
		CellLeftLength,
		CellRightLength,//20220625 [jjh]
		CellLength2,
		CuttingHeightTop,
		CuttingHeightBot,
		Misalign,
		Mismatch,
		ShoulderLine,
		TabHeight,
		TabHeight2,
		TabPitch,
		TabSide,
		TabWidth,
		VHomeTiltTop,
		VHomeTiltBot,
		Overlay,
		Insulation,
		BotOverlay,
	};

	class Accessor;
	class DebugItemInfo;
	class DebugInfo;

	class ImageViewEx;
	class InspectionCriteria;
	class InspectionCriteriaList;
	class InspMergeOptions;
	class MeasureOffsets;
	class Root;
	class SetupImage;
	class UrgentVariable;

	class MeasureValue;
}

// 현식 여기에 use 추가? 아마도
//승만
namespace classification
{

	enum enColTitle
	{
		ITEM = 0,
		USE,
		ENABLE,
		PRIORITY,
		SIZE_X,
		SIZE_X_CO,
		SIZE_Y,
		SIZE_Y_CO,
		ORAND,
		AREA,
		AREA_CO,
        COUNT,
        PARENT,
        AREA_Ratio,
        AREA_Ratio_CO,
		SMALL_INSP,
		SMALL_INSP_LEVEL,
		INSPECTION_AREA,
		INSPECTION_IMAGE_TYPE,
		RATIO_XY,
		RATIO_XY_CO,
		RATIO_YX,
		RATIO_YX_CO,
		WIDTH,
		WIDTH_CO,
		LENGTH,
		LENGTH_CO,
		RATIO_WL,
		RATIO_WL_CO,
		RATIO_LW,
		RATIO_LW_CO,
		MIN,
		MIN_CO,
		AVG,
		AVG_CO,
		MAX,
		MAX_CO,
        BRIGHT_AVG,
        BRIGHT_AVG_CO,
		DARK_AVG,
        DARK_AVG_CO,
		MASS_CENTERGV,
		MASS_CENTERGV_CO,
		EDGE_ENERGY,
		EDGE_ENERGY_CO,
        BLACK_EDGE_ENERGY,
        BLACK_EDGE_ENERGY_CO,
        WHITE_EDGE_ENERGY,
        WHITE_EDGE_ENERGY_CO,
        SECTION_THRESHOLD,
        SECTION_THRESHOLD_CO,
		HOR,
		HOR_CO,
		VER,
		VER_CO,
		RATIO_HV,
		RATIO_HV_CO,
		RATIO_VH,
		RATIO_VH_CO,
		RATIO_WHITE,
		RATIO_WHITE_CO,
		RATIO_DARK,
		RATIO_DARK_CO,
		DEFECT_BRIGHTNESS,
		BLOB_THRESHOLD,
		BLOB_THRESHOLD_CO,
		CATEGORY,
		COMPACT,
		COMPACT_CO,
		ANGLE,
		ANGLE_CO,
		STD,
		STD_CO,
		MINIMUM_SIZE_X,
		MINIMUM_SIZE_Y,
		/*COUNT,
        PARENT,*/
		OVER_GV_PX_COUNT,
        OVER_GV_PX_COUNT_CO,
		DIS_TO_EDGE_mm,
        DIS_TO_EDGE_mm_CO,
        DIS_TO_EDGE_2_mm,
        DIS_TO_EDGE_2_mm_CO,
        IS_NEAR_EDGE,
        IS_NEAR_EDGE_CO,
		CT_END,
		//CT_END,

	};

	enum enInspectionImageType : long
	{
		Bright = 0,
		Dark = 1,
		IIT_None = 2,
		INS_END,
	};


	enum enLogicResult : long
	{
		NO = 0,
		YES = 1,
		NONE = 2,
	};

	enum enJudge : long
	{
		JD_REJECT = 0,
		JD_GOOD = 1,
	};

	enum enCode : long
	{
		CD_OR = 0,
		CD_AND,
	};

	enum enCompareOperator : long
	{
		NOT_USE = 0,
		BIGGER_THAN_SPEC = 1,
		SMALLER_THAN_SPEC = 2,
		CO_END = 3,
		SAME,
	};

	enum enLogicKey : long		//순서 아님
	{
		InspImageBrightness = 0,
		DefectBrightness,
		ElectrodeEdge,
		Category,
		Size,
		SizeX,
		SizeY,
		AVG_GV,
		MIN_GV,
		MAX_GV,
		Length,
		Width,
		Area,
		Area_Ratio,
		Std,
		RatioLW,
		RatioWL,
		RatioYX,
		RatioXY,
		Angle,
		Compact,
		Bright_avg_gv,
		Dark_avg_gv,
		Section_Threshold,
		RatioWhite, // [sm]
		RatioDark,
		EdgeEnergy,
		EdgeEnergy_Black,
		EdgeEnergy_White,
		RatioArea,
		MASS_CENTER_GV_gv,
		Hor,
		Ver,
		RatioHV,
		RatioVH,
		OverPxCnt,
		Dis2Edge,
        Dis2Edge_2,
		IsNearEdge,
        Sharpness_EdgeEnergy,
        Sharpness_EdgeEnergy_Black,
        Sharpness_EdgeEnergy_White,
	};

	struct LogicKey2String
	{
		CString GetString(enLogicKey key)
		{
			switch (key)
			{
            case classification::SizeX:
                return _T("Size X");
                break;
            case classification::SizeY:
                return _T("Size Y");
                break;
            case classification::Area:
                return _T("AREA");
                break;
            case classification::Area_Ratio:
                return _T("AREA_Ratio");
                break;
			case classification::InspImageBrightness:
				return _T("INSP IMAGE BRIGHTNESS");
				break;
			case classification::DefectBrightness:
				return _T("DEFECT BRGIHTNESS");
				break;
            case classification::RatioXY:
                return _T("RATIO XY");
                break;
            case classification::RatioYX:
                return _T("RATIO YX");
                break;
            case classification::Width:
                return _T("WIDTH");
                break;
            case classification::Length:
                return _T("LENGTH");
                break;
            case classification::RatioLW:
                return _T("RATIO LW");
                break;
            case classification::RatioWL:
                return _T("RATIO WL");
                break;

			case classification::Size:
				return _T("SIZE");
				break;
            case classification::Dark_avg_gv:
                return _T("DARK_AVG");
                break;
            case classification::Bright_avg_gv:
                return _T("BRIGHT_AVG");
                break;
			case classification::AVG_GV:
				return _T("AVG_GV");
				break;
            case classification::MIN_GV:
                return _T("MIN_GV");
                break;
            case classification::MAX_GV:
                return _T("MAX_GV");
                break;
			case classification::ElectrodeEdge:
				return _T("ELECTRODE EDGE");
				break;
			case classification::Category:
				return _T("CATEOGRY");
				break;
            case classification::MASS_CENTER_GV_gv:
                return _T("Mass Center GV");
                break;
			case classification::Std:
				return _T("STD");
				break;
            case classification::EdgeEnergy:
                return _T("Edge Energy");
                break;
			case classification::Compact:
				return _T("COMPACT");
				break;
			case classification::Section_Threshold:
				return _T("Section Th");
				break;
            case classification::Hor:
                return _T("Hor");
                break;
            case classification::Ver:
                return _T("Ver");
                break;
            case classification::RatioHV:
                return _T("Ratio HV");
                break;
            case classification::RatioVH:
                return _T("Ratio VH");
                break;
			case classification::Angle:
				return _T("Angle");
				break;
			case classification::RatioWhite:
				return _T("Ratio White");
				break;
			case classification::RatioDark:
				return _T("Ratio Black");
				break;
			case classification::RatioArea:
				return _T("Ratio Area");
				break;
            case classification::OverPxCnt:
                return _T("250 > CNT");
                break;
            case classification::Dis2Edge:
                return _T("Dis 2 Edge(mm)");
                break;
            case classification::Dis2Edge_2:
                return _T("Dis 2 Edge_2(mm)");
                break;
            case classification::EdgeEnergy_Black:
                return _T("EdgeEnergy Black");
                break;
            case classification::EdgeEnergy_White:
                return _T("EdgeEnergy White");
            case classification::IsNearEdge:
                return _T("Is Near Edge");
                break;
            case classification::Sharpness_EdgeEnergy:
                return _T("Sharpness_EdgeEnergy");
                break;
            case classification::Sharpness_EdgeEnergy_White:
                return _T("White_Sharpness_EdgeEnergy");
                break;
            case classification::Sharpness_EdgeEnergy_Black:
                return _T("Black_Sharpness_EdgeEnergy");
                break;
			default:
				return _T("Key error");
				break;
			}
		}
	};

	enum enDefectBrightness : long
	{
		DFB_WHITE = 0,
		DFB_BLACK = 1,
		DFB_MIX = 2,
		DFB_NONE = 3,
		DFB_NO_USE,
	};

	enum enCategory : long
	{
		CTG_NONE = 0,
		CTG_CONNECTION_TAPE,
		CTG_WINKLE_ZIPPER2,
		CTG_TAB_FOLDING,
		CTG_NG_TAG,
		CTG_BURR,
		CTG_SPUR,
	};

	enum enSmallInsplevel : long
    {
		level1 = 0,
		level2,
		level3,
		level4,
		end,
		
    };

	enum enParent : long
    {
        None = 0,
        Stain,
        Stain_Pinhole,
        Pinhole,
        Dent,
        Island,
        End,
		Line,
		
		Horizontal_Line,
		
		Edge_ChippinPinhole,
		Drag,
		Edge_Line,
		
		
		Black_Dot,
        Scratch,
        Crack,
        Delamination_Crack,
        Plain_Wrinkle,
        Edge_Defect,
        ETC_Defect,

		White_Line,
        White_Pinhole,
        Dark_Line,
        Dark_Crater,
        Crater,
        Protrusion,
        Delami,
        Edge_Drag,
        Foil_Island,
        Foil_Defect,
		
	};

 
	struct enumtoString
	{
        CString Parent2String(enParent type)
        {
            switch (type)
            {
                case enParent::Line:
                    return _T("Line");
                    break;
                case enParent::Pinhole:
                    return _T("Pinhole");
                    break;
                case enParent::Horizontal_Line:
                    return _T("Horizontal_Line");
                    break;
                case enParent::Stain:
                    return _T("Stain");
                    break;
                case enParent::Island:
                    return _T("Island");
                    break;
                case enParent::Edge_ChippinPinhole:
                    return _T("Edge_ChippinPinhole");
                    break;
                case enParent::Drag:
                    return _T("Drag");
                    break;
                case enParent::Edge_Line:
                    return _T("Edge_Line");
                    break;
                case enParent::Dent:
                    return _T("Dent");
                    break;
                case enParent::Stain_Pinhole:
                    return _T("Stain_Pinhole");
                    break;
                case enParent::Black_Dot:
                    return _T("Black_Dot");
                    break;
                case enParent::Scratch:
                    return _T("Scratch");
                    break;
                case enParent::Crack:
                    return _T("Crack");
                    break;
                case enParent::Delamination_Crack:
                    return _T("Delamination_Crack");
                    break;
                case enParent::Plain_Wrinkle:
                    return _T("Plain_Wrinkle");
                    break;
                case enParent::Edge_Defect:
                    return _T("Edge_Defect");
                    break;
                case enParent::ETC_Defect:
                    return _T("ETC_Defect");
                    break;
                case enParent::End:
                    return _T(" ");
                    break;
                default:
                    return _T(" ");
                    break;
            }
		}

		CString Category(enCategory type)
		{
			switch (type)
			{
			case enCategory::CTG_NONE:
				return _T("");
				break;
			case enCategory::CTG_CONNECTION_TAPE:
				return _T("connection tape");
				break;
			case enCategory::CTG_NG_TAG:
				return _T("ng tag");
				break;
			case enCategory::CTG_TAB_FOLDING:
				return _T("Tab folding");
				break;
			case enCategory::CTG_WINKLE_ZIPPER2:
				return _T("zipper(2)");
				break;
			default:
				return _T("Error");
				break;
			}
		}

		CString SmallInsp2String(enSmallInsplevel type)
        {
            switch (type)
            {                
                case enSmallInsplevel::level1:
                    return _T("1");
                    break;
                case enSmallInsplevel::level2:
                    return _T("2");
                    break;
                case enSmallInsplevel::level3:
                    return _T("3");
                    break;
                case enSmallInsplevel::level4:
                    return _T("4");
                    break;
                case enSmallInsplevel::end:
                    return _T(" ");
                    break;
            }
		}

		CString CompareOperator2String(enCompareOperator type)
		{
			switch (type)
			{
				/*case classification::SAME:
					return _T("=");
					break;*/
			
				
			case classification::BIGGER_THAN_SPEC:
				return _T("(크)<");
				break;
			case classification::SMALLER_THAN_SPEC:
				return _T("(작)>");
				break;
			case classification::NOT_USE:
				//return _T("NotUse");
				return _T(" ");
				break;
				
			default:
				return _T("Error");
				break;
			}
		}

		CString OrAnd2String(bool bOrAnd)
		{
			switch (bOrAnd)
			{
			case 0:
				return _T("OR");
				break;
			case 1:
				return _T("AND");
				break;
            default:
                return _T("Error");
                break;
			}

		}

		enCompareOperator String2CompareOperator(CString type)
		{
			if (_T("(크)<") == type)
			{
				return classification::BIGGER_THAN_SPEC;
			}
			else if (_T("(작)>") == type)
			{
				return classification::SMALLER_THAN_SPEC;
			}
			/*else if (_T("=") == type)
			{
				return classification::SAME;
			}*/
		}

		CString InspectionImageType2String(enInspectionImageType type)
		{
			switch (type)
			{
			case enInspectionImageType::IIT_None:
				return _T("");
				break;
			case enInspectionImageType::Bright:
				return _T("Bright");
				break;
			case enInspectionImageType::Dark:
				return _T("Dark");
				break;
			case enInspectionImageType::INS_END:
				return _T("");
				break;
			default:
				return _T("error");
				break;
			}
		}

		CString DefectBrightness2String(enDefectBrightness type)
		{
			switch (type)
			{
			case enDefectBrightness::DFB_NONE:
				return _T("");
				break;
			case enDefectBrightness::DFB_BLACK:
				return _T("BLACK");
				break;
			case enDefectBrightness::DFB_WHITE:
				return _T("WHITE");
				break;
			case enDefectBrightness::DFB_MIX:
				return _T("MIX");
				break;
			default:
				return _T("error");
				break;
			}
		}

	};
	//현식

	enum enInspectionArea : long
	{
		IA_NONE = 0,
		IA_BACKGROUND_BURR,
		IA_SCRAB,
		IA_TAB,
		IA_TAB_INSULATION,
		IA_ELECTRODE_EDGE,
		IA_ELECTRODE,
		IA_TAB_END,

		IA_ELECTRODE_EDGE_LEFT,
		IA_ELECTRODE_EDGE_RIGHT,
		IA_TAB_INSULATION_GAP,
		IA_CUTTINGHEIGHT_TOP,
		IA_CUTTINGHEIGHT_BOT,
		IA_END,

	};

	struct ConvertInspectionArea
	{
        /*DefectType Parent2DefectType(long type)
        {
            switch ((enParent)type)
            {
                case enParent::None:
                    return DefectType::START_Coater_Surface;
                case enParent::Line:
                    return DefectType::Line;
                case enParent::Pinhole:
                    return DefectType::Pinhole;
                case enParent::Horizontal_Line:
                    return DefectType::Horizontal_Line;
                case enParent::Stain:
                    return DefectType::Stain;
                case enParent::Island:
                    return DefectType::Island;
                case enParent::Edge_ChippinPinhole:
                    return DefectType::Chipping_Pinhole;
                case enParent::Drag:
                    return DefectType::Drag;
                case enParent::Edge_Line:
                    return DefectType::Edge_Line;
                case enParent::Dent:
                    return DefectType::Dent;
                case enParent::Crater:
                    return DefectType::Crater;
                case enParent::Black_Dot:
                    return DefectType::Balck_Dot;
                case enParent::End:
                    return DefectType::END_Coater_Surface;
                
			}
        }*/
        /*None = 0, Line, Pinhole, Horizontal_Line, Stain, Island, Edge_ChippinPinhole, Drag, Edge_Line,
    Dent, Crater, Black_Dot, End,*/

        /*START_Coater_Surface, Line, Pinhole, Horizontal_Line, Stain, Island, Chipping_Pinhole,
        Drag, Edge_Line, Dent, Crater, Balck_Dot, END_Coater_Surface,*/

		

		long InspArea2Tabindex(long ia)
		{
			switch ((enInspectionArea)ia)
			{
			case classification::IA_NONE:
				break;
			case classification::IA_BACKGROUND_BURR:
				break;
			case classification::IA_SCRAB:
				break;
			case classification::IA_TAB:
				break;
			case classification::IA_TAB_INSULATION:
				break;
			case classification::IA_ELECTRODE_EDGE:
				break;
			case classification::IA_ELECTRODE:
				break;
			case classification::IA_ELECTRODE_EDGE_LEFT:
				ia = classification::IA_ELECTRODE_EDGE;
				break;
			case classification::IA_ELECTRODE_EDGE_RIGHT:
				ia = classification::IA_ELECTRODE_EDGE;
				break;
			case classification::IA_CUTTINGHEIGHT_TOP:
				ia = classification::IA_TAB_INSULATION;
				break;
			case classification::IA_CUTTINGHEIGHT_BOT:
				ia = classification::IA_TAB_INSULATION;
				break;
			case classification::IA_END:
				ia = classification::IA_NONE;
				break;
			default:
				ia = classification::IA_NONE;
				break;
			}

			return ia;
		}

		LPCSTR GetString(enInspectionArea ia)
		{

			switch (ia)
			{
			case classification::IA_NONE:
				return "NONE";
				break;
			case classification::IA_BACKGROUND_BURR:
				return "BACKGROUND_BURR";
				break;
			case classification::IA_SCRAB:
				return "SCRAB";
				break;
			case classification::IA_TAB:
				return "TAB";
				break;
			case classification::IA_TAB_INSULATION:
				return "TAB_INSULATION";
				break;
			case classification::IA_ELECTRODE_EDGE:
				return "ELECTRODE_EDGE";
				break;
			case classification::IA_ELECTRODE:
				return "ELECTRODE";
				break;
			case classification::IA_TAB_END:
				return "TAB_END";
				break;
			case classification::IA_ELECTRODE_EDGE_LEFT:
				return "ELECTRODE_EDGE_LEFT";
				break;
			case classification::IA_ELECTRODE_EDGE_RIGHT:
				return "ELECTRODE_EDGE_RIGHT";
				break;
			case classification::IA_CUTTINGHEIGHT_TOP:
				return "CUTTINGHEIGHT_TOP";
				break;
			case classification::IA_CUTTINGHEIGHT_BOT:
				return "CUTTINGHEIGHT_BOT";
				break;
			case classification::IA_END:
				return "END";
				break;
            case classification::IA_TAB_INSULATION_GAP:
                return "IA_TAB_INSULATION_GAP";
                break;
			default:
				return "NA";
				break;
			}

		}

		COLORREF GetUIColor(enInspectionArea ia)
		{
			COLORREF color = RGB(0, 0, 0);
			switch (ia)
			{
			case classification::IA_NONE:
				color = RGB(0, 100, 200);
				break;
			case classification::IA_BACKGROUND_BURR:
				color = RGB(200, 0, 0);
				break;
			case classification::IA_SCRAB:
				color = RGB(0, 200, 0);
				break;
			case classification::IA_TAB:
				color = RGB(0, 0, 200);
				break;
			case classification::IA_TAB_INSULATION:
				color = RGB(200, 200, 0);
				break;
			case classification::IA_ELECTRODE_EDGE:
				color = RGB(200, 0, 200);
				break;
			case classification::IA_ELECTRODE:
				color = RGB(0, 200, 200);
				break;
			case classification::IA_TAB_END:
				color = RGB(100, 0, 0);
				break;
			case classification::IA_ELECTRODE_EDGE_LEFT:
				color = RGB(0, 100, 0);
				break;
			case classification::IA_ELECTRODE_EDGE_RIGHT:
				color = RGB(0, 0, 100);
				break;
			case classification::IA_CUTTINGHEIGHT_TOP:
				color = RGB(100, 0, 100);
				break;
			case classification::IA_CUTTINGHEIGHT_BOT:
				color = RGB(0, 100, 100);
				break;
			case classification::IA_END:
				color = RGB(0, 200, 100);
				break;
			default:
				color = RGB(200, 0, 0);
				break;
			}
			return color;
		}
	};

	enum enSmallDefectCode : long
	{
		SMD_Heated_Line = 1101,
		SMD_Scratch = 1301,
		SMD_Pinhole = 2101,
		SMD_Protrution = 2201,
		SMD_Crater_Level1 = 3101,
		SMD_Crater_Level2 = 3102,
		SMD_Crater_Level3 = 3103,
		SMD_Crater_Level4 = 3104,
		SMD_Island = 5101,
		SMD_END = 6000,
	};


	struct stDefectResult
	{
		int nCandidateDefectCode;
		int nJudge;
		int nSmallDefectCode;
	};

}
namespace ISICF = classification;

#ifdef __INSPECTION_CLASS_EXPORT__
#define __INSPECTOIN_CLASS__	__declspec(dllexport)
#else
#define __INSPECTOIN_CLASS__	__declspec(dllimport)
#endif
