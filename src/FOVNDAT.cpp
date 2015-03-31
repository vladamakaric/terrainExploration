#include "FOVNDAT.h"
#include "simulationDefinitions.h"
#include "f_node.h"

CFOVNDAT::CFOVNDAT()
{
	int nodeNum = U_TERRAIN->nodeNum;

	NLSFOVNNum = new int[nodeNum];
	FOVNodes = new CFOVNode*[nodeNum];
	NLSFOVNodes = new CFOVNode**[nodeNum];
	nodeConnectability = new int*[nodeNum];
	LSVsBetweenOrigNodes = new CLineSegmentVisibility**[nodeNum];

	for(int i=0; i<nodeNum; i++)
	{
		LSVsBetweenOrigNodes[i] = new CLineSegmentVisibility *[nodeNum];
		nodeConnectability[i] = new int[nodeNum];
	}

	memset(NLSFOVNNum, 0, sizeof(int)* nodeNum);
	memset(FOVNodes, 0, sizeof(CFOVNode*)* nodeNum);
	memset(NLSFOVNodes, 0, sizeof(CFOVNode**)* nodeNum);

	for(int i=0; i< nodeNum; i++)
	{
		for(int j=i+1; j< nodeNum; j++)
		{
			LSVsBetweenOrigNodes[i][j] = 0;
			nodeConnectability[i][j] = NCS::NOT_TESTED;

			if(!U_TERRAIN->NodesByIndexConnectable(i,j))
				nodeConnectability[i][j] = NCS::NO_WAY;

			nodeConnectability[j][i] = nodeConnectability[i][j];	
		}
	}
}

CFOVNode *CFOVNDAT::GetFOVNodeFromNode(CNode* _node)
{
	return FOVNodes[_node->terrainIndx];
}

int CFOVNDAT::GetFOVNodesFromNLS(CNodeLineSegment * _NLS, CFOVNode*** _NLSFOVNArrayPointer)
{
	if(NLSFOVNodes[_NLS->terrainIndx]!=0)
	{
		*_NLSFOVNArrayPointer = NLSFOVNodes[_NLS->terrainIndx];
		return NLSFOVNNum[_NLS->terrainIndx];
	}
	
	*_NLSFOVNArrayPointer = 0;
	_NLSFOVNArrayPointer = 0;
	return 0;
}

void CFOVNDAT::EraseFOVNodesFromNLS(CNodeLineSegment * _NLS)
{
	if(NLSEmpty(_NLS))
		return;

	CFOVNode **fovNodes;
	int fovNodesNumber = GetFOVNodesFromNLS(_NLS, &fovNodes);

	for(int i=0; i < fovNodesNumber; i++)
	{
		F::NODE::EraseConnectionsToAndFrom(fovNodes[i]);
		delete fovNodes[i];
	}
	
	delete []fovNodes;
	NLSFOVNNum[_NLS->terrainIndx] = 0;
	NLSFOVNodes[_NLS->terrainIndx] = 0;
}

CFOVNode* CFOVNDAT::CreateNewFOVNodeFromNode( CNode *_node )
{
	CFOVNode *tempFOVN = new CFOVNode(_node);
	FOVNodes[_node->terrainIndx] = tempFOVN;

	return tempFOVN;
}

void CFOVNDAT::SetNLSToVisited( CNodeLineSegment * _NLS )
{
	EraseFOVNodesFromNLS(_NLS);
	NLSFOVNNum[_NLS->terrainIndx] = -1;
}

bool CFOVNDAT::NLSVisited( CNodeLineSegment * _NLS )
{
	return NLSFOVNNum[_NLS->terrainIndx] == -1;
}

void CFOVNDAT::SetNewNLSFOVNodes( CNodeLineSegment * _NLS, CFOVNode** _newNLSFOVNArray, int _newNLSFOVNNumber, bool _erasePreviousFOVNs /*= true*/ )
{
	if(_erasePreviousFOVNs)
		EraseFOVNodesFromNLS(_NLS);
	else
	{	
		CFOVNode **fovNodes;
		GetFOVNodesFromNLS(_NLS, &fovNodes);

		if(fovNodes)
		delete []fovNodes;
	}

	NLSFOVNodes[_NLS->terrainIndx] = _newNLSFOVNArray;
	NLSFOVNNum[_NLS->terrainIndx] = _newNLSFOVNNumber;
}

bool CFOVNDAT::NLSEmpty( CNodeLineSegment * _NLS )
{
	return NLSFOVNNum[_NLS->terrainIndx] < 1;
}

int CFOVNDAT::NodeConnectability( CFOVNode* _node1, CFOVNode* _node2 )
{
	return nodeConnectability[_node1->origNode->terrainIndx][_node2->origNode->terrainIndx];
}

void CFOVNDAT::SetNodesConnectStatus(CFOVNode* _node1, CFOVNode* _node2, int _ncs)
{
	nodeConnectability[_node1->origNode->terrainIndx][_node2->origNode->terrainIndx] = _ncs;
	nodeConnectability[_node2->origNode->terrainIndx][_node1->origNode->terrainIndx] = _ncs;
}

int CFOVNDAT::GetFOVNodesFromNLSNumber( CNodeLineSegment * _NLS )
{
	return NLSFOVNNum[_NLS->terrainIndx];
}

void CFOVNDAT::InitializeLSV( CFOVNode* _node1, CFOVNode* _node2 )
{
	LSVsBetweenOrigNodes[_node1->origNode->terrainIndx][_node2->origNode->terrainIndx] = new CLineSegmentVisibility(CLineSegment(_node1->position, _node2->position));
}

CLineSegmentVisibility* CFOVNDAT::GetNodesLSV( CFOVNode* _node1, CFOVNode* _node2 )
{
	return LSVsBetweenOrigNodes[_node1->origNode->terrainIndx][_node2->origNode->terrainIndx];
}

void CFOVNDAT::EraseLSV( CFOVNode* _node1, CFOVNode* _node2 )
{
	delete LSVsBetweenOrigNodes[_node1->origNode->terrainIndx][_node2->origNode->terrainIndx];
	LSVsBetweenOrigNodes[_node1->origNode->terrainIndx][_node2->origNode->terrainIndx] = 0;
}
