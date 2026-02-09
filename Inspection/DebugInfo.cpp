#include "stdafx.h"
#include "DebugInfo.h"
#include "ImageViewEx.h"
#include "Widget/ProfileView.h"

#include "../VisionModule/Language/LanguageText.h"

#include "Algorithm/ImageProcessing.h"
#include "Base/basedef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Inspection;

//============================================================================================================
// Debug Item Info는 외부 노출 모든 함수들은 this가 nullptr 인 경우를 상정하고 만들자
// Inline중에는 nullptr로 들어오게 하여 Debug를 위한 추가 연산은 하지 않으려는 의도로 이렇게 만들었다
//============================================================================================================

DebugItemInfo::DebugItemInfo()
	: m_overlay_displayNo(false)
{
}

DebugItemInfo::~DebugItemInfo()
{
}

bool DebugItemInfo::IsEmpty() const
{
	if (!IsOverlayEmpty()) return false;
	if (m_image.GetSizeX() > 0) return false;

	return m_resultText.IsEmpty();
}

bool DebugItemInfo::IsOverlayEmpty() const
{
	if (m_overlays_rect.size() != 0) return false;
	if (m_overlays_rect_32f.size() != 0) return false;
	if (m_overlays_point32fC2.size() != 0) return false;
	if (m_overlays_point32sC2.size() != 0) return false;
	if (m_overlays_line.size() != 0) return false;
	if (m_overlays_quadrangle32f.size() != 0) return false;
	if (m_overlays_text.size() != 0) return false;

	return true;
}
void DebugItemInfo::UpdateProfileView(IPVM::ProfileView* profileview ) const
{
	if (0 >= (long)m_profile.size()) return;

	profileview->Clear();
	for (long index = 0; index < (long)m_profile.size(); index++)
	{
		auto& profile = m_profile[index];
		std::vector<IPVM::Point_32f_C2> profileDatas;
		for (long point = 0; point < (long)profile.size(); point++)
		{
			auto& value = profile[point];
			profileDatas.emplace_back((float)point, (float)value);
		}
		int nPosition = _wtoi(m_resultText);
		if (profileDatas.size() > 0)
		{
			profileview->SetData(index, &profileDatas[0], (long)profileDatas.size(), NOISE_VALUE_32F, RGB(0, 0, 0), RGB(0,0, 0));
			profileview->SetCursorPosition(nPosition);
			profileview->SetColor_CursorVerticalLine(RGB(255, 0, 0));
		}
	}
}
void DebugItemInfo::UpdateImageView(ImageViewEx* imageView) const
{
	if (m_image.GetSizeX() > 0)
	{
		imageView->SetImage(m_image);
	}

	if (IsOverlayEmpty()) return;

	imageView->ImageOverlayClear();

	for (long index = 0; index < (long)m_overlays_rect.size(); index++)
	{
		auto& overlay = m_overlays_rect[index];
		imageView->ImageOverlayAdd(overlay.m_obj, overlay.m_color, 2);

		if (m_overlay_displayNo)
		{
			IPVM::Point_32s_C2 pt(overlay.m_obj.CenterPoint());
			CString str;
			str.Format(_T("%d"), index + 1);
			imageView->ImageOverlayAdd(pt, str, overlay.m_color, 50);
		}
	}

	for (long index = 0; index < (long)m_overlays_rect_32f.size(); index++)
	{
		auto& overlay = m_overlays_rect_32f[index];
		imageView->ImageOverlayAdd(overlay.m_obj, overlay.m_color, 2);

		if (m_overlay_displayNo)
		{
			IPVM::Point_32s_C2 pt(overlay.m_obj.CenterPoint());
			CString str;
			str.Format(_T("%d"), index + 1);
			imageView->ImageOverlayAdd(pt, str, RGB(255, 0, 0), 50);
		}
	}

	for (auto& line : m_overlays_line)
	{
		imageView->ImageOverlayAdd(line.m_obj, line.m_color, 2);
	}

	for (auto& point : m_overlays_point32fC2)
	{
		imageView->ImageOverlayAdd(point.m_obj, point.m_color, 3);
	}

	for (auto& point : m_overlays_point32sC2)
	{
		imageView->ImageOverlayAdd(point.m_obj, point.m_color, 3);
	}

	for (auto& quadrangle : m_overlays_quadrangle32f)
	{
		imageView->ImageOverlayAdd(quadrangle.m_obj, quadrangle.m_color);
	}

	for (auto& text : m_overlays_text)
	{
		imageView->ImageOverlayAdd(text.m_obj.m_pos, text.m_obj.m_text, text.m_color, text.m_obj.m_fontSize);
	}

	imageView->ImageOverlayShow();
}

