#include "stdafx.h"

#include "IniHelper.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:4996)

bool IsKeyExist(LPCTSTR pathName, LPCTSTR category, LPCTSTR key)
{
	// 2020/7/31
	//  INI 파일에 키가 없는 경우 만들어 주는 코드가 있음
	//  기존 처리 방법: 읽어들인 값이 인자 Default와 같은 경우 Read 후 Write 함
	//  문제점: 값이 DEFAULT 값과 같은 경우 INI 파일이 계속 저장된다 (기록시간이 바뀜)
	//
	// 수정된 처리 방법
	//  Default 값으로 되어 있는 것을 읽었는지
	//  아니면 해당 Key 가 없는건지 확인하기 위해 Default 값을 바꿔서 읽어 보았다.
	//  실제 Key가 있는지 없는지 체크할 수 있으면 좋겠지만 따로 함수를 못찾아서
	//  일단 이렇게 하였다.
	//  Ini 파일이 큰 경우 쓸대없는 시간을 차지할 수도 있다고 생각한다.
	//  추후 좀 더 심플하게 체크할 수 있었으면 좋겠다.

	TCHAR strValue[MAX_PATH];

	GetPrivateProfileString(category, key, _T("0"), strValue, MAX_PATH - 1, pathName);
	CString ret1 = strValue;
	GetPrivateProfileString(category, key, _T("1"), strValue, MAX_PATH - 1, pathName);
	CString ret2 = strValue;

	return ret1 != _T("0") || ret2 != _T("1");
}

CString TrimSymbol(LPCTSTR  szName)
{
	// [] 은 Category 로 쓰이므로 분류명으로 사용되서는 안되는 내용이다.
	// <> 로 바꾸어주자
	CString strName(szName);

	strName.Replace(_T("["), _T("$#<"));
	strName.Replace(_T("]"), _T("$#>"));

	return strName;
}

CString IniHelper::MakeKeyName(LPCTSTR szNameHeader, LPCTSTR szKeyname, int nKeyPostNum)
{
	CString strNameHeader(szNameHeader);

	CString dest;
	CString dest2;

	if(strNameHeader == _T(""))
		dest = szKeyname;
	else
		dest.Format(_T("%s_%s"), szNameHeader, szKeyname);
		
	if(nKeyPostNum >= 0)
	{
		dest2.Format(_T("_%d"), nKeyPostNum);
		dest += dest2;
	}
	
	return dest;	
}

int16_t IniHelper::LoadSHORT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int16_t defValue)
{
	CString strValue;
	strValue.Format(_T("%d"), defValue);

	auto ret = LoadSTRING(pathName, TrimSymbol(category), key, strValue);

	if (ret == strValue && !IsKeyExist(pathName, TrimSymbol(category), key))
	{
		SaveSTRING(pathName, category, key, strValue);
	}

	return int16_t(::_ttol(ret));
}

void IniHelper::SaveSHORT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int16_t nValue)
{
	CString strValue;
	strValue.Format(_T("%d"), nValue);

	WritePrivateProfileString(TrimSymbol(category), key, strValue, pathName);
}

int IniHelper::LoadINT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int defValue)
{
	CString strValue;
	strValue.Format(_T("%d"), defValue);

	auto ret = LoadSTRING(pathName, TrimSymbol(category), key, strValue);

	if (ret == strValue && !IsKeyExist(pathName, TrimSymbol(category), key))
	{
		SaveSTRING(pathName, TrimSymbol(category), key, strValue);
	}

	return int(::_ttol(ret));
}

void IniHelper::SaveINT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int nValue)
{
	CString strValue;
	strValue.Format(_T("%d"), nValue);

	WritePrivateProfileString(TrimSymbol(category), key, strValue, pathName);
}

CString IniHelper::LoadSTRING(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CString defValue)
{
	TCHAR strValue[MAX_PATH];
	GetPrivateProfileString(TrimSymbol(category), key, defValue, strValue, MAX_PATH-1, pathName);

	CString ret = strValue;

	if (ret == defValue && !IsKeyExist(pathName, TrimSymbol(category), key))
	{
		SaveSTRING(pathName, TrimSymbol(category), key, defValue);
	}

	return ret;
}

void IniHelper::SaveSTRING(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CString strValue)
{
	WritePrivateProfileString(TrimSymbol(category), key, strValue, pathName);
}

float IniHelper::LoadFLOAT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, float defValue)
{
	CString strValue;
	strValue.Format(_T("%.40f"), defValue);

	auto ret = LoadSTRING(pathName, TrimSymbol(category), key, strValue);

	if (ret == strValue && !IsKeyExist(pathName, TrimSymbol(category), key))
	{
		SaveSTRING(pathName, TrimSymbol(category), key, strValue);
	}

	return float(::_ttof(ret));
}

void IniHelper::SaveFLOAT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, float dValue)
{
	CString strValue;
	strValue.Format(_T("%.40f"), dValue);

	WritePrivateProfileString(TrimSymbol(category), key, strValue, pathName);
}

