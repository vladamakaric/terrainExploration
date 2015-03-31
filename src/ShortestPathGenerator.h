#pragma once
#include <vector>
#include "PathFindingNode.h"
#include "Vector.h"
#include "Terrain.h"


using namespace std;

class CShortestPathGenerator
{
public:
	static double pathLenght;
	static CPathFindingNode* current;
	static vector<CPathFindingNode*> openNodes;

	static void GenerateVectorPath(CPathFindingNode *_start, CPathFindingNode *_goal, vector<CVector> &_path);
	static void Generate(CPathFindingNode *_start, CPathFindingNode *_goal, vector<CPathFindingNode*> &path);
	static void SavePathGValues(vector<CPathFindingNode*> &_path, vector<double> &_pathGValues);
	static void CalculateNextNode();
	static void CleanOpenNodes();
	static void Clean();
	
};