DebugInfo::DebugInfo()
	: m_moduleCategory(ProcItemCategory::END)
{
}

DebugInfo::~DebugInfo()
{
}

const DebugItemInfo& DebugInfo::GetAt(long index) const
{
	static DebugItemInfo empty;
	if (this == nullptr) return empty;

	return m_items[index];
}

long DebugInfo::GetItemCount() const
{
	if (this == nullptr) return 0;

	return (long)m_items.size();
}

ProcItemCategory DebugInfo::GetModuleCategory() const
{
	if (this == nullptr) return ProcItemCategory::END;
	return m_moduleCategory;
}

void DebugInfo::Init(ProcItemCategory moduleCategory)
{
	if (this == nullptr) return;
	m_moduleCategory = moduleCategory;

	Reset();
}

void DebugInfo::Reset()
{
	if (this == nullptr) return;

	m_items.clear();
	m_keyToIndex.clear();
}

bool DebugInfo::AddItem(LanguageDef def)
{
	if (this == nullptr) return false;

	CString text = LANG_TEXT(def);
	if (m_keyToIndex.find(text) != m_keyToIndex.end())
	{
		return false;
	}

	DebugItemInfo info;
	info.m_key = text;

	long index = (long)m_items.size();
	m_keyToIndex[text] = index;
	m_items.push_back(info);

	return true;
}

void DebugInfo::AddOverlay(LanguageDef def, const IPVM::Point_32f_C2& overlay, COLORREF crColor)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_overlays_point32fC2.emplace_back(overlay, crColor);
	}
}

void DebugInfo::AddOverlay(LanguageDef def, const IPVM::Point_32s_C2& overlay, COLORREF crColor)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_overlays_point32sC2.emplace_back(overlay, crColor);
	}
}

void DebugInfo::AddOverlay(LanguageDef def, const IPVM::Rect& overlay, COLORREF crColor)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_overlays_rect.emplace_back(overlay, crColor);
	}
}
void DebugInfo::AddOverlay(LanguageDef def, const IPVM::Rect& overlay, CString str,  COLORREF crColor, long fontSize, COLORREF crTextColor)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_overlays_rect.emplace_back(overlay, crColor);

		DebugOverlayTextInfo info;
		info.m_pos = IPVM::Point_32f_C2(overlay.m_right, overlay.m_top);
		info.m_fontSize = fontSize;
		info.m_text = str;

		item->m_overlays_text.emplace_back(info, crTextColor);
	}
}
void DebugInfo::AddOverlay(LanguageDef def, const IPVM::Rect& overlay,bool ShowText, COLORREF crColor)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_overlays_rect.emplace_back(overlay, crColor);
		CString str;
		str.Format(_T("Left : %d  Right : %d  Top : %d  Bottom : %d \r\n"), overlay.m_left, overlay.m_right, overlay.m_top, overlay.m_bottom);
		item->m_resultText.AppendFormat(str);
	}
}

void DebugInfo::AddOverlay(LanguageDef def, const IPVM::Rect_32f& overlay, COLORREF crColor)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_overlays_rect_32f.emplace_back(overlay, crColor);
	}
}

