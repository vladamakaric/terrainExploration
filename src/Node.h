#pragma once
#include <vector>
#include "Vector.h"
#include "BaseEntity.h"



///////////////////////////
#include "PathFindingNode.h"
#include <vector>
using namespace std;
///////////////////////

class CTempNode : public CPathFindingNode
{
public:
	void AddNeighboursToOpenNodesAndCalculatePFParams(vector<CPathFindingNode*> &_openNodes, CVector _goal);
	vector<CPathFindingNode*> neighbours;
};

class CObstacle;
class CNode : public CBaseEntity
{
public:
	CNode(CVector _position) : position(_position), terrainIndx(-1), tagged(0) {tempNode.position = position; }
	int terrainIndx;
	bool tagged;
	CVector position;
	CObstacle* polygon;
	CNode* next;
	CNode* previous;
	CTempNode tempNode;
};