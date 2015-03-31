#include "f_misc.h"
#include "f_intersection.h"
#include "f_distance.h"
#include "Line.h"

bool F::MISC::ValueBetweenValues(double _v1, double _v2, double _value )
{
	double maxValue = _v1;
	double minValue;

	if(maxValue < _v2)
		maxValue = _v2;

	if(maxValue == _v2)
		minValue = _v1;
	else
		minValue = _v2;

	return _value >= minValue && _value <= maxValue;
}

void F::MISC::GetLeftRightTopBottomFrom2Vecs(const CVector &_a, const CVector &_b, double &right, double &left,  double &top, double &bottom)
{
	if(_a.x < _b.x)
	{
		left = _a.x;
		right = _b.x;
	}
	else
	{
		right = _a.x;
		left = _b.x;
	}

	if(_a.y < _b.y)
	{
		bottom = _a.y;
		top = _b.y;
	}
	else
	{
		top = _a.y;
		bottom = _b.y;
	}
}

CNearestCandidate* F::MISC::GetSmallestValueCandidate(vector<CNearestCandidate> &candidates)
{
	double minValue = 9999999999.9;
	CNearestCandidate* smallestValueCandidate = 0;

	for(int i=0; i < candidates.size(); i++)
	{
		if(candidates[i].value < minValue)
		{
			minValue = candidates[i].value;
			smallestValueCandidate = &candidates[i];			
		}
	}

	return smallestValueCandidate;
}

void F::MISC::GetUniqueNodeLineSegmentsFromFAGTiles(vector<CNodeLineSegment*> &_uNodeLines, vector<CFAGTile*> &_fagTiles)
{
	for(int i=0; i < _fagTiles.size(); i++)
	{
		for(int j=0; j< _fagTiles[i]->nodeLines.size(); j++ )
		{
			if(!_fagTiles[i]->nodeLines[j]->tagged)
			{
				_fagTiles[i]->nodeLines[j]->tagged = true;
				_uNodeLines.push_back(_fagTiles[i]->nodeLines[j]);
			}
		}
	}

	UntagPointerContainer(_uNodeLines);
}

void F::MISC::CalculateCircleFrom3Points(const CVector& _p1, const CVector &_p2,const CVector &_p3, CCircle& _c)
{
	CVector _mp1 = (_p1+_p2)/2; 
	CVector _mp2 = (_p2+_p3)/2;

	CLine a(_p1,_p2);
	CLine b(_p3,_p2);
	a.Perp();
	b.Perp();
	a.ThroughPoint(_mp1);
	b.ThroughPoint(_mp2);

	_c.position = F::INTERSECTION::GetLineIntersection(a,b);
	_c.r = F::DISTANCE::GetDistance(_p1, _c.position);
}

bool F::MISC::AddIntersection(double _upper, double _lower, double _y)
{
	if( _upper - _y < 0.0001)
		return false;

	if(_y - _lower < 0.0001)
		return true;

	return true;
}

bool F::MISC::IsCircleOutOfMap(int _HR, const CVector &_position, int _mHeight, int _mWidth , int _border)
{
	return _position.x + _HR + _border > _mWidth || _position.x - _HR - _border < 0 || _position.y + _HR + _border > _mHeight ||  _position.y - _HR - _border < 0;
}

void F::MISC::GetLeftRightTopBottomFromLineSegment(const CLineSegment &_ls, double &right, double &left,  double &top, double &bottom)
{
	GetLeftRightTopBottomFrom2Vecs(_ls.a,_ls.b, right,left,top,bottom);
}

bool F::MISC::TwoPointsOnSameSideOfLine( const CVector &_p1, const CVector &_p2, const CLine &_l )
{
	if(_l.b)
	{
		double y1 = _l.GetY(_p1.x);
		double y2 = _l.GetY(_p2.x);

		if( (y1>_p1.y && y2>_p2.y) || (y1<_p1.y && y2<_p2.y) )
			return true;
	}
	else
	{
		double x1 = _l.GetX(_p1.y);
		double x2 = _l.GetX(_p2.y);

		if( (x1>_p1.x && x2>_p2.x) || (x1<_p1.x && x2<_p2.x) )
			return true;
	}

	return false;
}

void F::MISC::PathFindingNodePathToVectorPath( vector<CPathFindingNode*> &PFNPath, vector<CVector> &vectorPath )
{
	for(int i=0; i<PFNPath.size(); i++)
		vectorPath.push_back(PFNPath[i]->position);
}
