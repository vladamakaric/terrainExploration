#pragma once
#include "Node.h"
#include "BaseEntity.h"

class CObstacle;
class CNodeLineSegment : public CBaseEntity
{
public:
	CNodeLineSegment() : tagged(0), terrainIndx(-1) {}
	int terrainIndx;


	double Lenght() const;
	CNodeLineSegment *next;
	CNodeLineSegment *previous;
	int parentID;
	bool tagged;
	CObstacle* polygon;

	CNode *up;
	CNode *down;
	CNode *right;
	CNode *left;
	CVector normalOut;
};