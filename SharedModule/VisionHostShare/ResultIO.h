#include "basedef.h"

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ResultIO : public CommonShare::BaseObject
	{
	public:
		ResultIO();
		~ResultIO();
		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
		void SetData(int& nIO, AREA_GRABMODE& Area_Grabmode, enCAMERA& CameraPos);

		AREA_GRABMODE		m_Area_Grabmode;
		enCAMERA			m_CameraPos;
		int					m_nIO;
	};
}

