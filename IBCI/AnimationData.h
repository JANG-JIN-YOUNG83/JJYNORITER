#pragma once
#include <map>
#include <vector>

#define NODATA -9000

struct _sAdata
{
	int   nIndex;	//그냥 가지고 있자.
	std::vector<float> vecfValue;
	BOOL bJudge;

	_sAdata &operator=(const _sAdata &src)
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

	_sAdata()
	{
		ClearData(2);
	}
};

class AnimationData
{
public:
	AnimationData();
	~AnimationData();
	ResultMismatch m_Mismatch;

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

	long m_nLaneCount;

	bool m_bOSDS;
    bool m_bRollerOS;

	const _sAdata& GetPoint(long index) const;
	long GetPointCount() const;
	long GetGroupCount() const;
	long GetMaxPointCount() const;

	void DataSetZero(long dataGroupCount);
	void Init(long xAxisCount, long dataGroupCount);
	bool SetAccMaxDiffData(long id, int index, float value, BOOL bJudge);
	bool SetData(long id, int index, float value, BOOL bJudge);

private:
	std::vector<_sAdata> m_pointValues;
	std::map<long, long> m_xindexToPointIndex;
};
