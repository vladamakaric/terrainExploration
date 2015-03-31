#pragma once 
#include <vector>
#include "Vector.h"
#include "PointCluster.h"
#include "BaseEntity.h"
#include "global.h"
#include "Polygon.h"
#include "NodeLineSegment.h"
#include "Circle.h"

using namespace std;

class CObstacle : public CBaseEntity
{
public:
	vector<CVector*> *pointsVector;
	vector<CVector*> *convexHull;
	CPolygon *polygon;
	CNodeLineSegment *nodeLines;
	GLint displayList;
	bool tagged;
	CCircle boundingCircle;
	CNode* GetNearestNode(CVector &_position);
	
	CObstacle() : tagged(0) { pointsVector = new vector<CVector*>(); }
	
	void DrawBody();
	void Draw();
	void InitializeAfterLoadedFromFile();
	void OptimiseObstacle();
	void InitialiseNodeLines();
	bool PointInside(CVector &_point);
	bool PointsConnectableOutsideOfObstacle(int _indx1, int _indx2);
};
