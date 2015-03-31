#pragma once
#include "PointCluster.h"
#include "Obstacle.h"
#include "Node.h"
#include "FOVNDAT.h"

using namespace std;
class CTerrain
{
public:
	CTerrain(int _clusterNum, int percentageOfLinks , int _HR1, int _HR2, int _minDistBetweenC, int _minDistBetweenCP, int _chainClusterIntersection, int _minDistBetweenCM, int _maxChainLinks, double _numOfCPKoef, int _k ,int _levelWidth, int _levelHeight);
	CTerrain(char *_fileName);
	~CTerrain();

	CPointCluster* GetSingleCluster();
	bool IsInSameChain(CPointCluster* _possibleChainMember, CPointCluster* _parrentCluster);
	bool IsClusterGood(const CVector &_position, int _HR, int _minDistBetweenC, CPointCluster* _parrentCluster = 0,  int _minDistBetweenCM = 0);
	void RemovePointsFromClusterChains(int _minDist);
	void RemovePointsFromClusterChain(CPointCluster* _first, int _minDist);
	void OptimiseObstacles();
	void Draw();

	bool GoodLine(CVector &_a, CVector &_b, int _indx1, int _indx2);
	bool LineIntersectsAllObstaclesExeptOne(CVector &_a, CVector &_b, int _ID = -1);
	CObstacle* PointInObstacles(CVector &_p);
	void PostConstructionSetup();
	void DetermineNodeConnectability();
	bool NodeToNLSConnectable(CNode* _node, CNodeLineSegment* _NLS);

	void DetermineNodesToNLSsConnectability();
	void DetermineNodeToNLSsConnectability(CNode* _node);
	bool NodeToNLSConnectableByMiddlePointAndBothNodes(CNode* _node, CNodeLineSegment* _NLS);
	CNodeLineSegment* GetNodeToEdgeNodeInfiniteLineNLSIntersection(CNode* _node, CNode* _edgeNode);

	void DegugModeToggle();
	void CountNodesAndAssignIndicesToNodesAndNLSs();
	void GenerateLevelEdges();
	bool NodesConnectable(CNode* _node1, CNode* _node2);
	bool NodesByIndexConnectable(int _node1Indx, int _node2Indx);

	void AddNeighboursToTempNodes();
	CNode* GetNearestNodeToPoint(CVector &point);


	////////////////
	vector<CPointCluster*> *pointClusters;
	vector<CObstacle> obstacles;
	CNodeLineSegment  *levelEdges[4];

	vector<CNode*> *nodeConnections; 
	int nodeNum;
	CNode **nodes;
	CNodeLineSegment **nodeLineSegments;
	bool **nodesToNLSsConnectability;
	bool **nodeConnectability;
	bool DebugMode;
};