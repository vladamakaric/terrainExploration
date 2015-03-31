#include "f_node.h"
#include "f_distance.h"
#include "f_vector.h"
#include "simulationDefinitions.h"

CNode* F::NODE::GetTaggedNode(vector<CNode*> &_viewNodes)
{
	for(int i=0; i < _viewNodes.size(); i++)
	{
		if(_viewNodes[i]->tagged)
			return _viewNodes[i];
	}

	return 0;
}

CNode* F::NODE::GetNearestNode(CVector& _refPoint ,vector<CNode*> &_nodes, CNode* _forbiden)
{
	double minDist = 99999999999999;
	double dist;
	CNode* minNode = 0;
	for(int i=0; i<_nodes.size(); i++)
	{
		if(_forbiden == _nodes[i])
			continue;

		dist = F::DISTANCE::GetDistanceSq(_refPoint, _nodes[i]->position);
		if(minDist > dist)
		{
			minDist = dist;
			minNode = _nodes[i];
		}
	}

	return minNode;
}

bool F::NODE::GetSamePointOnTwoNodeLines(CNodeLineSegment& _a, CNodeLineSegment& _b, CVector &_p)
{
	if(_a.down->ID == _b.up->ID)
	{
		_p = _a.down->position;
		return true;
	}

	if(_a.up->ID == _b.down->ID)
	{
		_p = _a.up->position;
		return true;
	}

	if(_a.up->ID == _b.up->ID)
	{
		_p = _a.up->position;
		return true;
	}

	if(_a.down->ID == _b.down->ID)
	{
		_p = _a.down->position;
		return true;
	}

	return false;
}

CNodeLineSegment* F::NODE::GetAdjacentNodeLineByPoint(CNodeLineSegment &_a, CVector &_p)
{
	if(_a.next->down->position == _p || _a.next->up->position == _p)
		return _a.next;

	if(_a.previous->down->position == _p || _a.previous->up->position == _p)
		return _a.previous;

	return 0;
}

CNodeLineSegment* F::NODE::GetNodeLineBetween(CNodeLineSegment &_a, CNodeLineSegment &_b)
{
	if(_a.next->ID == _b.previous->ID )
		return _b.previous;

	if(_a.previous->ID == _b.next->ID )
		return _b.next;

	return 0;
}

bool F::NODE::FOVNodesConnected(CFOVNode* _a, CFOVNode *_b)
{
	return _a->next == _b || _a->previous == _b;
}

bool F::NODE::ConnectTwoFOVNodes( CFOVNode* _a, CFOVNode *_b )
{
	if(_a->next == _b || _a->previous == _b)
		return true;

	if(_a->origNode->next == _b->origNode)
	{
		_a->next = _b;
		_b->previous = _a;
		return true;
	}
	
	if(_b->origNode->next == _a->origNode)
	{
		_a->previous = _b;
		_b->next = _a;
		return true;
	}

	return false;
}

void F::NODE::GetUniqueNodesFromNLSs( vector<CNode*> &_nodes , vector<CNodeLineSegment*> &_NLS )
{
	for(int i=0; i<_NLS.size(); i++)
	{
		if(!_NLS[i]->down->tagged)
		{
			_nodes.push_back(_NLS[i]->down);
			_NLS[i]->down->tagged = true;
		}

		if(!_NLS[i]->up->tagged)
		{
			_nodes.push_back(_NLS[i]->up);
			_NLS[i]->up->tagged = true;
		}
	}

	F::MISC::UntagPointerContainer(_nodes);
}

CNode* F::NODE::GetOtherNodeOnLineSegment( CNode* _node, CNodeLineSegment * _NLS )
{
	if(_node == _NLS->down)
		return _NLS->up;
	else
		return _NLS->down;
}

CNode* F::NODE::GetOtherConnectedNode( CNode* _connectedNode, CNode* _node )
{
	if(_node->next == _connectedNode)
		return _node->previous;
	else
		return _node->next;
}

bool F::NODE::ANextIsB( CNode *_a, CNode *_b )
{
	return _a->next == _b;
}

void F::NODE::MutualyAddFOVNodesAsNeighbours(CFOVNode* _a, CFOVNode *_b)
{
	_a->AddNeighbour(_b);
	_b->AddNeighbour(_a);
}

bool F::NODE::NodeOnNLS( CNode* _node, CNodeLineSegment* _NLS )
{
	return _NLS->down == _node || _NLS->up == _node;
}
