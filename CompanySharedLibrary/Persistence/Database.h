#pragma once

#include "basedef.h"

#include <vector>
#include <map>

class ArchiveAllType;

namespace IntekPlus
{
	namespace VisionMadang
	{
		struct Point_32s_C2;
		struct Rect;
		struct Size_32s;
	}
}
namespace IPVM = IntekPlus::VisionMadang;

//======================================================================================================
// DB 내용
//======================================================================================================

class __INTEKPLUS_SHARED_PERSISTENCE_API__ Database
{
public:
	Database(void);
	Database(const Database &Object);

	~Database(void);

public:
	long GetType(void);
	bool operator != (const Database &Object);
	bool operator == (const Database &Object);
	Database& operator = (const Database &Object);
	Database& operator [] (const wchar_t * strIndex);

	//=================================================================================
	// Link 관련
	//=================================================================================

	bool Link(bool bSave, uint8_t &nData);
	bool Link(bool bSave, int16_t &nData);
	bool Link(bool bSave, int32_t &bData);
	bool Link(bool bSave, bool &bData);
	bool Link(bool bSave, long &nData, long nDef = 0);
	bool Link(bool bSave, uint32_t &nData);
	bool Link(bool bSave, unsigned long &nData);
	bool Link(bool bSave, float &fData);
	bool Link(bool bSave, double &fData);
	bool Link(bool bSave, CStringW &strData);
	bool Link(bool bSave, CPoint &ptData);
	bool Link(bool bSave, IPVM::Point_32s_C2 &ptData);
	bool Link(bool bSave, CSize &szData);
	bool Link(bool bSave, IPVM::Size_32s &szData);
	bool Link(bool bSave, CRect &rtData);
	bool Link(bool bSave, IPVM::Rect &rtData);
	bool Link(bool bSave, int64_t &nData);
	bool Link(bool bSave, uint64_t &nData);

	template <typename T> bool Link(bool bSave, std::vector<T> &vecData)
	{
		long  nElementNum = (long)vecData.size();

		bool ret = (*this)[_T("size")].Link(bSave, nElementNum);

		if (true != ret)
		{
			nElementNum = 0;
		}

		if (!bSave)
		{
			vecData.resize(nElementNum);
		}

		if (nElementNum>0)
		{
			T *pT = &vecData[0];
			(*this)[_T("vector")].LinkArray(bSave, pT, nElementNum);
		}

		return ret;
	}

	template <typename T> bool LinkVectorClass(bool bSave, std::vector<T> &vecData)
	{
		long  nElementNum = (long)vecData.size();

		bool ret = (*this)[_T("size")].Link(bSave, nElementNum);

		if (true != ret)
		{
			nElementNum = 0;
		}

		if (!bSave)
		{
			vecData.resize(nElementNum);
		}

		CStringW str;
		for (int i = 0; i<nElementNum; i++)
		{
			str.Format(_T("%d"), i);
			(*this)[str].Link(bSave, vecData[i]);
		}

		return ret;
	}

	template <typename T1, typename T2> bool Link(bool bSave, std::map<T1, T2> &mapData)
	{
		long  nElementNum = (long)mapData.size();

		bool ret = (*this)[_T("size")].Link(bSave, nElementNum);

		if (true != ret)
		{
			nElementNum = 0;
		}

		if (!bSave)
		{
			mapData.clear();
		}

		CStringW str;
		std::map<T1, T2>::iterator it = mapData.begin();

		for (int i = 0; i<nElementNum; i++)
		{
			T1 First;
			T2 Second;

			if (bSave)
			{
				First = it->first;
				Second = it->second;
			}

			str.Format(_T("%d"), i);
			(*this)[str][_T("First")].Link(bSave, First);
			(*this)[str][_T("Second")].Link(bSave, Second);

			if (bSave) it++;
			else
			{
				mapData[First] = Second;
			}
		}

		return ret;
	}

	template <typename T> bool Link(bool bSave, T &Data)
	{
		return Data.LinkDataBase(bSave, *this);
	}

	bool SafeLink(bool bSave, long &nData);
	bool SafeLink(bool bSave, uint64_t &nData);

	template <typename T> bool SafeLink(bool bSave, std::vector<T> &vecData)
	{
		return __SafeLinkVector_Base(bSave, vecData);
	}

	long GetLinkArrayNum();
	bool LinkArray(bool bSave, uint8_t *pnArray, long nArrayNum);
	bool LinkArray(bool bSave, int16_t *pnArray, long nArrayNum);
	bool LinkArray(bool bSave, int32_t *pbArray, long nArrayNum);
	bool LinkArray(bool bSave, long *plArray, long nArrayNum);
	bool LinkArray(bool bSave, unsigned long *plArray, long nArrayNum);
	bool LinkArray(bool bSave, float *pfArray, long nArrayNum);
	bool LinkArray(bool bSave, double *pfArray, long nArrayNum);
	bool LinkArray(bool bSave, CStringW	*pStrArray, long nArrayNum);
	bool LinkArray(bool bSave, CPoint *pptArray, long nArrayNum);
	bool LinkArray(bool bSave, IPVM::Point_32s_C2 *pptArray, long nArrayNum);
	bool LinkArray(bool bSave, CSize *pszArray, long nArrayNum);
	bool LinkArray(bool bSave, IPVM::Size_32s *pszArray, long nArrayNum);
	bool LinkArray(bool bSave, CRect *prtArray, long nArrayNum);
	bool LinkArray(bool bSave, IPVM::Rect *prtArray, long nArrayNum);
	bool LinkArray(bool bSave, int64_t *plArray, long nArrayNum);
	bool LinkArray(bool bSave, uint64_t *plArray, long nArrayNum);
	bool SafeLinkArray(bool bSave, uint64_t *plArray, long nArrayNum);

	Database&	GetSubDBFmt(const wchar_t * szFmt, ...);
	void DelSubDB(const wchar_t * strSubDBName);
	void Reset(void);

	bool Load(const void* buffer, long bufferSize);
	bool Load(LPCTSTR strFileName);
	bool Save(LPCTSTR strFileName);

	void GetBuffer(void **buffer, long *bufferSize);
	void ReleaseBuffer(void *buffer);

	bool Serialize(ArchiveAllType &ar);

private:
	bool Load(CFile &File);
	bool Save(CFile &File);
	template <typename T> bool __SafeLinkVector_Base(bool bSave, std::vector<T> &vecData)
	{
		long  nElementNum = (long)vecData.size();

		bool ret = (*this)[_T("size")].Link(bSave, nElementNum);

		if (true != ret)
		{
			nElementNum = 0;
		}

		if (!bSave)
		{
			vecData.resize(nElementNum);
		}

		if (nElementNum>0)
		{
			T *pT = &vecData[0];
			(*this)[_T("vector")].SafeLinkArray(bSave, pT, nElementNum);
		}

		return ret;
	}

	long __GetStringBufferSize(CStringW &strData, WCHAR **pStrBuffer = nullptr);
	void __BinaryData_SetSize(ULONG nSize);
	void __BinaryData_Link(bool bSave, BYTE *pData, long nSize);
	void __BinaryData_LinkString(bool bSave, CStringW &strData);
	bool __LinkReady(bool bSave, long nLinkType);

	bool __Serialize_Old(ArchiveAllType &ar);

	long	__nBinaryDataCount;
	long	__nDataType;
	BYTE*	__pBinary;
	ULONG	__nBinarySize;

	std::map<CStringW, Database> *__pDataDB;		// 하위 DB
};
