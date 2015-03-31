#include "PointCluster.h"
#include "f_distance.h"
#include "draw.h"

int CPointCluster::globalID=0;

CPointCluster::CPointCluster(const CVector &_position,int _HRadius, int _pointNum, int _minDist)
{
	ID = globalID++;
	position = _position;
	hRadius = _HRadius;
	next = 0;
	previous = 0;

	points = new vector<CVector*>();

	int xSign,ySign;
	while(_pointNum)
	{
		xSign=1;
		ySign=1;

		if(rand()%10>5)
			xSign=-1;

		if(rand()%10<5)
			ySign=-1;

		CVector *randVec = new CVector((rand()%(_HRadius))*xSign, (rand()%(_HRadius))*ySign);

		if(randVec->Length()>=_HRadius)
		{
			delete randVec;
			continue;
		}

		*randVec += position;

		if(!points->size())
		{
			points->push_back(randVec);	
			continue;
		}

		if(IsMinDistance(*randVec, _minDist, points->size()))
			points->push_back(randVec);	
		else
			delete randVec;

		_pointNum--;
	}	
}

bool CPointCluster::IsMinDistance(const CVector &v, int _minDist, int _vertNum)
{
	for(int i=0; i < _vertNum; i++)
	{
		if(F::DISTANCE::GetDistance(v, *(*points)[i]) < _minDist)
			return false;
	}

	return true;
}

void CPointCluster::Draw()
{
	for(unsigned int i=0; i < points->size(); i++)
	{
		glBegin(GL_POINTS);
		Draw::PushVertex( *(*points)[i]);
		glEnd();
	}

	glColor3f(1,1,0);
	Draw::CircleOutline(position,hRadius,hRadius/1.5);

	if(next) next->Draw();
}

bool CPointCluster::IsInvalid()
{
	if(points->size()<3)
		return true;
	return false;
}