#pragma once
#include <vector>
#include "Vector.h"
#include "BaseEntity.h"


using namespace std;

class CPathFindingNode : public CBaseEntity 
{
public:
	CPathFindingNode() {CleanPFInfo();}
	CVector position;
	double F,G,H;
	bool closed;
	CPathFindingNode* parent;
	CPathFindingNode* tempNeighbour;
	virtual void GetNeighbours(vector<CPathFindingNode*> &_neighbours){};
	virtual void AddNeighboursToOpenNodesAndCalculatePFParams(vector<CPathFindingNode*> &_openNodes, CVector _goal)=0;
	virtual void ClearNeighbours(){};
	void CleanPFInfo(){ F=0; G=0; H=0; closed = false; parent = 0; tempNeighbour = 0;};
};