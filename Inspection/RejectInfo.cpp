#include "stdafx.h"
#include "RejectInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//====================================================================================

REJECT_PIECE_INFO::REJECT_PIECE_INFO()
	: m_length(0.f)
	, m_height(0.f)
	, m_width(0.f)
	, m_area(0.f)
	, m_angle(0.f)
	, m_blobRectNorm(IPVM::Rect_32f(0,0,0,0))
{
}

REJECT_PIECE_INFO::REJECT_PIECE_INFO(const IPVM::Rect& rect, float length, float height, float width, float area, LPCTSTR detail, float angle)
	: m_rect(rect)
	, m_blobRectNorm(IPVM::Rect_32f(0, 0, 0, 0))
	, m_length(length)
	, m_height(height)
	, m_width(width)
	, m_area(area)
	, m_angle(angle)
	, m_detail(detail)
{
}

REJECT_PIECE_INFO::REJECT_PIECE_INFO(const IPVM::Rect& rect, const IPVM::Rect_32f& rect32f, float length, float height, float width, float area, LPCTSTR detail, float angle)
	: m_rect(rect)
	, m_blobRectNorm(rect32f)
	, m_length(length)
	, m_height(height)
	, m_width(width)
	, m_area(area)
	, m_angle(angle)
	, m_detail(detail)
{
}

REJECT_PIECE_INFO::~REJECT_PIECE_INFO()
{
}

//====================================================================================

REJECT_INFO::REJECT_INFO()
{
}

REJECT_INFO::~REJECT_INFO()
{
}

void REJECT_INFO::Add(const IPVM::Rect& rect, const float length)
{
	m_objs.emplace_back(rect, length, 0.f, 0.f, 0.f, _T(""));
}

void REJECT_INFO::Add(const IPVM::Rect& rect, const float length, LPCTSTR detail)
{
	m_objs.emplace_back(rect, length, 0.f, 0.f, 0.f, detail);
}

void REJECT_INFO::Add(const IPVM::Rect& rect, const IPVM::Rect_32f& rect32f, const float length, LPCTSTR detail)
{
	m_objs.emplace_back(rect, rect32f, length, 0.f, 0.f, 0.f, detail);
}

void REJECT_INFO::Add(const IPVM::Rect& rect, const float length, const float height, const float width, const float area, LPCTSTR detail)
{
	m_objs.emplace_back(rect, length, height, width, area, detail);
}

void REJECT_INFO::Add(const IPVM::Rect& rect, const IPVM::Rect_32f& rect32f, const float length, const float height, const float width, const float area, LPCTSTR detail)
{
	m_objs.emplace_back(rect, rect32f, length, height, width, area, detail);
}

void REJECT_INFO::Append(const REJECT_INFO& info)
{
	m_objs.insert(m_objs.end(), info.m_objs.begin(), info.m_objs.end());
}

void REJECT_INFO::Clear()
{
	m_objs.clear();
}

long REJECT_INFO::Count() const
{
	return (long)m_objs.size();
}

const REJECT_PIECE_INFO& REJECT_INFO::operator [] (const long index) const
{
	return m_objs[index];
}

REJECT_PIECE_INFO& REJECT_INFO::operator [] (const long index)
{
	return m_objs[index];
}

std::vector<REJECT_PIECE_INFO>::iterator REJECT_INFO::begin()	{ return m_objs.begin();	}
std::vector<REJECT_PIECE_INFO>::iterator REJECT_INFO::end()		{ return m_objs.end();		}

std::vector<REJECT_PIECE_INFO>::const_iterator REJECT_INFO::cbegin() const	{ return m_objs.cbegin(); }
std::vector<REJECT_PIECE_INFO>::const_iterator REJECT_INFO::cend() const	{ return m_objs.cend(); }
