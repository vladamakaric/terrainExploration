#pragma once
#include "Vector.h"
#include "FOVNode.h"
#include "FieldOfView.h"
#include <vector>

using namespace std;

class CUnseenPositionNodeManager
{
	enum NCS{NO, HARD_NO, YES, HARD_YES};
	int GetNodeConnectivityStatus(CFOVNode* _fnode);
	void SetNodeConnectivityStatus(CFOVNode* _fnode, int _ncs);
	void TestNodeConnectivity(CFOVNode* _fnode);

	CFieldOfView* FOV;
	CFOVNode *USPN;
	int *nodeConnectivityStatus;
	
public:
	vector<CFOVNode*> connectedToUSPNNodes;
	CUnseenPositionNodeManager(){};
	void Initialize(CFieldOfView* _FOV);
	void InitializeNewUSPN(CFOVNode* _USPN);
	void UpdateConnectivity();
	void ConnectNodesToUSPNode();
	void ConnectNodeToUSPNode(CFOVNode* _fnode, bool allBetsAreOff = false, CObstacle *_obst =0);

	void DisconectNodesFromUSPNode();
	bool OirgNodeConnectionGood(CFOVNode* _fnode);
	bool NLSNodeConnectionGood(CFOVNode* _fnode);
	bool UniversalNodeConnectionGood(CFOVNode* _fnode);
};