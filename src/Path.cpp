#include "Path.h"
#include "f_distance.h"

bool CPath::AddPoint( CVector& _point, bool checkForMinDist)
{
	if(!capsules.size())
	{
		if(!firstPoint)
		{
			firstPoint = new CVector(_point);
			return true;
		}
		else
		{
			if(checkForMinDist)
			if(!F::DISTANCE::DistBetweenVecsBiggerThan(*firstPoint, _point, 200))
				return false;

			capsules.push_back(new CCapsule(*firstPoint, _point, r));
			return true;
		}
	}

	CVector lastAddedPoint =  (*capsules.rbegin())->center.b;

	if(checkForMinDist)
	if(!F::DISTANCE::DistBetweenVecsBiggerThan(lastAddedPoint, _point, 200))
		return false;

	capsules.push_back(new CCapsule(lastAddedPoint, _point, r));
	return true;
}

void CPath::RemoveLastAddedPoint()
{
	if(capsules.size())
	{
		delete capsules.back();
		capsules.pop_back();
	}
	else
	{
		if(firstPoint)
		{
			delete firstPoint;
			firstPoint = 0;
		}
	}
}

void CPath::Draw()
{
	CCapsule c;
	c.r = r;
	for(int i=0; i<points.size()-1; i++)
	{
		c.SetCenter(points[i], points[i+1]);
		c.DrawOutline();
	}
}
