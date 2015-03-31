#include "PathOptimizer.h"
#include <list>


void CPathOptimizer::Optimize( vector<CVector> &optimizedPath, vector<CPathFindingNode*> &origPath)
{
	list<CVector*> tempPath;

	for(int i=0; i<origPath.size(); i++)
	{
		optimizedPath.push_back(origPath[i]->position);
	}
}