void DebugInfo::AddOverlay(LanguageDef def, const IPVM::LineSeg_32f& overlay, COLORREF crColor)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_overlays_line.emplace_back(overlay, crColor);
	}
}

void DebugInfo::AddOverlay(LanguageDef def, const IPVM::Quadrangle_32f& overlay, COLORREF crColor)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_overlays_quadrangle32f.emplace_back(overlay, crColor);
	}
}

void DebugInfo::AddOverlayText(LanguageDef def, const IPVM::Point_32f_C2& pos, LPCTSTR text, COLORREF crColor, long fontSize)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		DebugOverlayTextInfo info;
		info.m_pos = pos;
		info.m_fontSize = fontSize;
		info.m_text = text;
		item->m_overlays_text.emplace_back(info, crColor);
	}
}

void DebugInfo::AddOverlayAndText(LanguageDef def, const IPVM::Point_32f_C2& overlay, LPCTSTR textTitle)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_overlays_point32fC2.emplace_back(overlay, RGB(255, 0, 0));
		if (!CString(textTitle).IsEmpty())
		{
			item->m_resultText.AppendFormat(_T("%s : "), textTitle);
		}

		item->m_resultText.AppendFormat(_T("%.2f, %.2f\r\n"), overlay.m_x, overlay.m_y);
	}
}

void DebugInfo::AddOverlayAndText(LanguageDef def, const IPVM::Rect_32f& overlay, LPCTSTR textTitle)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_overlays_rect_32f.emplace_back(overlay, RGB(255, 0, 0));
		if (!CString(textTitle).IsEmpty())
		{
			item->m_resultText.AppendFormat(_T("%s : "), textTitle);
		}

		item->m_resultText.AppendFormat(_T("Left: %.2f, Right: %.2f, Top: %.2f, Bottom: %.2f\r\n"),
			overlay.m_left, overlay.m_right, overlay.m_top, overlay.m_bottom);
	}
}

void DebugInfo::AddOverlayAndText(LanguageDef def, const std::vector<IPVM::Point_32s_C2>& overlays)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		for (long index = 0; index < (long)overlays.size(); index++)
		{
			item->m_overlays_point32sC2.emplace_back(overlays[index], RGB(255, 0, 0));
			item->m_resultText.AppendFormat(_T("%d, %d\r\n"), overlays[index].m_x, overlays[index].m_y);
		}
	}
}

void DebugInfo::AddText(LanguageDef def, const double value, LPCTSTR textTitle)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		if (!CString(textTitle).IsEmpty())
		{
			item->m_resultText.AppendFormat(_T("%s : "), textTitle);
		}

		item->m_resultText.AppendFormat(_T("%.2f\r\n"), value);
	}
}

void DebugInfo::AddText(LanguageDef def, const float value, LPCTSTR textTitle)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		if (!CString(textTitle).IsEmpty())
		{
			item->m_resultText.AppendFormat(_T("%s : "), textTitle);
		}

		item->m_resultText.AppendFormat(_T("%.2f\r\n"), value);
	}
}

DebugItemInfo* DebugInfo::FindItem(LanguageDef def)
{
	if (this == nullptr) return nullptr;

	return FindItem(LANG_TEXT(def));
}

DebugItemInfo* DebugInfo::FindItem(LPCTSTR key)
{
	if (this == nullptr) return nullptr;

	auto itKeyToIndex = m_keyToIndex.find(key);
	if (itKeyToIndex == m_keyToIndex.end())
	{
		return nullptr;
	}

	return &m_items[itKeyToIndex->second];
}

void DebugInfo::SetImage(LanguageDef def, const IPVM::Image_8u_C1& image)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_image.Create(image.GetSizeX(), image.GetSizeY());

		if (image.GetSizeX() > 0)
		{
			auto status = IPVM::ImageProcessing::Copy(image, IPVM::Rect(image), item->m_image);
			ASSERT(status == IPVM::Status::success);
		}
	}
}

