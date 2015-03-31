#pragma once
#include "NodeLineSegment.h"
#include "FOVNode.h"
#include "f_misc.h"
#include "f_vector.h"
#include "Line.h"
#include "simulationDefinitions.h"

namespace F
{
	namespace NODE
	{
		//template<class T> inline void

		template<class T> inline bool PointToNodeConnectionIsEdge(T *node, CVector &_point)
		{
			if(!node->next || !node->previous)
				return true;

			CVector directionOfConnection = F::VECTOR::GetVectorFromAToB(_point, node->position).Direction()/2;
			CVector testPosition = node->position + directionOfConnection;

			return !U_FAG->PointInObstacle(testPosition);
		}

		template<class T> inline bool NodeIsEdgeByConnections(T *node)
		{
			if(GetNodeConnections(node)==2)
				return false;
			return true;
		}

		template<class T> inline int GetNodeConnections(T *node) 
		{
			if(!node->next && !node->previous)
				return 0;

			if(node->next && node->previous)
				return 2;

			return 1;
		}

		template<class T> inline bool NodeConnectionToPointIsEdge(T *node, CVector &_point, bool checkConnections = true) 
		{
			if(checkConnections)
			{
				if(!node->next || !node->previous)
					return true;
			}

			CVector directionOfConnection = F::VECTOR::GetVectorFromAToB(node->position, _point).Direction();
			CVector testPosition = node->position + directionOfConnection;

			CLine tempLine(_point, node->position);
			if(!F::MISC::TwoPointsOnSameSideOfLine(node->next->position, node->previous->position, tempLine))
				return false;

			return !U_FAG->PointInObstacle(testPosition);
		}

		template<class T> inline T* GetAnyConnection(T *node) 
		{
			if(node->next)
				return node->next;

			if(node->previous)
				return node->previous;

			return 0;
		}

		template<class T> inline void EraseConnectionsTo(T *node) 
		{
			node->next=0;
			node->previous = 0;
		}

		template<class T> inline bool NodesConnected(T* a, T* b)
		{
			return a->next == b || b->next == a;
		}
		
		template<class T> inline void EraseConnectionsToAndFrom(T *node) 
		{
			T* left;
			T* right;

			left = node->previous;
			right = node->next;

			if(left)
			{
				if(left->next==node)
					left->next=0;

				if(left->previous==node)
					left->previous=0;
			}

			if(right)
			{
				if(right->next==node)
					right->next=0;
				
				if(right->previous==node)
					right->previous=0;
			}

			node->previous=0;
			node->next=0;
		}

		CNodeLineSegment* GetAdjacentNodeLineByPoint(CNodeLineSegment &_a, CVector &_p);
		CNodeLineSegment* GetNodeLineBetween(CNodeLineSegment &_a, CNodeLineSegment &_b);
		CNode* GetNearestNode(CVector& _refPoint ,vector<CNode*> &_nodes, CNode* _forbiden = 0);
		CNode* GetTaggedNode(vector<CNode*> &viewNodes);

		CNode* GetOtherNodeOnLineSegment(CNode* _node, CNodeLineSegment * _NLS);
		CNode* GetOtherConnectedNode( CNode* _connectedNode, CNode* _node );
		bool ANextIsB(CNode *_a, CNode *_b);

		void MutualyAddFOVNodesAsNeighbours(CFOVNode* _a, CFOVNode *_b);

		//CFOVNode *GetOtherNLSNode
		bool GetSamePointOnTwoNodeLines(CNodeLineSegment& _a, CNodeLineSegment& _b, CVector &_p);
		bool ConnectTwoFOVNodes(CFOVNode* _a, CFOVNode *_b);
		bool FOVNodesConnected(CFOVNode* _a, CFOVNode *_b);
		void GetUniqueNodesFromNLSs(vector<CNode*> &_nodes , vector<CNodeLineSegment*> &_NLS);
		bool NodeOnNLS(CNode* _node, CNodeLineSegment* _NLS);
	}
}