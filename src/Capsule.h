#ifndef CAPSULE_H
#define CAPSULE_H

#include "LineSegment.h"
#include "Vector.h"

class CCapsule
{
public:
	CCapsule(){tagged = 0;}
	CCapsule(CLineSegment &_ls, double _r) : center(_ls),  r(_r) {UpdateSides();tagged = 0;}
	CCapsule(CVector &_a, CVector &_b, double _r) : center(_a,_b), r(_r) {UpdateSides();tagged = 0;}
	void SetCenter(CVector &_a, CVector &_b) { center.a = _a; center.b = _b; UpdateSides();tagged = 0; }
	void SetCenter(CLineSegment &_ls) { center = _ls; UpdateSides(); }
	void UpdateSides();
	void DrawOutline();
	void DrawFill();
	CLineSegment center;
	CLineSegment sides[2];
	double r;
	bool tagged;
};
#endif