void DebugInfo::SetOverlay(LanguageDef def, const std::vector<IPVM::Rect>& overlays, bool showDisplayNo, COLORREF crColor, bool noReset)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		if(!noReset)
			item->m_overlays_rect.clear();
		
		for (auto& overlay : overlays)
		{
		
			item->m_overlays_rect.emplace_back(overlay, crColor);
			DebugOverlayTextInfo info;
			
			
		}
		item->m_overlay_displayNo = showDisplayNo;
	}
}

void DebugInfo::SetOverlay(LanguageDef def, const std::vector<IPVM::Rect>& overlays, const std::vector<float>& fvalue, const float fstandard, COLORREF crColor_OK, COLORREF crColor_NG, bool noReset)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		if (!noReset)
			item->m_overlays_rect.clear();

		for (int index = 0; index< overlays.size(); index++)
		{
			COLORREF color;

			if (fstandard < fvalue[index])
				color = crColor_OK;
			else
				color = crColor_NG;
			CString strText;
			strText.Format(_T("%.2f"), fvalue[index]);

			DebugOverlayTextInfo info;
			info.m_pos = IPVM::Point_32f_C2(overlays[index].m_left, overlays[index].CenterPoint().m_y);
			info.m_fontSize = 3;
			info.m_text = strText;
			item->m_overlays_text.emplace_back(info, color);
			item->m_overlays_rect.emplace_back(overlays[index], color);

		}
		item->m_overlay_displayNo = false;
	}
}

void DebugInfo::SetOverlay(LanguageDef def, const std::vector<IPVM::Rect>& overlays, const std::vector<float>& values1, const std::vector<float>& values2, const std::vector<bool>& IsNoTabRect, bool showDisplayNo)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_overlays_rect.clear();
		item->m_overlays_text.clear();

		for (int index = 0; index < overlays.size() ; index++)
		{
			item->m_overlays_rect.emplace_back(overlays[index], RGB(255, 0, 0));

			auto& value1 = values1[index];
			auto& value2 = values2[index];
			item->m_resultText.AppendFormat(_T("%d  Width : %.2fum  Height : %.2fum \r\n"), index + 1, value1, value2);
		}

		int n = 0;

		for (int i=0; i< IsNoTabRect.size(); i++)
		{
			DebugOverlayTextInfo info;
			info.m_pos = IPVM::Point_32f_C2(item->m_overlays_rect[n].m_obj.m_right, item->m_overlays_rect[n].m_obj.m_top);
			info.m_fontSize = 70;
			info.m_text = IsNoTabRect[i] ? _T("No Tab") : _T("");

			item->m_overlays_text.emplace_back(info, RGB(255, 0, 0));

			n++;
		}
		item->m_overlay_displayNo = showDisplayNo;
	}
}

void DebugInfo::SetRejectInfo(const REJECT_INFO& info)
{
	if (this == nullptr) return;

	std::vector<IPVM::Rect> rejectRois;
	std::vector<float> rejectLengths;
	for (long index = 0; index < info.Count(); index++)
	{
		rejectRois.push_back(info[index].m_rect);
		rejectLengths.push_back(info[index].m_length);
	}
	
	SetOverlay(LanguageDef::Debug_Reject_Rectangle, rejectRois);

	SetText(LanguageDef::Debug_Reject_Length, rejectLengths);
}

