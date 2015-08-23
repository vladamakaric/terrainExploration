#ifndef NLSFOVNDAT_H
#define NLSFOVNDAT_H

#include "FOVNode.h"
#include "FieldOfView.h"
#include <vector>
#include "Path.h"
#include "LineSegmentVisibility.h"

using namespace std;

class CNLSFOVNDAT //node line segment field of view node direct acces table
{
	enum NCS{NOT_TESTED ,NO_WAY, NO, YES};
	int ID;
	int *nodeConnectStatus;
	CFOVNode* NLSnode;
	CFieldOfView *FOV;
	CLineSegmentVisibility **nodeConnectionLSVs;

	int GetNodeConnectability(CFOVNode* _node);
public:
	CNLSFOVNDAT(CFOVNode* _fn, CFieldOfView* _FOV);
	~CNLSFOVNDAT();
	void MutualyAddNeighbours();
	void ClearNeighbours();
	bool ConnectionGood(CFOVNode* _node);
	void UpdateConnections(const vector<CPath*> &paths);
};

#endif
