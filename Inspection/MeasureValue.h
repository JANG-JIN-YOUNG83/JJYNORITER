#pragma once

#include "basedef.h"

namespace Inspection
{
	class __INSPECTOIN_CLASS__ MeasureValue
	{
	public:
		MeasureValue(MeasureOffsetType offsetType);
		~MeasureValue();

		double GetValue_um() const;
		double GetValue_mm() const;

		MeasureValue& operator = (const MeasureValue& rhs);

		void Reset();
		void Set_mm_ApplyOffset(bool isUp, double value);
		void Set_um_ApplyOffset(bool isUp, double value);
		void Set_mm(double value);
		void Set_um(double value);

	private:
		const MeasureOffsetType m_offsetType;
		double m_value_um;		// Offset 까지 적용된 최종 측정값
	};
}
