#include "stdafx.h"
#include "ParaCollector.h"
#include "../../SharedModule/VisionHostShare/ParameterVision.h"

#include <afxmt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Base;

CCriticalSection g_csCollector;
Share::ParameterVision* g_collector;
bool g_changeParaOldValue = false;
CString g_cameraName;
CString g_moduleName;

FUNCTION_PARACOLLECTOR::FUNCTION_PARACOLLECTOR(Share::ParameterVision* collector)
{
	CSingleLock lock(&g_csCollector, TRUE);

	g_collector = collector;
	ParaCollector_UpdateOldPara(true);
}

FUNCTION_PARACOLLECTOR::~FUNCTION_PARACOLLECTOR()
{
	CSingleLock lock(&g_csCollector, TRUE);

	g_collector = nullptr;
	ParaCollector_UpdateOldPara(false);
}

FUNCTION_PARACOLLECTOR_CAMERA::FUNCTION_PARACOLLECTOR_CAMERA(LPCTSTR cameraName)
{
	CSingleLock lock(&g_csCollector, TRUE);
	g_cameraName = cameraName;
}

FUNCTION_PARACOLLECTOR_CAMERA::~FUNCTION_PARACOLLECTOR_CAMERA()
{
	CSingleLock lock(&g_csCollector, TRUE);
	g_cameraName = _T("");
}

FUNCTION_PARACOLLECTOR_MODULE::FUNCTION_PARACOLLECTOR_MODULE(LPCTSTR moduleName)
{
	CSingleLock lock(&g_csCollector, TRUE);
	g_moduleName = moduleName;
}

FUNCTION_PARACOLLECTOR_MODULE::~FUNCTION_PARACOLLECTOR_MODULE()
{
	CSingleLock lock(&g_csCollector, TRUE);
	g_moduleName = _T("");
}

void Base::ParaCollector_Add(LPCTSTR paraName, bool isChanged, LPCTSTR previousValue, LPCTSTR value, std::vector<int> ReleationItem)
{
	if (g_collector == nullptr) return;

	CSingleLock lock(&g_csCollector, TRUE);
	g_collector->Add(g_cameraName, g_moduleName, paraName, isChanged, previousValue, value, ReleationItem);
}

void Base::ParaCollector_UpdateOldPara(bool enable)
{
	CSingleLock lock(&g_csCollector, TRUE);
	g_changeParaOldValue = enable;
}