double IniHelper::LoadDOUBLE(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, double defValue)
{
	CString strValue;
	strValue.Format(_T("%.40lf"), defValue);

	auto ret = LoadSTRING(pathName, TrimSymbol(category), key, strValue);

	if (ret == strValue && !IsKeyExist(pathName, TrimSymbol(category), key))
	{
		SaveSTRING(pathName, TrimSymbol(category), key, strValue);
	}

	return ::_ttof(ret);
}

void IniHelper::SaveDOUBLE(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, double dValue)
{
	CString strValue;
	strValue.Format(_T("%.40lf"), dValue);

	WritePrivateProfileString(TrimSymbol(category), key, strValue, pathName);
}

CPoint IniHelper::LoadPOINT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CPoint defValue)
{
	TCHAR szVal[MAX_PATH-1];
	
	CString strTemp;
	CPoint ptResult;

	strTemp.Format(_T("%d,%d"), defValue.x, defValue.y);
	GetPrivateProfileString(TrimSymbol(category), key, strTemp, szVal, MAX_PATH-1, pathName);

	_stscanf(szVal, _T("%d,%d"), &ptResult.x, &ptResult.y);
	
	return ptResult;
}

void IniHelper::SavePOINT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CPoint ptValue)
{
	CString strValue;
	strValue.Format(_T("%d,%d"), ptValue.x, ptValue.y);

	WritePrivateProfileString(TrimSymbol(category), key, strValue, pathName);
}

CRect IniHelper::LoadRECT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CRect defValue)
{
	TCHAR szVal[MAX_PATH-1];
	CString strTemp;
	CRect rtResult;

	strTemp.Format(_T("%d,%d,%d,%d"), defValue.left, defValue.top, defValue.right, defValue.bottom);
	GetPrivateProfileString(TrimSymbol(category), key, strTemp, szVal, MAX_PATH-1, pathName);

	_stscanf(szVal, _T("%d,%d,%d,%d"), &rtResult.left, &rtResult.top, &rtResult.right, &rtResult.bottom);
	
	return rtResult;
}

void IniHelper::SaveRECT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CRect rtValue)
{
	CString strValue;
	strValue.Format(_T("%d,%d,%d,%d"), rtValue.left, rtValue.top, rtValue.right, rtValue.bottom);

	WritePrivateProfileString(TrimSymbol(category), key, strValue, pathName);
}

uint64_t IniHelper::LoadULONGLONG(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint64_t defValue)
{
	TCHAR szVal[MAX_PATH-1];
	CString strDefault;
//	uint64_t lResult = 0;

	strDefault.Format(_T("%I64u"), defValue);
	GetPrivateProfileString(TrimSymbol(category), key, strDefault, szVal, MAX_PATH-1, pathName);

	return _ttoi64(szVal);
}

void IniHelper::SaveULONGLONG(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint64_t nValue)
{
	CString strValue;
	strValue.Format(_T("%I64u"), nValue);

	WritePrivateProfileString(TrimSymbol(category), key, strValue, pathName);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int16_t *nValue, int16_t nDefValue)
{
	*nValue = LoadSHORT(pathName, category, key, nDefValue);
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int16_t *nValue)
{
	SaveSHORT(pathName, category, key, *nValue);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int32_t *nValue, int32_t nDefValue)
{
	*nValue = LoadINT(pathName, category, key, nDefValue);
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int32_t *nValue)
{
	SaveINT(pathName, category, key, *nValue);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint32_t *nValue, uint32_t nDefValue)
{
	CString strValue;
	strValue.Format(_T("%I32u"), nDefValue);

	auto ret = LoadSTRING(pathName, TrimSymbol(category), key, strValue);

	if (ret == strValue && !IsKeyExist(pathName, TrimSymbol(category), key))
	{
		SaveSTRING(pathName, TrimSymbol(category), key, strValue);
	}

	*nValue = static_cast<uint32_t>(::_ttoi(ret));
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint32_t *nValue)
{
	CString strValue;
	strValue.Format(_T("%I32u"), *nValue);

	WritePrivateProfileString(TrimSymbol(category), key, strValue, pathName);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint64_t *nValue, uint64_t nDefValue)
{
	*nValue = LoadULONGLONG(pathName, category, key, nDefValue);
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint64_t *nValue)
{
	SaveULONGLONG(pathName, category, key, *nValue);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, long *nValue, long nDefValue)
{
	*nValue = LoadINT(pathName, category, key, (int)nDefValue);
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, long *nValue)
{
	SaveINT(pathName, category, key, (int)*nValue);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint16_t *nValue, uint16_t nDefValue)
{
	*nValue = (uint16_t)LoadINT(pathName, category, key, (int)nDefValue);
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint16_t *nValue)
{
	SaveINT(pathName, category, key, (int)*nValue);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CString *strValue, CString strDefValue)
{
	*strValue = LoadSTRING(pathName, category, key, strDefValue);
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CString *strValue)
{
	SaveSTRING(pathName, category, key, *strValue);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, float *fValue, float fDefValue)
{
	*fValue = LoadFLOAT(pathName, category, key, fDefValue);
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, float *fValue)
{
	SaveFLOAT(pathName, category, key, *fValue);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, double *fValue, double fDefValue)
{
	*fValue = LoadDOUBLE(pathName, category, key, fDefValue);
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, double *fValue)
{
	SaveDOUBLE(pathName, category, key, *fValue);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CPoint *ptValue, CPoint ptDefValue)
{
	*ptValue = LoadPOINT(pathName, category, key, ptDefValue);
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CPoint *ptValue)
{
	SavePOINT(pathName, category, key, *ptValue);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CRect *rtValue, CRect rtDefValue)
{
	*rtValue = LoadRECT(pathName, category, key, rtDefValue);
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CRect *rtValue)
{
	SaveRECT(pathName, category, key, *rtValue);
}

