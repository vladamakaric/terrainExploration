#include "UnseenPositionNodeManager.h"
#include "simulationDefinitions.h"
#include "LineSegment.h"
#include "f_node.h"

void CUnseenPositionNodeManager::InitializeNewUSPN(CFOVNode* _USPN)
{
	USPN = _USPN;
	CLineSegment ls;

	for(int i=0; i< U_TERRAIN->nodeNum; i++)
	{
		if(F::NODE::NodeConnectionToPointIsEdge(U_TERRAIN->nodes[i], USPN->position, false))
		{
			nodeConnectivityStatus[i] = NCS::YES;

			ls.Set(U_TERRAIN->nodes[i]->position, USPN->position);
			if(!U_FAG->LineSegmentCollisionWithObstacles_ForbiddenNodes(ls, U_TERRAIN->nodes[i]))
				nodeConnectivityStatus[i] = NCS::HARD_YES;
		}	
		else 
			nodeConnectivityStatus[i] = NCS::NO;
	}

	for(int i=0; i<FOV->origFOVNodes.size(); i++)
		TestNodeConnectivity(FOV->origFOVNodes[i]);
}

void CUnseenPositionNodeManager::UpdateConnectivity()
{
	if(!FOV->altered)
		return;

	for(int i=0; i<FOV->origFOVNodes.size(); i++)
		TestNodeConnectivity(FOV->origFOVNodes[i]);
}

int CUnseenPositionNodeManager::GetNodeConnectivityStatus( CFOVNode* _fnode )
{
	return nodeConnectivityStatus[_fnode->origNode->terrainIndx];
}

void CUnseenPositionNodeManager::SetNodeConnectivityStatus( CFOVNode* _fnode, int _ncs )
{
	nodeConnectivityStatus[_fnode->origNode->terrainIndx] = _ncs;
}

void CUnseenPositionNodeManager::TestNodeConnectivity( CFOVNode* _fnode )
{
	if(GetNodeConnectivityStatus(_fnode) == NCS::YES || GetNodeConnectivityStatus(_fnode) == NCS::NO)
	{
		if(F::NODE::GetNodeConnections(_fnode)==2)
		{
			if(GetNodeConnectivityStatus(_fnode) == NCS::NO)
				SetNodeConnectivityStatus(_fnode, NCS::HARD_NO);
			else
				if(FOV->LineSegmentIntersectsVisibleObstacles(CLineSegment(USPN->position, _fnode->position), _fnode))
					SetNodeConnectivityStatus(_fnode, NCS::HARD_NO);
		}
		else
		{
			if(FOV->LineSegmentIntersectsVisibleObstacles(CLineSegment(USPN->position, _fnode->position), _fnode,false))
				SetNodeConnectivityStatus(_fnode, NCS::HARD_NO);
			
		}
	}
}

bool CUnseenPositionNodeManager::OirgNodeConnectionGood( CFOVNode* _fnode )
{
	int ncs = GetNodeConnectivityStatus(_fnode);

	if(ncs == NCS::YES || ncs == NCS::HARD_YES)
		return true;

	if(ncs == NCS::NO)
		return F::NODE::GetNodeConnections(_fnode) != 2;

	return false;
}

bool CUnseenPositionNodeManager::NLSNodeConnectionGood( CFOVNode* _fnode )
{
	return !FOV->LineSegmentIntersectsVisibleObstacles(CLineSegment(USPN->position, _fnode->position), _fnode,false);
}

void CUnseenPositionNodeManager::ConnectNodesToUSPNode()
{
	int origFOVNodesSize = FOV->origFOVNodes.size();
	int allFOVNodesSize = origFOVNodesSize + FOV->edgdeFOVNodes.size();
	CFOVNode* current;

	for(int i=0; i<allFOVNodesSize; i++)
	{
		if(i<origFOVNodesSize)
			current = FOV->origFOVNodes[i];
		else
		{
			current = FOV->edgdeFOVNodes[i-origFOVNodesSize];

			if(current->origNode)
				continue;
		}

		if(UniversalNodeConnectionGood(current))
		{
			current->tempNeighbour = USPN;
			connectedToUSPNNodes.push_back(current);
		}
	}
}

bool CUnseenPositionNodeManager::UniversalNodeConnectionGood( CFOVNode* _fnode )
{
	if(_fnode->origNLS)
		return NLSNodeConnectionGood(_fnode);
	
	return OirgNodeConnectionGood(_fnode);
}

void CUnseenPositionNodeManager::DisconectNodesFromUSPNode()
{
	for(int i=0; i<connectedToUSPNNodes.size(); i++)
		connectedToUSPNNodes[i]->tempNeighbour = 0;

	connectedToUSPNNodes.clear();
}

void CUnseenPositionNodeManager::Initialize( CFieldOfView* _FOV )
{
	FOV = _FOV;
	nodeConnectivityStatus = new int[U_TERRAIN->nodeNum];
}

void CUnseenPositionNodeManager::ConnectNodeToUSPNode( CFOVNode* _fnode, bool allBetsAreOff ,CObstacle *_obst)
{
	if(!allBetsAreOff)
	if(UniversalNodeConnectionGood(_fnode))
	{
		_fnode->tempNeighbour = USPN;
		connectedToUSPNNodes.push_back(_fnode);
		return;
	}

	if(allBetsAreOff)
	{
		if(!_obst)
		if(UniversalNodeConnectionGood(_fnode))
		{
			_fnode->tempNeighbour = USPN;
			connectedToUSPNNodes.push_back(_fnode);
			return;
		}

		if(!_obst)
			return;

		if(_fnode->origNode)
		{
			if((!_fnode->next || !_fnode->previous) && (_fnode->origNode->polygon == _obst) )
			{
				_fnode->tempNeighbour = USPN;
				connectedToUSPNNodes.push_back(_fnode);
				return;
			}
		}

	/*	if(F::NODE::NodeConnectionToPointIsEdge(_fnode->origNode, USPN->position, true))
		{
			_fnode->tempNeighbour = USPN;
			connectedToUSPNNodes.push_back(_fnode);
			return;
		}*/

		if(_fnode->origNLS)
		if(_fnode->origNLS->polygon == _obst)
		{
			_fnode->tempNeighbour = USPN;
			connectedToUSPNNodes.push_back(_fnode);
		}
	}
}
