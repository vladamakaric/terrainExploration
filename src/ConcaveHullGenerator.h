#pragma once

#include "Vector.h"
#include <vector>
#include "PointCluster.h"

using namespace std;

class CConcaveHullGenerator
{
	int k;
	int firstPointIndex;
	bool fistPointRemoved;
	vector<CVector*> *points;
	vector<CVector*> *hull; 
	vector<int> addedPointIndices;
	vector<CVector*> kNearestVecs;
	vector<int> kNearestIndices;

	bool IsKVecGood(int _pointIndex);
	bool IsAdded(int _i);
	bool CalculateNextHullPoint(const CVector &a);
	int GetExtremeYAxisVectorIndex();
	bool CalculateKNearestVecs();
	void AddAllChainedClusterPoints(CPointCluster *_pCluster);
public:

	void Generate(CPointCluster *_pCluster, vector<CVector*> *_polyPoints, int _k, vector<CVector*> *_noClusterPoints = 0);
	CConcaveHullGenerator(){};
};