void IniHelper::Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CTimeSpan *tmValue, CTimeSpan tmDefValue)
{
	TCHAR szVal[MAX_PATH-1];
	CString strTemp;
	CRect rtResult;

	strTemp = tmDefValue.Format(_T("%D, %H, %M, %S"));
	GetPrivateProfileString(TrimSymbol(category), key, strTemp, szVal, MAX_PATH-1, pathName);

	long nD, nH, nM, nS;
	_stscanf(szVal, _T("%d, %d, %d, %d"), &nD, &nH, &nM, &nS);
	*tmValue = CTimeSpan(nD, nH, nM, nS);
}

void IniHelper::Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CTimeSpan *tmValue)
{
	CString strValue = tmValue->Format(_T("%D, %H, %M, %S"));
	WritePrivateProfileString(TrimSymbol(category), key, strValue, pathName);
}

void IniHelper::Delete(LPCTSTR  pathName, LPCTSTR  category)
{
	CString strCategory(TrimSymbol(category));

	//------------------------------------------------------------------------------------
	// 파일내용읽기
	//------------------------------------------------------------------------------------
	CString strTotal;
	{
		CFile file(pathName, CFile::modeRead);

		long	nFileLength =  (long)file.GetLength();
		BYTE*	pBuffer		= new BYTE[nFileLength+1];
		pBuffer[nFileLength] = 0;

		file.Read(pBuffer, nFileLength);
		file.Close();

		strTotal = pBuffer;
	}

	//------------------------------------------------------------------------------------
	// 줄별로 STRING 으로 만들기
	//------------------------------------------------------------------------------------

	std::vector<CString> vecLine;
	long nStartLine = -1;

	for (int32_t n=0; n<=strTotal.GetLength(); n++)
	{
		if((strTotal.GetAt(n) == 13 || n==strTotal.GetLength()) && nStartLine>=0)
		{
			CString str = strTotal.Mid(nStartLine, n-nStartLine);
			str = str.TrimLeft();
			str = str.TrimRight();

			vecLine.push_back(str);
			nStartLine = -1;
		}
		else if(n!=strTotal.GetLength() && nStartLine == -1 && strTotal.GetAt(n) != 13 && strTotal.GetAt(n) != 10)
		{
			nStartLine = n;
		}
	}

	CString strNCategory = _T("[") + strCategory + _T("]");
	long nDelete1 = -1;
	long nDelete2 = -1;

	for (int32_t n=0; n<(long)vecLine.size(); n++)
	{
		if(nDelete1 == -1 && vecLine[n] == strNCategory)
		{
			nDelete1 = n;
			continue;
		}

		if(nDelete1 != -1 && vecLine[n].Left(1) == _T("["))
		{
			nDelete2 = n;
			break;
		}
	}

	if(nDelete1 != -1 && nDelete2 == -1)
	{
		vecLine.erase(vecLine.begin() + nDelete1, vecLine.end());
	}

	if(nDelete1 != -1 && nDelete2 != -1)
	{
		vecLine.erase(vecLine.begin() + nDelete1, vecLine.begin() + nDelete2);
	}

	CString strNewLine;
	strNewLine.Format(_T("%c%c"), 13, 10);

	for (int32_t n=0; n<(long)vecLine.size(); n++)
	{
		vecLine[n] += strNewLine;
	}

	//------------------------------------------------------------------------------------
	// 파일내용쓰기
	//------------------------------------------------------------------------------------

	{
		CFile file(pathName, CFile::modeCreate | CFile::modeWrite);

		for (int32_t n=0; n<(long)vecLine.size(); n++)
		{
			BYTE *pBuffer	= nullptr;
			long nStrLength	= vecLine[n].GetLength();

			#ifdef _UNICODE
				pBuffer = new BYTE[(nStrLength+1) * 2];
				memset(pBuffer, 0, (nStrLength+1) *2);
				WideCharToMultiByte(CP_ACP,
									0,
									vecLine[n].GetBuffer(),
									-1,
									(LPSTR)pBuffer,
									(nStrLength+1) * 2,
									nullptr,
									nullptr);

				pBuffer[nStrLength] = 0;
				file.Write(pBuffer, (UINT)strlen((char *)pBuffer));
				delete []pBuffer;

			#else
				pBuffer = (BYTE *)vecLine[n].GetBuffer();
				file.Write(pBuffer, nStrLength);
			#endif
		}
		file.Close();
	}
}







