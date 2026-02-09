 #pragma once

#define ANIMATION_DATA_MAX_NUM	(10)

class AnimationData;

class AnimationCustomizeBase
{
public:
	AnimationCustomizeBase();
	virtual ~AnimationCustomizeBase();

	virtual void DataSetZero() {}
	virtual void Draw(CDC* dc, const AnimationData& data, bool isDebugMode) = 0;
	virtual void Draw(CDC* dc, const DefectPointXY& data) = 0;
	virtual void UpdateViewSize(CDC* dc, long cx, long cy);


	long m_viewSizeX;
	long m_viewSizeY;

protected:
	
};

