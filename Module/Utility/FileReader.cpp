#include "pch.h"
#include "FileReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Utility;

bool FileReader::Load(LPCTSTR filePath, std::vector<BYTE>& o_data)
{
	CFile file;
	if (!file.Open(filePath, CFile::modeRead))
	{
		return false;
	}

	o_data.resize(file.GetLength());
	if (o_data.size() > 0)
	{
		file.Read(&o_data[0], (UINT)o_data.size());
	}

	file.Close();
	return true;
}
