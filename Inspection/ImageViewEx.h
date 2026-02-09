#pragma once
#include "basedef.h"
#include "Types/Image_8u_C1.h"
#include "Widget/ImageView.h"

#include <vector>

namespace Inspection
{
	class __INSPECTOIN_CLASS__ ImageViewEx : public IPVM::ImageView
	{
	public:
		ImageViewEx(HWND hwndParent, const IPVM::Rect& rtPosition, const long dlgCtrlID = 0);
		ImageViewEx(HWND hwndParent, const int frameRsrcID, const long dlgCtrlID = 0);
		void SetImage(const IPVM::Image_8u_C1& image);

		void GetProfile(const IPVM::LineSeg_32f& line, std::vector<BYTE>& o_profiles);

	private:
		IPVM::Image_8u_C1 m_sourceImage;
	};

}
