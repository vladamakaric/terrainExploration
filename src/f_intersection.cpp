#include "f_intersection.h"
#include "f_distance.h"
#include "f_misc.h"
#include "f_vector.h"

#include "Line.h"

bool F::INTERSECTION::PointOnLineOnLineSegment( const CVector &_p, const CLineSegment &_ls )
{
	if(_ls.a.x != _ls.b.x)
		return F::MISC::ValueBetweenValues(_ls.a.x, _ls.b.x, _p.x);
	else 
		return F::MISC::ValueBetweenValues(_ls.a.y, _ls.b.y, _p.y);
}

bool F::INTERSECTION::LineSegmentIntersectsCircle( const CLineSegment &_ls, const CCircle &_c )
{
	return F::DISTANCE::GetLineSegmentPointDistanceSq(_ls, _c.position) < _c.r * _c.r;
}

bool F::INTERSECTION::LineIntersectsCircle( const CLine &_l, const CCircle &_c )
{
	return F::DISTANCE::GetLinePointDistance( _l, _c.position ) <= _c.r;
}

CVector F::INTERSECTION::GetLineIntersection( const CLine &_a, const CLine &_b )
{
	return CVector( (_b.b*_a.c - _a.b * _b.c )/( _a.b*_b.a - _b.b*_a.a),(_b.a*_a.c - _a.a*_b.c)/(_a.a*_b.b - _b.a*_a.b));
}

int F::INTERSECTION::GetLineCircleIntersection( const CLine &_l, const CCircle &_c, CVector *_interPoints , bool checkForIntersection)
{
	if(checkForIntersection)
	if(!LineIntersectsCircle(_l, _c))
		return 0;

	CLine temp = _l;
	temp.Translate(-_c.position);

	double a = temp.a;
	double b = temp.b;
	double c = temp.c;
	double r = _c.r;

	double x1,x2,y1,y2;

	x1 = (-(a*c) + sqrt(b*b*(-c*c + a*a*r*r + b*b*r*r)))/(a*a + b*b);
	x2 = (-(a*c) - sqrt(b*b*(-c*c + a*a*r*r + b*b*r*r)))/(a*a + b*b);

	if(b)
	{
		y1 = temp.GetY(x1);
		y2 = temp.GetY(x2);
	}
	else
	{
		y1 = sqrt(r*r - x1*x1);
		y2 = -y1;
	}

	_interPoints[0] = CVector(x1,y1) + _c.position;
	_interPoints[1] = CVector(x2,y2) + _c.position;

	if(_interPoints[0] == _interPoints[1])
		return 1;

	return 2;
}

int F::INTERSECTION::GetLineSegmentCircleIntersection( const CLineSegment &_ls, const CCircle &_c, CVector *_interPoints, bool checkForIntersection /*= true*/ )
{
	if(checkForIntersection)
		if(!LineSegmentIntersectsCircle(_ls,_c))
			return 0;

	int interNum = GetLineCircleIntersection(CLine(_ls), _c, _interPoints, false);
	if(!interNum)
		return 0;

	if(interNum == 1)
	{
		if(PointOnLineOnLineSegment(_interPoints[0], _ls))
			return 1;
		return 0;
	}

	if(interNum == 2)
	{
		int i=0;
		if(PointOnLineOnLineSegment(_interPoints[0], _ls))
			i++;

		if(PointOnLineOnLineSegment(_interPoints[1], _ls))
		{
			_interPoints[i]=_interPoints[1];
			i++;
		}

		return i;
	}
}

bool F::INTERSECTION::PointProjectionOnLineSegment( const CVector &_p, const CLineSegment &_ls )
{
	CVector lineSegVec = F::VECTOR::GetVectorFromAToB(_ls.a,_ls.b);
	double lsvLenghSq = lineSegVec.LengthSq();
	CVector a = F::VECTOR::GetVectorFromAToB(_ls.a, _p);
	double projA = F::VECTOR::GetLenghtSqOfVectorProjectionAToB(a,lineSegVec);

	if(projA > lsvLenghSq)
		return false;

	CVector b = F::VECTOR::GetVectorFromAToB(_ls.b, _p);
	double projB = F::VECTOR::GetLenghtSqOfVectorProjectionAToB(b,lineSegVec);

	if(projB > lsvLenghSq)
		return false;

	return true;
}