void DebugInfo::SetText(LanguageDef def, LPCTSTR text)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_resultText = text;
	}
}
void DebugInfo::SetText(LanguageDef def, const std::vector<CString>& values)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_resultText = _T("");

		for (long index = 0; index < (long)values.size(); index++)
		{
			//auto& value = values[index]+ (LPCTSTR)_T("\r\n");
			CString value =(CString)values[index] + _T("\r\n");//  _T("\r\n");
			item->m_resultText.AppendFormat(value);
		}
	}

}
void DebugInfo::SetText(LanguageDef def, const std::vector<float>& values)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_resultText = _T("");

		for (long index = 0; index < (long)values.size(); index++)
		{
			auto& value = values[index];
			item->m_resultText.AppendFormat(_T("%d : %.2f\r\n"), index + 1, value);
		}
	}
}

void DebugInfo::SetText(LanguageDef def, const std::vector<long>& values)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_resultText = _T("");

		for (long index = 0; index < (long)values.size(); index++)
		{
			auto& value = values[index];
			item->m_resultText.AppendFormat(_T("%d : %d\r\n"), index + 1, value);
		}
	}
}

void DebugInfo::SetText(LanguageDef def, const float value)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_resultText.Format(_T("%.2f\r\n"), value);
	}
}

void DebugInfo::AddProfileVew(LanguageDef def, std::vector<BYTE>& profiles)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		item->m_profile.emplace_back(profiles);
	}
}

void DebugInfo::AddOverlay(LanguageDef def, const REJECT_INFO& info, COLORREF crColor, long fontSize)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		for (int i = 0; i < info.Count(); i++)
		{
			CString strText;

			if(_T("") == info[i].m_detail)
				strText.Format(_T("%d. Length : %.3f(mm) , Width : %.3f , Height : %.3f \r\n"), i + 1, info[i].m_length, info[i].m_width, info[i].m_height);
			else
				strText.Format(_T("%d. [ %s ] \r\n Length : %.3f(mm) , Width : %.3f , Height : %.3f \r\n"), i + 1, info[i].m_detail, info[i].m_length, info[i].m_width, info[i].m_height);

			item->m_resultText.AppendFormat(strText);

		}
	}
}


#define _PI           3.1415926535
void DebugInfo::DrawArrow(LanguageDef def, IPVM::Point_32f_C2 point, float inputDegree, int inputsize, bool bPointHead, COLORREF rgb, float fthick)
{
	if (this == nullptr) return;

	if (auto* item = FindItem(def))
	{
		//point 
		IPVM::LineSeg_32f Line[3];

		float fDegri[3];

		if (bPointHead)
		{
			fDegri[0] = (int(inputDegree + 180.0) % 360);
			fDegri[1] = (int(inputDegree + 160.0) % 360);
			fDegri[2] = (int(inputDegree + 200.0) % 360);
		}
		else
		{
			fDegri[0] = (int(inputDegree) % 360);
			fDegri[1] = (int(inputDegree + 160.0) % 360);
			fDegri[2] = (int(inputDegree + 200.0) % 360);
		}
		float radian[3];
		radian[0] = fDegri[0] * (_PI / 180.0);
		radian[1] = fDegri[1] * (_PI / 180.0);
		radian[2] = fDegri[2] * (_PI / 180.0);

		int nLength[3];
		nLength[0] = inputsize;
		nLength[1] = inputsize * 0.4;
		nLength[2] = inputsize * 0.4;

		int x[3];
		int y[3];

		for (int i = 0; i < 3; i++)
		{
			x[i] = (cos(radian[i]) * nLength[i]) * 1;
			y[i] = (sin(radian[i]) * nLength[i]) * -1;

			Line[i].m_sx = point.m_x;
			Line[i].m_sy = point.m_y;
			Line[i].m_ex = point.m_x + x[i];
			Line[i].m_ey = point.m_y + y[i];
			if (false == bPointHead && 1 <= i)
			{
				Line[i].m_sx = Line[0].m_ex;
				Line[i].m_sy = Line[0].m_ey;
				Line[i].m_ex = Line[0].m_ex + x[i];
				Line[i].m_ey = Line[0].m_ey + y[i];
			}
			item->m_overlays_line.emplace_back(Line[i], rgb);
		}

	}
}