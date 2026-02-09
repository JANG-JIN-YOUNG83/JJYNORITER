#pragma once
#include "basedef.h"
#include <map>
#include <vector>

namespace CommonShare
{
	class __COMMON_SHARE_CLASS__ ArchiveAllType : public CArchive
	{
	public:
		ArchiveAllType(CFile* pFile, UINT nMode);
		virtual ~ArchiveAllType();

		typedef BYTE* BYTEPTR;

		void Serialize_Element(CommonShare::BaseObject& data);
		void Serialize_Element(CStringW& istr);
		void Serialize_Element(SYSTEMTIME& stime);
		void Serialize_Element(bool& data);
		void Serialize_Element(double& data);
		void Serialize_Element(float& data);
		void Serialize_Element(int& data);
		void Serialize_Element(long& data);
		void Serialize_Element(int64_t& data);

		void Serialize_Binary(BYTEPTR& pData, ULONG& nDataNum);
		void Serialize_RawData(BYTE* pData, ULONG nDataNum);

		template <class T> void Serialize_Element(std::vector<T>& t)
		{
			long count = (long)t.size();
			if (IsLoading()) t.clear();

			if (count < 0) count = 0; //20230719 [jjh]

			Serialize_Element(count);
			if (IsLoading()) t.resize(count);

			for (auto& element : t)
			{
				Serialize_Element(element);
			}
		};

		template <class Key, class Value> void Serialize_Element(std::map<Key, Value>& t)
		{
			long count = (long)t.size();
			if (IsLoading()) t.clear();

			Serialize_Element(count);
			if (IsLoading())
			{
				t.clear();
				for (long index = 0; index < count; index++)
				{
					Key key;
					Serialize_Element(key);
					Serialize_Element(t[key]);
				}
			}
			else
			{
				for (auto& element : t)
				{
					auto key = element.first;

					Serialize_Element(key);
					Serialize_Element(element.second);
				}
			}
		};
	};
}
