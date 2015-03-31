#include "Obstacle.h"
#include "global.h"
#include "draw.h"
#include "ConcaveHullGenerator.h"
#include "Tesselator.h"
#include "f_distance.h"
#include "f_angle.h"
#include "f_vector.h"
#include "SmallestCircleGenerator.h"
#include "PrecompiledShapes.h"

void CObstacle::OptimiseObstacle()
{
	CVector a,b;
	if(pointsVector->size()>3)
	for(int i=1; i < pointsVector->size(); i++)
	{
		int lowerIndx = i-1;
		if(i==0) lowerIndx = pointsVector->size()-1;

		a = F::VECTOR::GetVectorFromAToB( *(*pointsVector)[i], *(*pointsVector)[lowerIndx]);
		b = F::VECTOR::GetVectorFromAToB( *(*pointsVector)[i], *(*pointsVector)[(i+1) % pointsVector->size()]);

		double angle = F::ANGLE::GetSmallerAngleBetweenTwoVectors(a,b);

		if(angle > PI-0.3)
		{
			pointsVector->erase(pointsVector->begin()+i);
			i--;
		}
	}

	convexHull = new vector<CVector*>();
	CConcaveHullGenerator* CHG = new CConcaveHullGenerator();
	CHG->Generate(0, convexHull, pointsVector->size(),pointsVector );
	delete CHG;

	polygon = new CPolygon(pointsVector,PI/10); 
	CSmallestCircleGenerator::Generate(convexHull, boundingCircle);
	displayList = CTesselator::Tesselate(polygon);
	InitialiseNodeLines();
}

void CObstacle::InitialiseNodeLines()
{
	int lineNum = polygon->pointNum;
	nodeLines = new CNodeLineSegment[lineNum];

	CNode* a = new CNode(polygon->points[0]);
	CNode* b; 
	int bIndex;

	for(int i=0; i < lineNum; i++)
	{
		nodeLines[i].parentID = ID;

		bIndex = i+1;
		if(i>lineNum-2)
			bIndex = 0;

		if(bIndex)
			b = new CNode(polygon->points[i+1]);
		else
			b = nodeLines[0].down;
			
		if(polygon->points[i].y < polygon->points[bIndex].y)
		{
			nodeLines[i].up = b;
			nodeLines[i].down = a;
		}
		else
		{
			nodeLines[i].up = a;
			nodeLines[i].down = b;
		}

		if(polygon->points[i].x < polygon->points[bIndex].x)
		{
			nodeLines[i].right = b;
			nodeLines[i].left = a;
		}
		else
		{
			nodeLines[i].right = a;
			nodeLines[i].left = b;
		}

		CVector normal = nodeLines[i].right->position - nodeLines[i].left->position;
		normal = normal.perp();
		normal.Normalise();

		CVector tempPos = (nodeLines[i].right->position+nodeLines[i].left->position)/2 + normal;
		if(polygon->PointInside(tempPos))
			normal *= -1;

		nodeLines[i].normalOut = normal;
		a->next = b;
		b->previous = a;
		a = b;
	}

	for(int i=0; i< lineNum; i++)
	{
		if(!i)
		{
			nodeLines[i].previous = &nodeLines[lineNum-1];
			nodeLines[i].next = &nodeLines[1];
		}
		else if(i==lineNum-1)
		{
			nodeLines[i].previous = &nodeLines[lineNum-2];
			nodeLines[i].next = &nodeLines[0];
		}
		else
		{
			nodeLines[i].previous = &nodeLines[i-1];
			nodeLines[i].next = &nodeLines[i+1];
		}
	}
}

CNode* CObstacle::GetNearestNode(CVector &_position)
{
	CNode* startNode = nodeLines[0].down;
	CNode* currentNode;
	CNode* nearestNode;
	double minDist = 99999999999999;
	double dist;

	do
	{
		dist = F::DISTANCE::GetDistanceSq(_position, currentNode->position);

		if(minDist > dist)
		{
			minDist = dist;
			nearestNode = currentNode;
		}

		currentNode = currentNode->next; 
	}while(startNode != currentNode->next);

	return nearestNode;
}

void CObstacle::InitializeAfterLoadedFromFile()
{
	polygon = new CPolygon(pointsVector,PI/10); 
	InitialiseNodeLines();
	displayList = CTesselator::Tesselate(polygon);
}

bool CObstacle::PointInside( CVector &_point )
{
	return polygon->PointInside(_point);
}

void CObstacle::DrawBody()
{
	glCallList(displayList);
}


void CObstacle::Draw()
{
	glColor3d(0.0/255, 67.0/255, 88.0/255);
	DrawBody();

	glColor4f(1,0,0,0.3);
	for(int i=0; i < polygon->pointNum; i++)
		CPCShapes::DrawShape(CPCShapes::triangle50, polygon->points[i]);
}