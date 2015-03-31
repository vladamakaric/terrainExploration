#pragma once
#include "Vector.h"
#include <vector>
#include "Obstacle.h"
#include "Circle.h"
#include <list>
#include "FOVNode.h"
#include "FOVNDAT.h"
#include "Path.h"

using namespace std;

class CFieldOfView
{
public:
	CFieldOfView() {DAT = new CFOVNDAT();}
	void Update(CCircle *_FOVCircle);
	void MutualyConnectNewCurrentlyVisibleFOVNodes();
	void ConnectNewCurrentlyVisibleFOVNodesWithOldOnes();
	void TryConnectingNewFOVNodeWithOldOnes(CFOVNode * _FOVNode);
	void TaggVisibleNLSFOVNodes(CNodeLineSegment* _NLS);
	void DetermineEdgeOrigFOVNodes();
	void DetermineCurrentlyVisibleEdgeFOVNodes();
	void CalculateFOVCircleIntersectionFOVNodes();
	void IntersectFOVCircleWithNLS_OneOrigFOVNodeVisible(const CVector &_intersection, CNodeLineSegment* _NLS, CFOVNode * _visibleEdgeFOVN, CFOVNode** _NLSFOVNodes, int _NLSFOVNNumber);
	void IntersectFOVCircleWithNLS_NoOrigFOVNodesVisible(CVector *_intersections, int _interNum,CNodeLineSegment* _NLS, CFOVNode** _NLSFOVNodes, int _NLSFOVNNumber);
	void ConnectFOVNodesOnSameNLS(CFOVNode* _a, CFOVNode*_b);
	void UntagNLSFOVNodes(CNodeLineSegment *_NLS);

	bool NewNLSIntersectionsWithNoFOVNodesVisibleGood(CVector* intersections, int interNum, CNodeLineSegment* _NLS);
	void DetermineNewCurrentlyVisibleFOVNodes();

	bool NodeVisible(CNode *_node); 
	bool FOVNodeVisible(CFOVNode *_FOVnode);
	bool NLSFOVNodeVisible(CFOVNode *_FOVnode, bool checkForPointInCircle=true);
	bool NLSFOVNodeVisibleCircleRadiusOffset(CFOVNode *_FOVnode, double _offset);
	int GetVisibleIntersectionsNumber(CVector* intersections,int interNum, CNodeLineSegment* _NLS);

	void AConnectsToBUsingEmptyNextOrPrevious(CFOVNode *_a, CFOVNode *_b);
	CFOVNode *GetTagedOrigFOVNodeFromNLS(CNodeLineSegment &_nls);
	CFOVNode *GetOtherFOVNodeFromNLS(CFOVNode *_FOVnode, CNodeLineSegment &_nls);

	bool NodeLineSegmentVisibleIfOnlyOneNodeVisible(CNode*visibleNode, CNodeLineSegment* _NLS);
	CFOVNode *GetLastFOVNodeInChain_NextDirection(CFOVNode *_FOVnode, bool onlyOrig=false);
	CFOVNode *GetLastFOVNodeInChain_PreviousDirection(CFOVNode *_FOVnode, bool onlyOrig=false);
	void ClearContainers();
	void UpdateIntersectedNLSList();
	void UpdateEdgeFOVNodes();
	void UpdateFOVNodesConnections(const vector<CPath*> &paths);
	void UpdateOrigNodeToOrigNodeConnections(const vector<CPath*> &paths);
	void UpdateOrigNodeToNLSNodeConnections(const vector<CPath*> &paths);
	void TaggObstacleFOVNodes(CObstacle& _obstacle);
	bool PointInVisibleObstacles(CVector &_point);

	void ClearConnectionsToNLSNodes();
	bool LineSegmentIntersectsVisibleObstacles(const CLineSegment &_ls, CFOVNode* _forbidenNode=0, bool _intersectWithNLSOnForbidenNodeObstacle = true);
	CNodeLineSegment* GetLineSegmentIntersectsVisibleObstaclesClosestToPointB(const CLineSegment &_ls);
	void DrawNodeConnections();
	void DrawVisibleEdges();
	void DetermineWholeObstacles();
	


	CCircle *FOVCircle;
	CFOVNDAT* DAT;
	
	list<CNodeLineSegment*> intersectedNLSs;

	vector<CObstacle*> obstacles;
	vector<CNodeLineSegment*> visitedNLSs;
	vector<CFOVNode*> origFOVNodes;
	vector<CFOVNode*> edgdeFOVNodes;

	vector<CFOVNode*> edgdeOrigFOVNodes;
	vector<CFOVNode*> newCurrentlyVisibleOrigFOVNodes;
	vector<CFOVNode*> newCurrentlyVisibleNLSFOVNodes;
	vector<CFOVNode*> currentlyVisibleEdgeOrigFOVNodes;
	vector<CNodeLineSegment*> VCNLS; //viewCircleNodeLineSegs
	vector<CNode*> NCVNLSN; // notCompletlyVisibleNLSNodes
	bool altered;

	void Draw();
	void DeadBeefNLS(CNodeLineSegment* _NLS); //generate error
};