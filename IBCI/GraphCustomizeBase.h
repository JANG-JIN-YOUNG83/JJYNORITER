 #pragma once

#define GRAPH_DATA_MAX_NUM	(10)

class GraphData;

class GraphCustomizeBase
{
public:
	GraphCustomizeBase();
	virtual ~GraphCustomizeBase();

	virtual void DataSetZero() {}
	virtual void Draw(CDC* dc, const GraphData& data, bool isDebugMode) = 0;
	virtual void Draw(CDC* dc, const DefectPointXY& data) = 0;
	virtual void UpdateViewSize(CDC* dc, long cx, long cy);


	long m_viewSizeX;
	long m_viewSizeY;

protected:
	
};

