#pragma once
#include <map>
#include <vector>

#define NODATA -9000

enum class LimitType
{
	Default = 0,
	UCL,
	USL,
};

struct Adata
{
	int   nIndex;	//그냥 가지고 있자.
	std::vector<float> vecfValue;
	BOOL bJudge;

	Adata &operator=(const Adata &src)
	{
		nIndex = src.nIndex;
		vecfValue = src.vecfValue;
		bJudge = src.bJudge;

		return *this;
	}

	void ClearData(long groupCount)
	{
		nIndex = -1;
		vecfValue.clear();
		vecfValue.resize(groupCount, -9999.f);
		bJudge = TRUE;
	}

	Adata()
	{
		ClearData(2);
	}
};

class GraphData
{
public:
	GraphData();
	~GraphData();

	CString m_title;

	float m_fSpecNominal;
	float m_fSpecMin;
	float m_fSpecMax;
	float m_fOffset;

	float m_fSpecAiMin;
	float m_fSpecAiMax;

	float m_fSpecNgMin;
	float m_fSpecNgMax;

	float m_fYaxisMin;
	float m_fYaxisMax;

	long m_headIndex;
	long m_tailIndex;

	float m_fSpecUCL;
	float m_fSpecLCL;

	float m_fSpecUSL;
	float m_fSpecLSL;

	long m_SpecX;

	const Adata& GetPoint(long index) const;
	long GetPointCount() const;
	long GetGroupCount() const;
	long GetMaxPointCount() const;

	void DataSetZero(long dataGroupCount);
	void Init(long xAxisCount, long dataGroupCount);
	bool SetAccMaxDiffData(long id, int index, float value, BOOL bJudge);
	bool SetData(long id, int index, float value, BOOL bJudge);

private:
	std::vector<Adata> m_pointValues;
	std::map<long, long> m_xindexToPointIndex;
};
