#pragma once
#include <vector>
#include "PathFindingNode.h"

using namespace std;

class CPathOptimizer
{
public:
	static void Optimize(vector<CVector> &optimizedPath, vector<CPathFindingNode*> &origPath);
};


