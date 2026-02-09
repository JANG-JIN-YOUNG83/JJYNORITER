#pragma once
#include <map>
#include <vector>
#include "../Module/Utility/FileTextLines.h"
#include <memory>
class CSVReader
{
public:
	CSVReader();
	~CSVReader();

	long	FindColumnIndex(LPCTSTR columnName) const;
	long	GetItemCount() const;
	LPCTSTR	GetItemText(long rowIndex, long columnIndex) const;
	std::shared_ptr<Utility::FileTextLines> LoadFileTextLines(LPCTSTR filePath);
	void Open(const Utility::FileTextLines& lines);

private:
	bool AddLine(LPCTSTR text, long rowIndex);

	std::vector<CString> m_columnNames;
	std::map<long, std::vector<CString>> m_itemValues;
};

