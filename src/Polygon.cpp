#include "Polygon.h"
#include "f_misc.h"
#include "Line.h"

CPolygon::CPolygon(vector<CVector*> *pointsVector, double minAngleDiffToPI)
{
	pointNum = pointsVector->size();
	points = new CVector[pointNum];

	for(int i=0; i < pointNum; i++)
		points[i] = *(*pointsVector)[i];
}

bool CPolygon::PointInside(CVector  &_p)
{
	
	CVector a,b;

	double x,y; 
	x = _p.x;
	y = _p.y;

	int interNum=0;
	CVector intersection(0,y);

	double upper,lower;
	int secondIndx;

	CLine currentLine; 

	for(int i=0; i<pointNum; i++)
	{
		a = points[i];
		secondIndx = i+1;
		
		if(secondIndx==pointNum)
			secondIndx=0;

		b = points[secondIndx];

		if(F::MISC::ValueBetweenValues(a.y,b.y,y))
		{
			currentLine.Set(a,b);
			intersection.x = currentLine.GetX(y);

			if(a.x==b.x)
			{
				if(x > a.x)
				{
					upper = F::MISC::Max(a.y,b.y);
					lower = F::MISC::Min(a.y,b.y);
					if(F::MISC::AddIntersection(upper,lower, y))
						interNum++;

					continue;
				}
			}

			if(a.y==b.y)
			{
				if(F::MISC::ValueBetweenValues(a.x,b.x,x) )
					interNum++;

				continue;
			}

			if(x<intersection.x)
				continue;

			if(F::MISC::ValueBetweenValues(a.x,b.x,intersection.x) )
			{
				upper = F::MISC::Max(a.y,b.y);
				lower = F::MISC::Min(a.y,b.y);
				if(F::MISC::AddIntersection(upper,lower, y))
					interNum++;
			}
		}
	}

	if(interNum%2==0)
		return false;

	return true;
}