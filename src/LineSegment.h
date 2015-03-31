#ifndef LINE_SEGMENT_H
#define LINE_SEGMENT_H

#include "Vector.h"
#include "NodeLineSegment.h"

class CLineSegment
{
public:
	CVector a,b;
	CLineSegment(){}
	CLineSegment(CVector _a, CVector _b) : a(_a), b(_b) {}
	CLineSegment(const CNodeLineSegment &_nls) : a(_nls.up->position), b(_nls.down->position) {}
	CVector MiddlePoint() const;
	double Lenght() const;
	void Set(const CNodeLineSegment &_nls) { a=_nls.up->position; b=_nls.down->position; }
	void Set(CVector &_a, CVector &_b){ a=_a; b=_b; }
	void Draw();
};

#endif
