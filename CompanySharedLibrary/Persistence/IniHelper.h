#pragma once

#include "basedef.h"
#include "stdint.h"

class __INTEKPLUS_SHARED_PERSISTENCE_API__ IniHelper
{
public:
	static CString	MakeKeyName(LPCTSTR szNameHeader, LPCTSTR szKeyname, int nKeyPostNum);

	static int16_t			LoadSHORT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int16_t defValue = 0);
	static void				SaveSHORT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int16_t value);
	static int				LoadINT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int defValue = 0);
	static void				SaveINT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int value);
	static CString			LoadSTRING(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CString defValue = _T(""));
	static void				SaveSTRING(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CString value);
	static float			LoadFLOAT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, float defValue = 0.f);
	static void				SaveFLOAT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, float value);
	static double			LoadDOUBLE(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, double defValue = 0.);
	static void				SaveDOUBLE(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, double value);
	static CPoint			LoadPOINT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CPoint defValue = CPoint(0, 0));
	static void				SavePOINT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CPoint value);
	static CRect			LoadRECT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CRect defValue = CRect(0, 0, 0, 0));
	static void				SaveRECT(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CRect value);

	static uint64_t		LoadULONGLONG(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint64_t defValue = 0);
	static void				SaveULONGLONG(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint64_t value);


	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int16_t *nValue, int16_t nDefValue=0);
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int16_t *nValue);
	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint16_t *nValue, uint16_t nDefValue = 0);
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint16_t *nValue);
	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int32_t *nValue, int32_t nDefValue = 0);
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, int32_t *nValue);
	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint32_t *nValue, uint32_t nDefValue = 0);
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint32_t *nValue);
	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint64_t *nValue, uint64_t nDefValue=0);
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, uint64_t *nValue);
	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, long *nValue, long nDefValue=0);
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, long *nValue);
	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CString *strValue, CString strDefValue = _T(""));
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CString *strValue);
	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, float *nValue, float nDefValue=0);
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, float *nValue);
	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, double *fValue, double fDefValue=0.f);
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, double *fValue);
	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CPoint *ptValue, CPoint ptDefValue=CPoint(0, 0));
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CPoint *ptValue);
	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CRect *rtValue, CRect rtDefValue=CRect(0, 0, 0, 0));
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CRect *rtValue);
	static void		Read(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CTimeSpan *tmValue, CTimeSpan tmDefValue = CTimeSpan());
	static void		Write(LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, CTimeSpan *tmValue);
	static void		Delete(LPCTSTR  pathName, LPCTSTR  category);

	template <class T>
	static void Link(bool bSave, LPCTSTR  pathName, LPCTSTR  category, LPCTSTR  key, T *value)
	{
		if(bSave)
			Write(pathName, category, key, value);
		else
			Read(pathName, category, key, value, *value);
	}
};
