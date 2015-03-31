#include "FOVNode.h"
#include "f_distance.h"

void CFOVNode::AddNeighboursToOpenNodesAndCalculatePFParams(vector<CPathFindingNode*> &_openNodes, CVector _goal)
{
	int origNeighbourNumber = origNeighbours.size();
	int neighbourNumber = NLSNeighbours.size() + origNeighbourNumber;
	CPathFindingNode* currentPFN;
	bool setAsParrent;

	double localG;
	for(int i=-1; i<neighbourNumber; i++)
	{
		if(i==-1)
		{
			if(!tempNeighbour)
				continue;

			currentPFN = tempNeighbour;
		}
		else
		{
			if(i<origNeighbourNumber)
				currentPFN = origNeighbours[i];
			else
				currentPFN = NLSNeighbours[i-origNeighbourNumber];
		}

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


void CFOVNode::AddNeighbour(CFOVNode* _neighbour)
{
	if(_neighbour->origNode)
		origNeighbours.push_back(_neighbour);
	else
		NLSNeighbours.push_back(_neighbour);
}

void CFOVNode::DrawConnections()
{
	CLineSegment ls;
	for(int i=0; i< NLSNeighbours.size(); i++)
	{
		ls.Set(NLSNeighbours[i]->position, position);
		ls.Draw();
	}

	for(int i=0; i< origNeighbours.size(); i++)
	{
		ls.Set(origNeighbours[i]->position, position);
		ls.Draw();
	}
}

void CFOVNode::ClearNeighbours()
{
	origNeighbours.clear();
	NLSNeighbours.clear();
	tempNeighbour = 0;
}

void CFOVNode::ClearOrigNeighboursTempNeighbour()
{
	for(int i=0; i< origNeighbours.size(); i++)
		origNeighbours[i]->tempNeighbour = 0;
}

CFOVNode::~CFOVNode()
{
	if(OrigNodeConnectionDAT)
		delete OrigNodeConnectionDAT;
}
