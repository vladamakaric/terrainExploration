#ifndef LINE_H
#define LINE_H

#include <cmath>
#include "Vector.h"
#include "draw.h"
#include "LineSegment.h"
#include "NodeLineSegment.h"
#include "f_misc.h"

class CLine
{
public:
	double a,b,c;
	CLine(){}
	CLine(const CLineSegment &ls)
	{
		Set(ls.b, ls.a);
	}

	CLine(const CNodeLineSegment &nls)
	{
		Set(nls);
	}

	CLine(const CVector &_a,const CVector &_b)
	{
		Set(_a,_b);
	}

	void Set(const CNodeLineSegment &nls)
	{
		Set(nls.down->position, nls.up->position);
	}

	void Set(const CVector &_a, const CVector &_b)
	{
		a = _a.y - _b.y;
		b = _b.x - _a.x;
		c = _a.x*_b.y - _b.x*_a.y;
	}

	//Line through point
	CLine(const CLine &_l, const CVector &_p)
	{
		a = _l.a;
		b = _l.b;
		ThroughPoint(_p);
	}

	void Perp()
	{
		F::MISC::Swap(a,b);
		if(std::abs(a) && std::abs(b)) a=-a;
	}

	//Adjust line to go through point
	void ThroughPoint( const CVector &_p)
	{
		c = -a*_p.x - b*_p.y;
	}
	
	CVector GetIntersectionOnXorYAxis()
	{
		if(b)
			return CVector(0, GetY(0));
		else
			return CVector(GetX(0), 0);
	}
	
	void Translate(const CVector &_t)
	{
		CVector axisIntersect = GetIntersectionOnXorYAxis();
		ThroughPoint(axisIntersect+_t);
	}

	double GetX(double _y) const
	{
		return (-b*_y - c)/a;
	}

	double GetY(double _x) const
	{
		return (-a*_x - c)/b;
	}

	void Draw()
	{
		if(b)
			Draw::Line(CVector(0, GetY(0)), CVector(1000, GetY(1000)));
		else
			Draw::Line(CVector(GetX(0), 0), CVector(GetX(1000), 1000));
	}
};

#endif
