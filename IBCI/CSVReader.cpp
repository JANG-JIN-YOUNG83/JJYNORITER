#include "pch.h"
#include "CSVReader.h"
#include <locale>
//#include "../HostBase/Language.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSVReader::CSVReader()
{
}

CSVReader::~CSVReader()
{
}/*

auto textlines = LoadFileTextLines(filePath);
if (textlines == nullptr) return false;

CSVReader csv;
csv.Open(*textlines);

long index_time = csv.FindColumnIndex(_T("TIME"));
long index_cell = csv.FindColumnIndex(_T("CELL"));
long index_model = csv.FindColumnIndex(_T("MODEL"));
long index_lane = csv.FindColumnIndex(_T("LANE"));
long index_marking = csv.FindColumnIndex(_T("INK"));*/
std::shared_ptr<Utility::FileTextLines> CSVReader::LoadFileTextLines(LPCTSTR filePath)
{
	/*HostBase::Language::SetLocaleForLanguage();*/
	setlocale(LC_ALL, "korean");
	auto data = std::make_shared<Utility::FileTextLines>();
	if (!data->Load(filePath)) return nullptr;

	return data;
}
long CSVReader::FindColumnIndex(LPCTSTR columnName) const
{
	auto upperColumnName = CString(columnName).Trim().MakeUpper();
	for (long index = 0; index < (long)m_columnNames.size(); index++)
	{
		CString columnName = m_columnNames[index];
		if (columnName.MakeUpper() == upperColumnName)
		{
			return index;
		}
	}

	return -1;
}

long CSVReader::GetItemCount() const
{
	return max(0, (long)m_itemValues.size() - 1);
}

LPCTSTR	CSVReader::GetItemText(long rowIndex, long columnIndex) const
{
	auto itValues = m_itemValues.find(rowIndex + 1);
	if (itValues == m_itemValues.end()) return _T("");

	if (columnIndex < 0 || columnIndex >= (long)itValues->second.size()) return _T("");

	return itValues->second[columnIndex];
}

void CSVReader::Open(const Utility::FileTextLines& lines)
{
	/*HostBase::Language::SetLocaleForLanguage();*/
	setlocale(LC_ALL, "korean");
	m_columnNames.clear();
	m_itemValues.clear();

	long rowIndex = 0;

	for (long index = 0; index < lines.GetCount(); index++)
	{
		if (AddLine(lines.GetAt(index), rowIndex))
		{
			rowIndex++;
		}
	}
}

bool CSVReader::AddLine(LPCTSTR text, long rowIndex)
{
	CString line = CString(text).Trim();
	if (line.IsEmpty()) return false;

	auto& itemValues = m_itemValues[rowIndex];

	while (true)
	{
		long columnE = line.FindOneOf(_T(",\t"));

		CString columnValue;
		if (columnE < 0)
		{
			columnValue = line;
		}
		else
		{
			columnValue = line.Left(columnE);
			line = line.Mid(columnE + 1);
		}

		columnValue = columnValue.Trim();
		itemValues.push_back(columnValue);

		if (m_columnNames.size() < itemValues.size())
		{
			if (rowIndex != 0)
			{
				m_columnNames.push_back(_T(""));
			}
			else
			{
				// 첫번째 줄이다 해당줄의 내용은 Column으로 취급한다
				m_columnNames.push_back(columnValue);
			}
		}

		if (columnE < 0)
		{
			break;
		}
	}

	return true;
}