bool F::INTERSECTION::LineSegmentIntersection(const CLineSegment &_ls1,const CLineSegment &_ls2, CVector *_ip /*= 0*/ )
{
	float s1_x, s1_y, s2_x, s2_y;
	s1_x = _ls1.b.x - _ls1.a.x;     s1_y = _ls1.b.y - _ls1.a.y;
	s2_x = _ls2.b.x - _ls2.a.x;     s2_y = _ls2.b.y - _ls2.a.y;

	float s, t;
	s = (-s1_y * (_ls1.a.x -  _ls2.a.x) + s1_x * (_ls1.a.y - _ls2.a.y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = ( s2_x * (_ls1.a.y -  _ls2.a.y) - s2_y * (_ls1.a.x  -  _ls2.a.x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		if(!_ip) return 1;

		_ip->x = _ls1.a.x + (t * s1_x);
		_ip->y = _ls1.a.y + (t * s1_y);
		return 1;
	}

	return 0;
}

bool F::INTERSECTION::PointInSquare(const CVector &_p,const CSquare &_s)
{
	if(_p.x >= _s.downLeft.x && _p.x <= _s.upRight.x && _p.y >= _s.downLeft.y && _p.y <= _s.upRight.y)
		return true;
	return false;
}

bool F::INTERSECTION::NodeLineFAGTileIntersection(CNodeLineSegment &_nl, CFAGTile &_ftile)
{
	CVector bottomLeft( _ftile.colNum * _ftile.parentFAG->tileWidth , _ftile.rowNum *_ftile.parentFAG->tileHeight);
	CVector topRight = bottomLeft;

	topRight.x += _ftile.parentFAG->tileWidth;
	topRight.y += _ftile.parentFAG->tileHeight;
	CSquare ftileSquare(bottomLeft, topRight);

	if(PointInSquare(_nl.up->position , ftileSquare) || PointInSquare(_nl.up->position , ftileSquare) )
		return true;

	CLine nLine(_nl.down->position, _nl.up->position); 
	double xBetween, yBetween;

	xBetween = nLine.GetX(bottomLeft.y);

	if(xBetween >= _nl.left->position.x && xBetween <= _nl.right->position.x)
	{
		if(xBetween >= bottomLeft.x && xBetween <= topRight.x)
			return true;
	}

	yBetween = nLine.GetY(bottomLeft.x);

	if(yBetween >= _nl.down->position.y && yBetween <= _nl.up->position.y)
	{
		if(yBetween >= bottomLeft.y && yBetween <= topRight.y)
			return true;
	}

	xBetween = nLine.GetX(topRight.y);

	if(xBetween >= _nl.left->position.x && xBetween <= _nl.right->position.x)
	{
		if(xBetween >= bottomLeft.x && xBetween <= topRight.x)
			return true;
	}

	yBetween = nLine.GetY(topRight.x);

	if(yBetween >= _nl.down->position.y && yBetween <= _nl.up->position.y)
	{
		if(yBetween >= bottomLeft.y && yBetween <= topRight.y)
			return true;
	}

	return false;
}

bool F::INTERSECTION::PointInCircle(const CVector &_p, const CCircle &_c)
{
	return !F::DISTANCE::DistBetweenVecsBiggerThan(_p,_c.position, _c.r);
}

bool F::INTERSECTION::CircleOutOfSquare(const CCircle &_c,const CSquare &_s)
{
	if(PointInSquare(_c.position, _s))
		return false;

	if(_c.position.x - _c.r > _s.upRight.x)
		return true;

	if(_s.downLeft.x > _c.position.x + _c.r)
		return true;

	if(_c.position.y - _c.r > _s.upRight.y)
		return true;

	if(_s.downLeft.y > _c.position.y + _c.r)
		return true;

	CVector upLeft(_s.downLeft.x, _s.upRight.y);
	CVector	downRight(_s.upRight.x, _s.downLeft.y);
	
	return !(PointInCircle(upLeft,_c) || PointInCircle(_s.upRight,_c) || PointInCircle(downRight,_c) || PointInCircle(_s.downLeft,_c));
}

bool F::INTERSECTION::CircleInSquare(const CCircle &_c,const CSquare &_s)
{
	if(!PointInSquare(_c.position, _s))
		return false;

	return (_s.downLeft.x < _c.position.x - _c.r && _s.upRight.x > _c.position.x + _c.r) && //horiz fit
		   (_s.downLeft.y < _c.position.y - _c.r && _s.upRight.y > _c.position.y + _c.r);   //vert fit
}

bool F::INTERSECTION::CircleIntersectsCircle(const CCircle &_c1, const CCircle &_c2)
{
	return !F::DISTANCE::DistBetweenVecsBiggerThan(_c1.position, _c2.position, _c1.r + _c2.r);
}

int F::INTERSECTION::GetLineSegmentCapsuleIntersection( const CLineSegment &_ls, const CCapsule &_c, CVector *_interPoints )
{
	CCircle capsuleCircles[2];
	capsuleCircles[0].Set(_c.center.a, _c.r);
	capsuleCircles[1].Set(_c.center.b, _c.r); 

	CVector refPoint(_ls.a);
	CVector max, min;
	double maxDist = 0;
	double minDist = 999999999999999;
	double dist;

	CVector circleIntersections[2];
	CVector sideLSIntersection;
	int circleInterNum;

	bool foundIntersection = false;

	for(int i=0; i<2; i++)
	{
		circleInterNum = GetLineSegmentCircleIntersection(_ls, capsuleCircles[i], circleIntersections, true);

		for(int j=0; j<circleInterNum; j++)
		{
			CCircle *otherCircle= &capsuleCircles[0];
			if(i==0) otherCircle = &capsuleCircles[1];

			if(PointProjectionOnLineSegment(circleIntersections[j], _c.center) || PointInCircle(circleIntersections[j], *otherCircle))
				continue;

			foundIntersection = true;
			dist = DISTANCE::GetDistanceSq(refPoint, circleIntersections[j]);

			if(dist > maxDist)
			{
				max = circleIntersections[j];
				maxDist = dist;
			}

			if(dist < minDist )
			{
				min = circleIntersections[j];
				minDist = dist;
			}
		}

		if(LineSegmentIntersection(_ls, _c.sides[i], &sideLSIntersection))
		{
			foundIntersection = true;
			dist = DISTANCE::GetDistanceSq(refPoint, sideLSIntersection);

			if(dist > maxDist)
			{
				max = sideLSIntersection;
				maxDist = dist;
			}

			if(dist < minDist )
			{
				min = sideLSIntersection;
				minDist = dist;
			}
		}
	}

	if(foundIntersection)
	{
		if(max == min)
		{
			_interPoints[0] = max;
			return 1;
		}

		_interPoints[0] = max;
		_interPoints[1] = min;

		return 2;
	}

	return 0;
}

bool F::INTERSECTION::PointInCapsule( const CVector &_p, const CCapsule &_c )
{
	return DISTANCE::GetLineSegmentPointDistanceSq(_c.center, _p) <= _c.r * _c.r;
}

bool F::INTERSECTION::LineIntersectsCapsule( const CLine &_l, const CCapsule &_c )
{
	CCircle capsuleCircle(_c.center.a, _c.r);
	if(LineIntersectsCircle( _l ,capsuleCircle))
		return true;

	capsuleCircle.position = _c.center.b;

	if(LineIntersectsCircle( _l ,capsuleCircle))
		return true;

	if(F::MISC::TwoPointsOnSameSideOfLine(_c.center.a, _c.center.b, _l))
		return true;

	return false;
}

bool F::INTERSECTION::PointInCircleWithRadiusOffset( const CVector &_p, const CCircle &_c, double _offset )
{
	CCircle temp = _c;
	temp.r += _offset;
	return PointInCircle(_p, temp);
}
