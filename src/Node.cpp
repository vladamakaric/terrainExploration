#include "Node.h"
#include "f_distance.h"

void CTempNode::AddNeighboursToOpenNodesAndCalculatePFParams(vector<CPathFindingNode*> &_openNodes, CVector _goal)
{
	CPathFindingNode* currentPFN;
	bool setAsParrent;

	double localG;
	int neighbourNum = neighbours.size();
	for(int i=-1; i<neighbourNum; i++)
	{
		if(i==-1)
		{
			if(tempNeighbour)
			currentPFN = tempNeighbour;
			else 
			{
				i=0;
				currentPFN = neighbours[0];
			}
		}
		else
			currentPFN = neighbours[i];
		/////////////////////////////////////////////

		if(currentPFN->closed)
			continue;

		setAsParrent = false;

		localG = F::DISTANCE::GetDistance(position, currentPFN->position);

		if(!currentPFN->G)
		{
			_openNodes.push_back(currentPFN);
			setAsParrent = true;
		}

		if(localG+G < currentPFN->G)
			setAsParrent = true;

		if(setAsParrent)
		{
			if(!currentPFN->H)
				currentPFN->H = F::DISTANCE::GetDistance(_goal, currentPFN->position);

			currentPFN->G = localG+G;
			currentPFN->F = currentPFN->H + currentPFN->G;
			currentPFN->parent = this;
		}
	}
}