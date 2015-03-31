#pragma once
#include "FOVNode.h"
#include "LineSegmentVisibility.h"
using namespace std;

class CFOVNDAT //FieldOfViewNodesDirectAccesTable
{
public:
	enum NCS{NOT_TESTED, NO_WAY, NO, YES}; //nodes connect status
	CFOVNDAT();
	CFOVNode* GetFOVNodeFromNode(CNode* _node);
	CFOVNode* CreateNewFOVNodeFromNode(CNode *_node);
	int GetFOVNodesFromNLS(CNodeLineSegment * _NLS, CFOVNode*** _NLSFOVNArrayPointer);
	int GetFOVNodesFromNLSNumber(CNodeLineSegment * _NLS);

	void SetNewNLSFOVNodes(CNodeLineSegment * _NLS, CFOVNode** _newNLSFOVNArray, int _newNLSFOVNNumber, bool _erasePreviousFOVNs = true);
	void EraseFOVNodesFromNLS(CNodeLineSegment * _NLS);

	void SetNLSToVisited(CNodeLineSegment * _NLS);
	bool NLSVisited(CNodeLineSegment * _NLS);
	bool NLSEmpty(CNodeLineSegment * _NLS);
	int NodeConnectability( CFOVNode* _node1, CFOVNode* _node2 );
	void SetNodesConnectStatus(CFOVNode* _node1, CFOVNode* _node2, int _ncs);

	void InitializeLSV(CFOVNode* _node1, CFOVNode* _node2);
	void EraseLSV(CFOVNode* _node1, CFOVNode* _node2);
	CLineSegmentVisibility* GetNodesLSV(CFOVNode* _node1, CFOVNode* _node2);


	CLineSegmentVisibility ***LSVsBetweenOrigNodes;
	CFOVNode ***NLSFOVNodes;
	CFOVNode **FOVNodes;
	int *NLSFOVNNum;
	int **nodeConnectability;
};
