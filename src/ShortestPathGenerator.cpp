#include "ShortestPathGenerator.h"
#include "f_misc.h"
#include <algorithm>

double CShortestPathGenerator::pathLenght;
CPathFindingNode *CShortestPathGenerator::current;
vector<CPathFindingNode*> CShortestPathGenerator::openNodes(0);

void CShortestPathGenerator::Generate(CPathFindingNode *_start, CPathFindingNode *_goal, vector<CPathFindingNode*> &_path )
{
	CleanOpenNodes();

	current = _start;
	current->closed=true;
	openNodes.push_back(current);

	do
	{
		current->AddNeighboursToOpenNodesAndCalculatePFParams(openNodes, _goal->position);
		CalculateNextNode();

	}while(current!=_goal);
	pathLenght = _goal->G;


	//do 
	//_path.push_back(current); 
	//while(current = current->parent);

	while(1)
	{
		_path.push_back(current);

		if(!current->parent)
			break;

		current = current->parent;
	}

	reverse(_path.begin(), _path.end());
}

void CShortestPathGenerator::CalculateNextNode()
{
	int Fmin=1000000000;
	CPathFindingNode *next;
	
	for(int i=0; i< openNodes.size(); i++)
	{
		if(openNodes[i]->closed)
			continue;

		if(Fmin > openNodes[i]->F)
		{
			Fmin = openNodes[i]->F;
			next = openNodes[i];
		}
	}

	next->closed = true;
	current = next;
}

void CShortestPathGenerator::CleanOpenNodes()
{
	for(int i=0; i< openNodes.size(); i++)
		openNodes[i]->CleanPFInfo();

	openNodes.clear();
}

void CShortestPathGenerator::GenerateVectorPath( CPathFindingNode *_start, CPathFindingNode *_goal, vector<CVector> &_path )
{
	vector<CPathFindingNode*> path;
	Generate(_start,_goal,path);
	F::MISC::PathFindingNodePathToVectorPath(path, _path);
}

void CShortestPathGenerator::SavePathGValues( vector<CPathFindingNode*> &_path, vector<double> &_pathGValues )
{
	for(int i=0; i< _path.size(); i++)
		_pathGValues.push_back(_path[i]->G);
}

void CShortestPathGenerator::Clean()
{
	CleanOpenNodes();
}
