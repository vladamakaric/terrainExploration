#include "NLSFOVNDAT.h"
#include "simulationDefinitions.h"
#include "LineSegment.h"
#include "LineSegmentVisibility.h"
#include "f_node.h"

CNLSFOVNDAT::CNLSFOVNDAT(CFOVNode* _fn, CFieldOfView* _FOV ) :
NLSnode(_fn),
FOV(_FOV)
{
	nodeConnectStatus = new int[U_TERRAIN->nodeNum];
	memset(nodeConnectStatus, 0, sizeof(int)* U_TERRAIN->nodeNum);

	nodeConnectionLSVs = new CLineSegmentVisibility*[U_TERRAIN->nodeNum];
	memset(nodeConnectionLSVs, 0, sizeof(CLineSegmentVisibility*) * U_TERRAIN->nodeNum);
}

void CNLSFOVNDAT::UpdateConnections( const vector<CPath*> &paths )
{
	CFOVNode* origFN;
	CLineSegment ls;
	int currentNCS;
	int currentTerrainIndx;
	for(int i=0; i < FOV->origFOVNodes.size(); i++ )
	{
		origFN = FOV->origFOVNodes[i];
		currentNCS = GetNodeConnectability(origFN);
		
		if(currentNCS == NCS::NO_WAY || currentNCS == NCS::YES)
			continue;

		currentTerrainIndx = origFN->origNode->terrainIndx;

		ls.Set(origFN->position, NLSnode->position);

		if(currentNCS == NCS::NOT_TESTED)
		{
			if(F::NODE::NodesConnected(origFN, NLSnode))
			{
				nodeConnectStatus[currentTerrainIndx] = NCS::YES;
				continue;
			}

			if(!U_TERRAIN->NodeToNLSConnectable(origFN->origNode, NLSnode->origNLS))
			{
				nodeConnectStatus[currentTerrainIndx] = NCS::NO_WAY;
				continue;
			}

			
			if(U_FAG->LineSegmentCollisionWithObstacles_ForbiddenNodeAndNLS(ls, origFN->origNode, NLSnode->origNLS))
			{
				nodeConnectStatus[currentTerrainIndx] = NCS::NO_WAY;
				continue;
			}
		}

		
		if(!nodeConnectionLSVs[currentTerrainIndx])
			nodeConnectionLSVs[currentTerrainIndx] = new CLineSegmentVisibility(ls);

		if(nodeConnectionLSVs[currentTerrainIndx]->TestUpdate(paths))
		{
			delete nodeConnectionLSVs[currentTerrainIndx];
			nodeConnectionLSVs[currentTerrainIndx] = 0;

			nodeConnectStatus[currentTerrainIndx] = NCS::YES;
		}
		else
			nodeConnectStatus[currentTerrainIndx] = NCS::NO;
		
		/*
		
		if(CLineSegmentVisibility::Test(ls,paths))
		{
			nodeConnectStatus[currentTerrainIndx] = NCS::YES;
		}
		else
			nodeConnectStatus[currentTerrainIndx] = NCS::NO;*/
	}
}

int CNLSFOVNDAT::GetNodeConnectability( CFOVNode* _node )
{
	return nodeConnectStatus[_node->origNode->terrainIndx];
}

bool CNLSFOVNDAT::ConnectionGood( CFOVNode* _node )
{
	return nodeConnectStatus[_node->origNode->terrainIndx] == NCS::YES;
}

CNLSFOVNDAT::~CNLSFOVNDAT()
{
	for(int i=0; i<U_TERRAIN->nodeNum; i++)
	{
		if(nodeConnectionLSVs[i])
			delete nodeConnectionLSVs[i];
	}

	delete []nodeConnectStatus;
	delete []nodeConnectionLSVs;
}

void CNLSFOVNDAT::MutualyAddNeighbours()
{
	CFOVNode* origFN;
	for(int i=0; i < FOV->origFOVNodes.size(); i++ )
	{
		origFN = FOV->origFOVNodes[i];

		if(ConnectionGood(origFN))
			F::NODE::MutualyAddFOVNodesAsNeighbours(origFN, NLSnode);
	}
}

void CNLSFOVNDAT::ClearNeighbours()
{
	NLSnode->origNeighbours.clear();
}
