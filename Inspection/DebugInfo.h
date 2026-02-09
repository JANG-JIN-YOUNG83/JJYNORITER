#pragma once
#include "basedef.h"
#include "ProcessingItemDef.h"
#include "RejectInfo.h"
#include "../VisionModule/Language/LanguageDef.h"

#include "Types/Image_8u_C1.h"
#include "Types/LineSeg_32f.h"
#include "Types/Point_32f_C2.h"
#include "Types/Point_32s_C2.h"
#include "Types/Rect.h"
#include "Types/Rect_32f.h"
#include "Types/Quadrangle_32f.h"

#include <map>
#include <vector>

namespace Inspection
{
	template <class _T> class DebugItemOverlayInfo
	{
	public:
		DebugItemOverlayInfo(const _T& obj, COLORREF color)
			: m_obj(obj)
			, m_color(color)
		{
		}

		_T m_obj;
		COLORREF m_color;
	};

	struct DebugOverlayTextInfo
	{
		IPVM::Point_32f_C2 m_pos;
		long m_fontSize;
		CString m_text;
	};

	class __INSPECTOIN_CLASS__ DebugItemInfo
	{
	public:
		DebugItemInfo();
		~DebugItemInfo();

		bool IsEmpty() const;
		bool IsOverlayEmpty() const;
		void UpdateImageView(ImageViewEx* imageView) const;
		void UpdateProfileView(IPVM::ProfileView* profileview) const;

		CString m_key;
		IPVM::Image_8u_C1 m_image;
		std::vector<DebugItemOverlayInfo<IPVM::Rect>> m_overlays_rect;
		std::vector<DebugItemOverlayInfo<IPVM::Rect>> m_overlays_rect_32f;
		std::vector<std::vector<BYTE>> m_profile;
		std::vector<DebugItemOverlayInfo<IPVM::Point_32f_C2>> m_overlays_point32fC2;
		std::vector<DebugItemOverlayInfo<IPVM::Point_32s_C2>> m_overlays_point32sC2;
		std::vector<DebugItemOverlayInfo<IPVM::LineSeg_32f>> m_overlays_line;
		std::vector<DebugItemOverlayInfo<IPVM::Quadrangle_32f>> m_overlays_quadrangle32f;
		std::vector<DebugItemOverlayInfo<DebugOverlayTextInfo>> m_overlays_text;
		bool m_overlay_displayNo;

		CString m_resultText;
	};

	class __INSPECTOIN_CLASS__ DebugInfo
	{
	public:
		DebugInfo();
		~DebugInfo();

		const DebugItemInfo& GetAt(long index) const;
		long GetItemCount() const;
		ProcItemCategory GetModuleCategory() const;

		void Init(ProcItemCategory moduleCategory);
		void Reset();

		bool AddItem(LanguageDef def);
		void AddOverlay(LanguageDef def, const IPVM::Point_32f_C2& overlay, COLORREF crColor = RGB(255, 0, 0));
		void AddOverlay(LanguageDef def, const IPVM::Point_32s_C2& overlay, COLORREF crColor = RGB(255, 0, 0));
		void AddOverlay(LanguageDef def, const IPVM::Rect& overlay, COLORREF crColor = RGB(255, 0, 0));
		void AddOverlay(LanguageDef def, const IPVM::Rect& overlay, CString str, COLORREF crColor, long fontSize = 2, COLORREF crTextColor = RGB(255, 0, 0));
		void AddOverlay(LanguageDef def, const IPVM::Rect& overlay,bool ShowText, COLORREF crColor = RGB(255, 0, 0));
		void AddOverlay(LanguageDef def, const IPVM::Rect_32f& overlay, COLORREF crColor = RGB(255, 0, 0));
		void AddOverlay(LanguageDef def, const IPVM::LineSeg_32f& overlay, COLORREF crColor = RGB(255, 0, 0));
		void AddOverlay(LanguageDef def, const IPVM::Quadrangle_32f& overlay, COLORREF crColor = RGB(255, 0, 0));
		void AddOverlay(LanguageDef def, const REJECT_INFO& info, COLORREF crColor = RGB(255, 0, 0), long fontSize = 8);
		void AddOverlayText(LanguageDef def, const IPVM::Point_32f_C2& pos, LPCTSTR text, COLORREF crColor = RGB(255, 0, 0), long fontSize = 8);

		template <class _T> void AddOverlay(LanguageDef def, const std::vector<_T>& overlays, COLORREF crColor = RGB(255, 0, 0))
		{
			for (auto& overlay : overlays)
			{
				AddOverlay(def, overlay, crColor);
			}
		}

		void AddOverlayAndText(LanguageDef def, const IPVM::Point_32f_C2& overlay, LPCTSTR textTitle = _T(""));
		void AddOverlayAndText(LanguageDef def, const IPVM::Rect_32f& overlay, LPCTSTR textTitle = _T(""));
		void AddOverlayAndText(LanguageDef def, const std::vector<IPVM::Point_32s_C2>& overlays);
		void AddText(LanguageDef def, const double value, LPCTSTR textTitle = _T(""));
		void AddText(LanguageDef def, const float value, LPCTSTR textTitle = _T(""));
		void AddProfileVew(LanguageDef def, std::vector<BYTE>& profiles);

		DebugItemInfo* FindItem(LanguageDef def);
		DebugItemInfo* FindItem(LPCTSTR key);

		void SetImage(LanguageDef def, const IPVM::Image_8u_C1& image);
		void SetOverlay(LanguageDef def, const std::vector<IPVM::Rect>& overlays, bool showDisplayNo = false, COLORREF crColor = RGB(255, 0, 0),bool noReset=false);
		void SetOverlay(LanguageDef def, const std::vector<IPVM::Rect>& overlays, const std::vector<float>& fvalue, const float fstandard, COLORREF crColor_OK, COLORREF crColor_NG, bool noReset);
		void SetOverlay(LanguageDef def, const std::vector<IPVM::Rect>& overlays, const std::vector<float>& values1, const std::vector<float>& values2, const std::vector<bool>& IsNoTabRect, bool showDisplayNo = false);
		void SetRejectInfo(const REJECT_INFO& info);
		void SetText(LanguageDef def, LPCTSTR text);
		void SetText(LanguageDef def, const std::vector<CString>& values);
		void SetText(LanguageDef def, const std::vector<float>& values);
		void SetText(LanguageDef def, const std::vector<long>& values);
		void SetText(LanguageDef def, const float value);

		void DrawArrow(LanguageDef def, IPVM::Point_32f_C2 point, float inputDegree, int inputsize, bool bPointHead, COLORREF rgb, float fthick = 1.0);


	private:
		ProcItemCategory m_moduleCategory;
		std::vector<DebugItemInfo> m_items;
		std::map<CString, long> m_keyToIndex;
	};
}
