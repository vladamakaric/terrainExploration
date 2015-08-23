#ifndef FOV_NODE_H
#define FOV_NODE_H

#include "Node.h"
#include "Vector.h"
#include "LineSegment.h"
#include <vector>
#include "PathFindingNode.h"
//#include "NLSFOVNDAT.h"

using namespace std;

class CNLSFOVNDAT;
class CFOVNode : public CPathFindingNode
{
public:
	CFOVNode() : origNode(0), origNLS(0) { SetUp(); }
	CFOVNode(CNode* _origN) : origNode(_origN), origNLS(0) {SetUp(); position = _origN->position; }
	CFOVNode(const CVector &_pos, CNodeLineSegment* _origNLS) :  origNLS(_origNLS), origNode(0) {SetUp(); position = _pos;}
	~CFOVNode();
	void SetUp() { CleanPFInfo(); next=0; previous=0; tagged = false; OrigNodeConnectionDAT = 0;}
	void AddNeighboursToOpenNodesAndCalculatePFParams(vector<CPathFindingNode*> &_openNodes, CVector _goal);
	void AddNeighbour(CFOVNode* _neighbour);
	void ClearNeighbours();
	void ClearOrigNeighboursTempNeighbour();

	bool tagged;
	CNode* origNode;
	CNodeLineSegment * origNLS;
	CFOVNode *next,*previous;

	CNLSFOVNDAT* OrigNodeConnectionDAT;
	vector<CFOVNode*> NLSNeighbours;
	vector<CFOVNode*> origNeighbours;

	////////////////////////////////////////////////
	void DrawConnections();

};

#endif
