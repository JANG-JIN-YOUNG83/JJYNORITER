#include "pch.h"
#include "GraphData.h"

//#include "Common.h"
//#include "../InterfaceModules/HostBase/Site.h"
//#include "../InterfaceModules/HostData/SystemParameter.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GraphData::GraphData()
	: m_fSpecNominal(NOISE_VALUE_32F)
	, m_fOffset(0)
	, m_fSpecMin(NOISE_VALUE_32F)
	, m_fSpecMax(NOISE_VALUE_32F)
	, m_fSpecAiMin(NOISE_VALUE_32F)
	, m_fSpecAiMax(NOISE_VALUE_32F)
	, m_fSpecNgMin(NOISE_VALUE_32F)
	, m_fSpecNgMax(NOISE_VALUE_32F)
	, m_fYaxisMin(NOISE_VALUE_32F)
	, m_fYaxisMax(NOISE_VALUE_32F)
	, m_fSpecUCL(NOISE_VALUE_32F)
	, m_fSpecLCL(NOISE_VALUE_32F)
	, m_fSpecUSL(NOISE_VALUE_32F)
	, m_fSpecLSL(NOISE_VALUE_32F)
	, m_headIndex(0)
	//, m_tailIndex(g_systemParameter.systemOption.m_graphMaxPointNum)
	//, m_pointValues(g_systemParameter.systemOption.m_graphMaxPointNum)
	, m_tailIndex(50)
	, m_pointValues(50)
{
}

GraphData::~GraphData()
{
}

const Adata& GraphData::GetPoint(long index) const
{
	return m_pointValues[(index + m_headIndex) % ((long)m_pointValues.size())];
}

long GraphData::GetPointCount() const
{
	return m_tailIndex - m_headIndex;
}

long GraphData::GetGroupCount() const
{
	if (m_pointValues.size() == 0) return 0;
	return (long)m_pointValues[0].vecfValue.size();
}

long GraphData::GetMaxPointCount() const
{
	return (long)m_pointValues.size();
}

void GraphData::Init(long xAxisCount, long dataGroupCount)
{
	m_pointValues.resize(xAxisCount);

	DataSetZero(dataGroupCount);
}

bool GraphData::SetAccMaxDiffData(long id, int index, float value, BOOL bJudge)
{
	auto itPointIndex = m_xindexToPointIndex.find(index);
	long maxBufferCount = (long)m_pointValues.size();

	if (itPointIndex == m_xindexToPointIndex.end())
	{
		long writeIndex = m_tailIndex % maxBufferCount;

		if (id < 0 || id >= (long)m_pointValues[writeIndex].vecfValue.size()) return false;

		if (m_tailIndex - m_headIndex == maxBufferCount)
		{
			auto xIndex = m_pointValues[m_headIndex % maxBufferCount].nIndex;
			m_xindexToPointIndex.erase(xIndex);
			m_headIndex++;
		}

		m_xindexToPointIndex[index] = writeIndex;
		m_pointValues[writeIndex].ClearData((long)m_pointValues[writeIndex].vecfValue.size());
		m_pointValues[writeIndex].nIndex = index;
		m_pointValues[writeIndex].vecfValue[id] = value;
		m_pointValues[writeIndex].bJudge = m_pointValues[writeIndex].bJudge;
		m_tailIndex++;
	}
	else
	{
		auto& point = m_pointValues[itPointIndex->second];
		if (id < 0 || id >= (long)point.vecfValue.size()) return false;

		float oldDiff = max(m_fSpecMin - point.vecfValue[id], point.vecfValue[id] - m_fSpecMax);
		float newDiff = max(m_fSpecMin - value, value - m_fSpecMax);

		if (oldDiff < NODATA || oldDiff < newDiff)
		{
			point.vecfValue[id] = value;
		}

		point.bJudge = min(point.bJudge, bJudge);
	}

	if (m_headIndex > maxBufferCount && m_tailIndex > maxBufferCount)
	{
		// 헤드, 테일이 무한정 길어지는 것 방지용
		m_headIndex -= maxBufferCount;
		m_tailIndex -= maxBufferCount;
	}

	return true;
}

bool GraphData::SetData(long id, int index, float value, BOOL bJudge)
{
	auto itPointIndex = m_xindexToPointIndex.find(index);
	long maxBufferCount = (long)m_pointValues.size();

	if (itPointIndex == m_xindexToPointIndex.end())
	{
		long writeIndex = m_tailIndex % maxBufferCount;

		if (id < 0 || id >= (long)m_pointValues[writeIndex].vecfValue.size()) return false;

		if (m_tailIndex - m_headIndex == maxBufferCount)
		{
			auto xIndex = m_pointValues[m_headIndex % maxBufferCount].nIndex;
			m_xindexToPointIndex.erase(xIndex);
			m_headIndex++;
		}

		m_xindexToPointIndex[index] = writeIndex;
		m_pointValues[writeIndex].ClearData((long)m_pointValues[writeIndex].vecfValue.size());
		m_pointValues[writeIndex].nIndex = index;
		m_pointValues[writeIndex].vecfValue[id] = value;
		m_pointValues[writeIndex].bJudge = m_pointValues[writeIndex].bJudge;
		m_tailIndex++;
	}
	else
	{
		auto& point = m_pointValues[itPointIndex->second];
		if (id < 0 || id >= (long)point.vecfValue.size()) return false;
		////중국사이트 그래프,리스트 countreset시 초기화 안되도록 수정 22.03.25
		//if ((HostBase::enumSite::LGESNB == HostBase::Site::Get() || HostBase::enumSite::LGESNA == HostBase::Site::Get() || HostBase::enumSite::LGESNJ == HostBase::Site::Get()))
		//{
		//	if (point.vecfValue[id] > -9955.) {
		//		m_xindexToPointIndex.clear();
		//		return SetData(id, index, value, bJudge);
		//	}
		//}
		point.vecfValue[id] = value;
		point.bJudge = min(point.bJudge, bJudge);
	}

	if (m_headIndex > maxBufferCount && m_tailIndex > maxBufferCount)
	{
		// 헤드, 테일이 무한정 길어지는 것 방지용
		m_headIndex -= maxBufferCount;
		m_tailIndex -= maxBufferCount;
	}

	return true;
}

void GraphData::DataSetZero(long dataGroupCount)
{
	for (int i = 0; i < m_pointValues.size(); i++)
	{
		m_pointValues[i].ClearData(dataGroupCount);
	}

	m_headIndex = 0;
	m_tailIndex = (long)m_pointValues.size();
	m_xindexToPointIndex.clear();
}
