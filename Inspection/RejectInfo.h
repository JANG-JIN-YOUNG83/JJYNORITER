#pragma once

#include "basedef.h"
#include "Types/Rect.h"
#include "Types/Rect_32f.h"

#include <vector>

class __INSPECTOIN_CLASS__ REJECT_PIECE_INFO
{
public:
	REJECT_PIECE_INFO();
	REJECT_PIECE_INFO(const IPVM::Rect& rect, float length, float height, float width, float area, LPCTSTR detail, float angle = 0.f);
	REJECT_PIECE_INFO(const IPVM::Rect& rect, const IPVM::Rect_32f& rect32f, float length, float height, float width, float area, LPCTSTR detail, float angle = 0.f);
	~REJECT_PIECE_INFO();

	IPVM::Rect m_rect;
	IPVM::Rect_32f m_blobRectNorm;
	float m_length;
	float m_height;
	float m_width;
	float m_area;
	float m_angle;

	CString m_detail;
};

class __INSPECTOIN_CLASS__ REJECT_INFO
{
public:
	REJECT_INFO();
	~REJECT_INFO();

	void Add(const IPVM::Rect& rect, const float length);
	void Add(const IPVM::Rect& rect, const float length, LPCTSTR detail);
	void Add(const IPVM::Rect& rect, const IPVM::Rect_32f& rect32f, const float length, LPCTSTR detail);
	void Add(const IPVM::Rect& rect, const float length, const float height, const float width, const float area, LPCTSTR detail);
	void Add(const IPVM::Rect& rect, const IPVM::Rect_32f& rect32f, const float length, const float height, const float width, const float area, LPCTSTR detail);
	void Append(const REJECT_INFO& info);
	void Clear();
	long Count() const;

	const REJECT_PIECE_INFO& operator [] (const long index) const;
	REJECT_PIECE_INFO& operator [] (const long index);

	std::vector<REJECT_PIECE_INFO>::iterator begin();
	std::vector<REJECT_PIECE_INFO>::iterator end();
	std::vector<REJECT_PIECE_INFO>::const_iterator cbegin() const;
	std::vector<REJECT_PIECE_INFO>::const_iterator cend() const;

private:
	std::vector<REJECT_PIECE_INFO> m_objs;
};
