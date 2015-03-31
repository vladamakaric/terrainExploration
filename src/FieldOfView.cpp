#include "FieldOfView.h"
#include "simulationDefinitions.h"
#include <vector>
#include "NodeLineSegment.h"
#include "f_intersection.h"
#include "f_distance.h"
#include "f_misc.h"
#include "f_node.h"
#include "f_vector.h"
#include "LineSegmentVisibility.h"
#include "PrecompiledShapes.h"
#include "Node.h"
#include "NLSFOVNDAT.h"
#include "NearestCandidate.h"

void CFieldOfView::Update(CCircle *_FOVCircle)
{
	int visitedNLSsNum = visitedNLSs.size();
	altered = false;

	FOVCircle = _FOVCircle;
	ClearContainers();

	U_FAG->GetCircleNodeLineSegments(*FOVCircle, VCNLS); 
	F::NODE::GetUniqueNodesFromNLSs(NCVNLSN, VCNLS);

	DetermineNewCurrentlyVisibleFOVNodes();
	MutualyConnectNewCurrentlyVisibleFOVNodes();
	ConnectNewCurrentlyVisibleFOVNodesWithOldOnes();	
	DetermineEdgeOrigFOVNodes();
	DetermineCurrentlyVisibleEdgeFOVNodes();
	CalculateFOVCircleIntersectionFOVNodes();
	UpdateIntersectedNLSList();

	F::MISC::UntagPointerContainer(origFOVNodes);
	UpdateEdgeFOVNodes();
	F::MISC::UntagPointerContainer(origFOVNodes);

	DetermineWholeObstacles();


	altered = visitedNLSsNum != visitedNLSs.size() || newCurrentlyVisibleNLSFOVNodes.size() || newCurrentlyVisibleOrigFOVNodes.size();
}

bool CFieldOfView::NodeVisible(CNode *_node)
{
	if(F::INTERSECTION::PointInCircle(_node->position,*FOVCircle))
		return !U_FAG->LineSegmentCollisionWithObstacles_ForbiddenNodes( CLineSegment(FOVCircle->position, _node->position) ,_node);

	return false;
}

bool CFieldOfView::NLSFOVNodeVisible(CFOVNode *_FOVNode, bool checkForPointInCircle  /*true*/)
{
	bool testLineSegmentCollision = true;

	if(checkForPointInCircle)
		testLineSegmentCollision = F::INTERSECTION::PointInCircle(_FOVNode->position, *FOVCircle);

	if(testLineSegmentCollision)
		return !U_FAG->LineSegmentCollisionWithObstacles_ForbiddenNLSs( CLineSegment(FOVCircle->position, _FOVNode->position), _FOVNode->origNLS);

	return 0;
}

void CFieldOfView::ConnectNewCurrentlyVisibleFOVNodesWithOldOnes()
{
	for (int i=0; i<newCurrentlyVisibleOrigFOVNodes.size(); i++)
		TryConnectingNewFOVNodeWithOldOnes(newCurrentlyVisibleOrigFOVNodes[i]);
}

void CFieldOfView::MutualyConnectNewCurrentlyVisibleFOVNodes()
{
	for(int i=0; i < newCurrentlyVisibleOrigFOVNodes.size(); i++)
	{
		for(int j=i+1; j < newCurrentlyVisibleOrigFOVNodes.size(); j++)
			F::NODE::ConnectTwoFOVNodes(newCurrentlyVisibleOrigFOVNodes[i], newCurrentlyVisibleOrigFOVNodes[j]);
	}
}

void CFieldOfView::DetermineNewCurrentlyVisibleFOVNodes()
{
	CFOVNode *temp;
	for(int i=0; i<NCVNLSN.size(); i++)
	{
		if(DAT->GetFOVNodeFromNode(NCVNLSN[i]))
			continue;

		if(NodeVisible(NCVNLSN[i]))
		{
			temp = DAT->CreateNewFOVNodeFromNode(NCVNLSN[i]);
			newCurrentlyVisibleOrigFOVNodes.push_back(temp);
			origFOVNodes.push_back(temp);
		}
	}
}

CFOVNode * CFieldOfView::GetOtherFOVNodeFromNLS( CFOVNode *_FOVnode, CNodeLineSegment &_nls )
{
	if(_FOVnode->origNode == _nls.down)
		return DAT->GetFOVNodeFromNode(_nls.up);

	if(_FOVnode->origNode == _nls.up)
		return DAT->GetFOVNodeFromNode(_nls.down);

	return 0;
}

void CFieldOfView::AConnectsToBUsingEmptyNextOrPrevious( CFOVNode *_a, CFOVNode *_b )
{
	if(!_a->next)
		_a->next = _b;

	if(!_a->previous)
		_a->previous = _b;

	return;
}

void CFieldOfView::TryConnectingNewFOVNodeWithOldOnes( CFOVNode * _FOVNode )
{
	if(!_FOVNode->next)
	{
		CFOVNode* temp = DAT->GetFOVNodeFromNode(_FOVNode->origNode->next);

		if(temp)
		{
			if(NodeVisible(temp->origNode))
			{
				temp->previous = _FOVNode;
				_FOVNode->next = temp;
			}
		}
	}

	if(!_FOVNode->previous)
	{
		CFOVNode* temp = DAT->GetFOVNodeFromNode(_FOVNode->origNode->previous);

		if(temp)
		{
			if(NodeVisible(temp->origNode))
			{
				temp->next = _FOVNode;
				_FOVNode->previous = temp;
			}
		}
	}
}

void CFieldOfView::DetermineEdgeOrigFOVNodes()
{
	for (int i=0; i<origFOVNodes.size(); i++)
	{
		if(origFOVNodes[i]->tagged)
			continue;

		CFOVNode* current = origFOVNodes[i];
		CFOVNode* nextEdge = GetLastFOVNodeInChain_NextDirection(current, true);
		CFOVNode* prevEdge = GetLastFOVNodeInChain_PreviousDirection(current, true);

		if(nextEdge)
			edgdeOrigFOVNodes.push_back(nextEdge);

		if(nextEdge != prevEdge && prevEdge)
			edgdeOrigFOVNodes.push_back(prevEdge);
	}
}

void CFieldOfView::DetermineCurrentlyVisibleEdgeFOVNodes()
{
	F::MISC::UntagPointerContainer(origFOVNodes);
	for (int i=0; i<edgdeOrigFOVNodes.size(); i++)
	{
		if(F::INTERSECTION::PointInCircle(edgdeOrigFOVNodes[i]->position, *FOVCircle))
		{
			if(NodeVisible(edgdeOrigFOVNodes[i]->origNode))
				edgdeOrigFOVNodes[i]->tagged = true;
		}
	}
}

CFOVNode * CFieldOfView::GetLastFOVNodeInChain_NextDirection( CFOVNode *_FOVnode, bool onlyOrig)
{
	CFOVNode* current = _FOVnode;

	while(1)
	{
		current->tagged = true;

		if(!current->next)
			return current;

		if(current->next->tagged)
			return 0;

		if(current->next->origNode)
		{
			current = current->next;
			continue;
		}

		if(!onlyOrig)
		{
			current->next->tagged=true;
			return current->next;
		}

		return current;
	}
}

CFOVNode * CFieldOfView::GetLastFOVNodeInChain_PreviousDirection( CFOVNode *_FOVnode, bool onlyOrig)
{
	CFOVNode* current = _FOVnode;

	while(1)
	{
		current->tagged = true;

		if(!current->previous)
			return current;

		if(current->previous->tagged)
			return 0;

		if(current->previous->origNode)
		{
			current = current->previous;
			continue;
		}

		if(!onlyOrig)
		{
			current->previous->tagged=true;
			return current->previous;
		}

		return current;
	}
}

void CFieldOfView::CalculateFOVCircleIntersectionFOVNodes()
{
	CFOVNode *a,*b,*visible = 0;
	CLine line;

	for(int i=0; i < VCNLS.size(); i++)
	{
		////////////////////////////////////////////////////////////////
		if(DAT->NLSVisited(VCNLS[i]))
			continue;

		a = DAT->GetFOVNodeFromNode(VCNLS[i]->down);
		b = DAT->GetFOVNodeFromNode(VCNLS[i]->up);

		if(a && b)
		if(F::NODE::FOVNodesConnected(a,b))
		{
			visitedNLSs.push_back(VCNLS[i]);
			DAT->SetNLSToVisited(VCNLS[i]);
			continue;
		}

		if(!F::INTERSECTION::CircleIntersectsCircle(CCircle(*VCNLS[i]), *FOVCircle))
			continue;

		line.Set(*VCNLS[i]);
		if(!F::INTERSECTION::LineIntersectsCircle(line, *FOVCircle))
			continue;
		///////////////////////////////////////////////////////////////////////

		bool exist = a || b;
		visible =0;

		if(exist)
		{
			if(a)
				if(a->tagged)
					visible = a;

			if(!visible)
				if(b)
					if(b->tagged)
						visible = b;
		}
		
		CFOVNode** NLSFOVNodes;
		int NLSFOVNNumber = DAT->GetFOVNodesFromNLS(VCNLS[i], &NLSFOVNodes);
		CLineSegment ls(*VCNLS[i]);
		CVector intersections[2];
		int interNum = F::INTERSECTION::GetLineSegmentCircleIntersection(ls, *FOVCircle, intersections, false);
		interNum = GetVisibleIntersectionsNumber(intersections, interNum, VCNLS[i]);

		if(!interNum) 
		{	
			if(a && b)
			if(NodeVisible(a->origNode) && NodeVisible(b->origNode))
			{					
				visitedNLSs.push_back(VCNLS[i]);
				DAT->SetNLSToVisited(VCNLS[i]);
				ConnectFOVNodesOnSameNLS(a,b);
			}	
			continue;
		}

		TaggVisibleNLSFOVNodes(VCNLS[i]);

		if(!exist || !visible )
		{
			if(interNum==2)
			IntersectFOVCircleWithNLS_NoOrigFOVNodesVisible(intersections, interNum, VCNLS[i], NLSFOVNodes, NLSFOVNNumber);
		}
		else
			IntersectFOVCircleWithNLS_OneOrigFOVNodeVisible(intersections[0], VCNLS[i],visible, NLSFOVNodes, NLSFOVNNumber );
	}
}

void CFieldOfView::IntersectFOVCircleWithNLS_OneOrigFOVNodeVisible(const CVector &_intersection, CNodeLineSegment* _NLS, CFOVNode * _visibleEdgeFOVN, CFOVNode** _NLSFOVNodes, int _NLSFOVNNumber)
{
	CFOVNode* connection=0;
	CFOVNode* outOfViewConnection=0;
	int addIntersection=1;
	int visibleFOVNNumber = 0;

	for(int i=0; i<_NLSFOVNNumber; i++)
		if(_NLSFOVNodes[i]->tagged)
			visibleFOVNNumber++;
	
	for(int i=0; i<_NLSFOVNNumber; i++)
	{
		connection = F::NODE::GetAnyConnection(_NLSFOVNodes[i]);
		if(!_NLSFOVNodes[i]->tagged)
		{
			if(connection == _visibleEdgeFOVN) 
				return;
			continue;
		}

		if(connection)
		if(connection->origNode && connection != _visibleEdgeFOVN)
		{
			DAT->SetNLSToVisited(_NLS);
			visitedNLSs.push_back(_NLS);
			ConnectFOVNodesOnSameNLS(connection, _visibleEdgeFOVN);
			return;
		}

		if(connection)
		if(!connection->tagged)
		{
			outOfViewConnection = connection;
			addIntersection = 0;
			break;
		}
	}

	int newFOVNodesNum = _NLSFOVNNumber -  visibleFOVNNumber + addIntersection;
	CFOVNode** newFOVNodes = new CFOVNode*[newFOVNodesNum];

	for(int i=addIntersection, j=0; j<_NLSFOVNNumber; j++)
	{
		if(_NLSFOVNodes[j]->tagged)
		{
			F::NODE::EraseConnectionsToAndFrom(_NLSFOVNodes[j]);
			delete _NLSFOVNodes[j];
			continue;
		}

		newFOVNodes[i]=_NLSFOVNodes[j];
		i++;
	}

	CNode* otherNode = F::NODE::GetOtherNodeOnLineSegment(_visibleEdgeFOVN->origNode, _NLS);
	bool connectNext = F::NODE::ANextIsB(_visibleEdgeFOVN->origNode, otherNode);

	if(addIntersection)
	{
		newFOVNodes[0] = new CFOVNode(_intersection, _NLS);
		////
		newCurrentlyVisibleNLSFOVNodes.push_back(newFOVNodes[0]);
		////

		if(connectNext)
			_visibleEdgeFOVN->next = newFOVNodes[0];
		else
			_visibleEdgeFOVN->previous = newFOVNodes[0];

		newFOVNodes[0]->next = _visibleEdgeFOVN;
	}
	else if(outOfViewConnection)
	{
		if(connectNext)
			_visibleEdgeFOVN->next = outOfViewConnection;
		else
			_visibleEdgeFOVN->previous = outOfViewConnection;

		outOfViewConnection->next = _visibleEdgeFOVN;
	}

	if(!_NLSFOVNNumber) intersectedNLSs.push_back(_NLS);
	DAT->SetNewNLSFOVNodes(_NLS, newFOVNodes, newFOVNodesNum, false);
	//////////////////////
	DeadBeefNLS(_NLS);////
	//////////////////////
}

void CFieldOfView::IntersectFOVCircleWithNLS_NoOrigFOVNodesVisible(CVector *_intersections, int _interNum,CNodeLineSegment* _NLS, CFOVNode** _NLSFOVNodes, int _NLSFOVNNumber)
{
	CFOVNode *minDistFN=0;
	CFOVNode *maxDistFN=0;
	CVector refPoint = _NLS->down->position;
	double maxDist = 0;
	double minDist= 99999999999999;
	double dist;
	int visibleNLSFNodes=0;

	for(int i=0; i<_NLSFOVNNumber; i++)
	{
		if(!_NLSFOVNodes[i]->tagged)
			continue;

		visibleNLSFNodes++;
		dist = F::DISTANCE::GetDistanceSq(refPoint, _NLSFOVNodes[i]->position);

		if(dist > maxDist)
		{
			maxDistFN = _NLSFOVNodes[i];
			maxDist = dist;
		}

		if(dist < minDist)
		{
			minDistFN = _NLSFOVNodes[i];
			minDist = dist;
		}
	}

	if(!maxDist)
	{
		if(!NewNLSIntersectionsWithNoFOVNodesVisibleGood(_intersections, _interNum, _NLS))
			return;

		CFOVNode** newFovNodes = new CFOVNode*[_interNum+_NLSFOVNNumber];

		for(int i=0; i<_interNum; i++)
		{
			newFovNodes[i] = new CFOVNode(_intersections[i], _NLS);


			/////////
			newCurrentlyVisibleNLSFOVNodes.push_back(newFovNodes[i]);
		}

		for(int i=0; i<_NLSFOVNNumber; i++)
			newFovNodes[i+_interNum] = _NLSFOVNodes[i];

		if(_interNum==2)
		{
			newFovNodes[0]->next = newFovNodes[1];
			newFovNodes[1]->previous = newFovNodes[0];
		}

		if(!_NLSFOVNNumber) intersectedNLSs.push_back(_NLS);
		DAT->SetNewNLSFOVNodes(_NLS, newFovNodes,_NLSFOVNNumber+_interNum, false);
		//////////////////////
		DeadBeefNLS(_NLS);////
		//////////////////////
		return;
	}

	if(maxDistFN)
	{
		CFOVNode *tempMinDistFN = F::NODE::GetAnyConnection(minDistFN);
		CFOVNode *tempMaxDistFN = F::NODE::GetAnyConnection(maxDistFN);
		double tempMinDist, tempMaxDist;

		bool minFNIsOrig = false, maxFNIsOrig = false;

		if(tempMinDistFN)
		if((tempMinDist = F::DISTANCE::GetDistanceSq(refPoint, tempMinDistFN->position))<minDist)
		{
			minDistFN = tempMinDistFN;
			minDist = tempMinDist;
		}

		if(tempMaxDistFN)
		if((tempMaxDist = F::DISTANCE::GetDistanceSq(refPoint, tempMaxDistFN->position))>maxDist)
		{
			maxDistFN = tempMaxDistFN;
			maxDist = tempMaxDist;
		}

		minFNIsOrig = minDistFN->origNode;
		maxFNIsOrig = maxDistFN->origNode;

		bool newMaxFN=false;
		bool newMinFN=false;

		if(	minFNIsOrig && maxFNIsOrig)
		{
			DAT->SetNLSToVisited(_NLS);
			ConnectFOVNodesOnSameNLS(minDistFN, maxDistFN);



			visitedNLSs.push_back(_NLS);
			return;
		}

		for(int i=0; i<_interNum; i++)
		{
			dist = F::DISTANCE::GetDistanceSq(refPoint, _intersections[i]);

			if(dist>maxDist)
			{
				newMaxFN = true;
				maxDistFN = new CFOVNode(_intersections[i], _NLS);
				maxDist = dist;

				//////////
				newCurrentlyVisibleNLSFOVNodes.push_back(maxDistFN);
				///
				continue;
			}

			if(dist<minDist)
			{
				newMinFN = true;
				minDistFN = new CFOVNode(_intersections[i], _NLS);
				minDist = dist;

				//////////
				newCurrentlyVisibleNLSFOVNodes.push_back(minDistFN);
				////
			}
		}


		if(!newMaxFN)
		{
			if(maxDistFN->tagged)
			{
				maxDistFN->tagged=false;
				visibleNLSFNodes--;
			}
		}

		if(!newMinFN)
		{
			if(minDistFN->tagged)
			{
				minDistFN->tagged=false;
				visibleNLSFNodes--;
			}
		}

		int newFNodesNum;

		if(!newMinFN && !newMaxFN)
			newFNodesNum =0;

		if(newMinFN || newMaxFN)
			newFNodesNum =1;

		if(newMaxFN && newMinFN)
			newFNodesNum = 2;

		int newNLSFOVNodesNum = _NLSFOVNNumber - visibleNLSFNodes + newFNodesNum;

		CFOVNode** newFOVNodes = new CFOVNode*[newNLSFOVNodesNum];

		int tempIndx =0;

		if(newMaxFN)
		{
			newFOVNodes[0] = maxDistFN;
			tempIndx++;
		}

		if(newMinFN)
			newFOVNodes[tempIndx] = minDistFN;

		for(int i=newFNodesNum, j=0; j<_NLSFOVNNumber; j++)
		{
			if(_NLSFOVNodes[j]->tagged)
			{
				F::NODE::EraseConnectionsToAndFrom(_NLSFOVNodes[j]);
				delete _NLSFOVNodes[j];
				continue;
			}

			newFOVNodes[i]=_NLSFOVNodes[j];
			i++;
		}
		////////////////////////////////////////////////////////////////////////////////connect

		if(maxFNIsOrig)
		{
			
			CNode* otherNLSNode = F::NODE::GetOtherNodeOnLineSegment(maxDistFN->origNode, _NLS);
			bool connectNext = maxDistFN->origNode->next == otherNLSNode;

			if(connectNext)
				maxDistFN->next = minDistFN;
			else
				maxDistFN->previous = minDistFN;
		}
		else
		{
			F::NODE::EraseConnectionsTo(maxDistFN);
			maxDistFN->previous = minDistFN;
		}

		if(minFNIsOrig)
		{
			CNode* otherNLSNode = F::NODE::GetOtherNodeOnLineSegment(minDistFN->origNode, _NLS);
			bool connectNext = minDistFN->origNode->next == otherNLSNode;

			if(connectNext)
				minDistFN->next = maxDistFN;
			else
				minDistFN->previous = maxDistFN;
		}
		else
		{
			F::NODE::EraseConnectionsTo(minDistFN);
			minDistFN->next = maxDistFN;
		}
		///////////////////////////////////////////////////////////////////////////////////////////
		DAT->SetNewNLSFOVNodes(_NLS, newFOVNodes,newNLSFOVNodesNum, false);

		//////////////////////
		DeadBeefNLS(_NLS);////
		//////////////////////
	}
}

int CFieldOfView::GetVisibleIntersectionsNumber( CVector* intersections,int interNum , CNodeLineSegment* _NLS)
{
	if(interNum==1)
	{
		CFOVNode* temp = new CFOVNode(intersections[0], _NLS);
		if(NLSFOVNodeVisible(temp,false))
		{
			delete temp;
			return 1;
		}
	}

	if(interNum==2)
	{
		int i=0;
		CFOVNode* temp = new CFOVNode(intersections[0], _NLS);
		if(NLSFOVNodeVisible(temp,false))
		{
			i++;
			delete temp;
		}

		temp = new CFOVNode(intersections[1], _NLS);
		if(NLSFOVNodeVisible(temp,false))
		{
			intersections[i]= intersections[1];
			i++;
			delete temp;
		}

		return i;
	}

	return 0;
}

bool CFieldOfView::NodeLineSegmentVisibleIfOnlyOneNodeVisible( CNode*visibleNode, CNodeLineSegment* _NLS )
{
	CNode *otherNodeOnNLS = F::NODE::GetOtherNodeOnLineSegment(visibleNode,_NLS);
	CNode *nodeOnOtherNLS = F::NODE::GetOtherConnectedNode(otherNodeOnNLS, visibleNode);

	CVector visionRay = F::VECTOR::GetVectorFromAToB(FOVCircle->position, visibleNode->position).Direction();
	CVector nlsDirection = F::VECTOR::GetVectorFromAToB(visibleNode->position, otherNodeOnNLS->position).Direction();
	CVector otherNLSDirection = F::VECTOR::GetVectorFromAToB(visibleNode->position,nodeOnOtherNLS->position ).Direction();

	return !F::VECTOR::VectorBetweenTwoVectors(nlsDirection, visionRay, otherNLSDirection);
}



void CFieldOfView::ConnectFOVNodesOnSameNLS( CFOVNode* _a, CFOVNode*_b )
{
	if(_a->origNode->next == _b->origNode)
	{
		_a->next = _b;
		_b->previous = _a;
	}
	else
	{
		_b->next = _a;
		_a->previous = _b;
	}
}

bool CFieldOfView::NewNLSIntersectionsWithNoFOVNodesVisibleGood( CVector* intersections, int interNum, CNodeLineSegment* _NLS )
{
	CFOVNode **fovNodes;
	int fovNodesNumber = DAT->GetFOVNodesFromNLS(_NLS, &fovNodes);

	if(!fovNodesNumber)
		return true;

	bool vertical = (_NLS->down->position.y - _NLS->up->position.y) !=0;


	//ovde CVektor je ustvari jednodimnezionalna linija, x je levo, a y je desno
	vector<CVector> oneDLines; 

	for(int i=0; i<fovNodesNumber; i++)
	{
		if(fovNodes[i]->tagged)
			continue;

		CFOVNode *otherFNode = F::NODE::GetAnyConnection(fovNodes[i]);

		if(otherFNode)
		{
			if(otherFNode->origNLS)
				otherFNode->tagged = true;

			double val1, val2;

			if(vertical)
			{
				val1 = otherFNode->position.y;
				val2 = fovNodes[i]->position.y;
			}
			else
			{
				val1 = otherFNode->position.x;
				val2 = fovNodes[i]->position.x;
			}

			oneDLines.push_back(CVector(F::MISC::Min(val1, val2),F::MISC::Max(val1, val2)));
		}
	}

	bool intersectionsGood = true;
	CVector middleIntersect;

	if(interNum==1)
		middleIntersect = intersections[0];
	else
		middleIntersect = (intersections[0] + intersections[1])/2;

	for(int j=0; j<oneDLines.size(); j++)
	{
		double comparisonVal = middleIntersect.x;
		if(vertical)
			comparisonVal = middleIntersect.y;

		if(comparisonVal >= oneDLines[j].x && comparisonVal <= oneDLines[j].y)
		{
			intersectionsGood = false;
			break;
		}
	}
	
	for(int i=0; i<fovNodesNumber; i++)
		fovNodes[i]->tagged = false;

	return intersectionsGood;
}

void CFieldOfView::TaggVisibleNLSFOVNodes( CNodeLineSegment* _NLS )
{
	CFOVNode **fovNodes;
	int fovNodesNumber = DAT->GetFOVNodesFromNLS(_NLS, &fovNodes);
	if(!fovNodesNumber) return;

	for(int i=0; i < fovNodesNumber; i++)
	{
		if(NLSFOVNodeVisible(fovNodes[i]))
			fovNodes[i]->tagged= true;
	}
}

void CFieldOfView::ClearContainers()
{
	newCurrentlyVisibleNLSFOVNodes.clear();
	newCurrentlyVisibleOrigFOVNodes.clear();
	NCVNLSN.clear();
	VCNLS.clear();
	edgdeOrigFOVNodes.clear();
	currentlyVisibleEdgeOrigFOVNodes.clear();
}

void CFieldOfView::UpdateIntersectedNLSList()
{
	list<CNodeLineSegment*>::iterator iter = intersectedNLSs.begin();
	while(iter!=intersectedNLSs.end())
	{
		if(DAT->NLSEmpty(*iter))
		{
			iter = intersectedNLSs.erase(iter);
		}
		else
			iter++;
	}
}

void CFieldOfView::UpdateEdgeFOVNodes()
{
	edgdeFOVNodes.clear();

	CFOVNode* nextEdge=0;
	CFOVNode* previousEdge=0;
	for(int i=0; i<origFOVNodes.size(); i++)
	{
		if(origFOVNodes[i]->tagged)
			continue;

		nextEdge = GetLastFOVNodeInChain_NextDirection(origFOVNodes[i]);
		previousEdge = GetLastFOVNodeInChain_PreviousDirection(origFOVNodes[i]);

		if(nextEdge)
			edgdeFOVNodes.push_back(nextEdge);

		if(previousEdge && nextEdge!=previousEdge)
		{
			edgdeFOVNodes.push_back(previousEdge);
		}
	}

	CFOVNode** NLSFOVNodes;
	int NLSFOVNNumber;

	nextEdge=0;
	previousEdge=0;
	for(list<CNodeLineSegment*>::iterator iter = intersectedNLSs.begin(); iter!=intersectedNLSs.end(); iter++)
	{
		if(DAT->NLSEmpty(*iter))
			continue;

		NLSFOVNNumber = DAT->GetFOVNodesFromNLS(*iter, &NLSFOVNodes);

		for(int i=0; i<NLSFOVNNumber; i++)
		{
			if(NLSFOVNodes[i]->tagged)
				continue;

			edgdeFOVNodes.push_back(NLSFOVNodes[i]);
			CFOVNode* connection = F::NODE::GetAnyConnection(NLSFOVNodes[i]);

			if(connection)
			{
				connection->tagged = true;
				edgdeFOVNodes.push_back(connection);
			}
		}
	}

	for(list<CNodeLineSegment*>::iterator iter = intersectedNLSs.begin(); iter!=intersectedNLSs.end(); iter++)
		UntagNLSFOVNodes(*iter);
}

void CFieldOfView::UntagNLSFOVNodes( CNodeLineSegment *_NLS )
{
	CFOVNode **NLSFOVNodes;
	int NLSFOVNNumber = DAT->GetFOVNodesFromNLS(_NLS, &NLSFOVNodes);

	/*
	CFOVNode *fnode1 = DAT->GetFOVNodeFromNode(_NLS->down);
	CFOVNode *fnode2 = DAT->GetFOVNodeFromNode(_NLS->up);

	if(fnode1)
		fnode1->tagged =*/

	for (int i=0; i<NLSFOVNNumber; i++)
		NLSFOVNodes[i]->tagged=false;
}

void CFieldOfView::DeadBeefNLS( CNodeLineSegment* _NLS )
{
	CFOVNode **NLSFOVNodes;
	int NLSFOVNNumber = DAT->GetFOVNodesFromNLS(_NLS, &NLSFOVNodes);
	int sum=0;
	for (int i=0; i<NLSFOVNNumber; i++)
	{
		sum += NLSFOVNodes[i]->position.x + NLSFOVNodes[i]->position.y + NLSFOVNodes[i]->tagged;
	}
}

bool CFieldOfView::LineSegmentIntersectsVisibleObstacles( const CLineSegment &_ls, CFOVNode* _forbidenNode, bool _intersectWithForbidenNodeObstacle)
{
	if(!U_FAG->LineSegmentCollisionWithObstacles(_ls))
		return false;

	CObstacle *forbidenObstacle=0;
	CNodeLineSegment* NLS =0;

	if(_forbidenNode)
	if(_forbidenNode->origNLS)
		NLS = _forbidenNode->origNLS;

	if(!_intersectWithForbidenNodeObstacle)
	{
		if(_forbidenNode->origNLS)
			forbidenObstacle = _forbidenNode->origNLS->polygon;
		else
			forbidenObstacle = _forbidenNode->origNode->polygon;
	}

	CLineSegment ls;
	for(int i=0; i<visitedNLSs.size(); i++)
	{
		if(visitedNLSs[i]->polygon == forbidenObstacle)
			continue;

		if(NLS == visitedNLSs[i])
			continue;

		if(_forbidenNode)
		if(_forbidenNode->origNode)
		if(_forbidenNode->origNode == visitedNLSs[i]->down || _forbidenNode->origNode == visitedNLSs[i]->up)
			continue;

		ls.Set(*visitedNLSs[i]);

		if(F::INTERSECTION::LineSegmentIntersection(_ls, ls))
			return true;
	}

	CFOVNode **fovNodes;
	int fovNodesNumber;

	for(list<CNodeLineSegment*>::iterator iter = intersectedNLSs.begin(); iter!=intersectedNLSs.end(); iter++)
	{
		if((*iter)->polygon == forbidenObstacle)
			continue;

		if((*iter) == NLS)
			continue;

		fovNodesNumber = DAT->GetFOVNodesFromNLS(*iter, &fovNodes);

		if(!fovNodesNumber)
			continue;

		for(int j=0; j<fovNodesNumber; j++)
		{
			if(fovNodes[j]->tagged)
				continue;

			CFOVNode *otherFNode = F::NODE::GetAnyConnection(fovNodes[j]);

			if(otherFNode)
			{
				if(_forbidenNode)
				if(_forbidenNode == otherFNode)
					continue;

				if(otherFNode->origNLS)
				otherFNode->tagged = true;

				ls.Set(otherFNode->position, fovNodes[j]->position);

				if(F::INTERSECTION::LineSegmentIntersection(_ls, ls))
				{
					UntagNLSFOVNodes(*iter);
					return true;
				}
			}
		}

		UntagNLSFOVNodes(*iter);
	}

	return false;
}

bool CFieldOfView::FOVNodeVisible( CFOVNode *_FOVnode )
{
	if(_FOVnode->origNLS)
		return NLSFOVNodeVisible(_FOVnode);
	else
		return NodeVisible(_FOVnode->origNode);
}

bool CFieldOfView::NLSFOVNodeVisibleCircleRadiusOffset( CFOVNode *_FOVNode, double _offset )
{
	CCircle offsetC = *FOVCircle;
	offsetC.r += _offset;

	if(F::INTERSECTION::PointInCircle(_FOVNode->position, offsetC))
		return !U_FAG->LineSegmentCollisionWithObstacles_ForbiddenNLSs( CLineSegment(FOVCircle->position, _FOVNode->position), _FOVNode->origNLS);
}

void CFieldOfView::UpdateOrigNodeToOrigNodeConnections( const vector<CPath*> &paths )
{
	CLineSegment ls;

	int ncs;
	for(int i=0; i<origFOVNodes.size(); i++)
	{
		for(int j=i+1; j<origFOVNodes.size(); j++)
		{

			ncs = DAT->NodeConnectability(origFOVNodes[i], origFOVNodes[j]);

			if(ncs == CFOVNDAT::NCS::NO || ncs == CFOVNDAT::NCS::NOT_TESTED )
			{
				if(ncs==CFOVNDAT::NCS::NOT_TESTED)
				{
					if(F::NODE::NodesConnected(origFOVNodes[i], origFOVNodes[j]))
					{
						DAT->SetNodesConnectStatus(origFOVNodes[i], origFOVNodes[j], CFOVNDAT::NCS::YES);
						F::NODE::MutualyAddFOVNodesAsNeighbours(origFOVNodes[i], origFOVNodes[j]);
						continue;
					}

					DAT->InitializeLSV(origFOVNodes[i], origFOVNodes[j]);
				}

				if(DAT->GetNodesLSV(origFOVNodes[i], origFOVNodes[j])->TestUpdate(paths))
				{
					DAT->EraseLSV(origFOVNodes[i], origFOVNodes[j]);
					DAT->SetNodesConnectStatus(origFOVNodes[i], origFOVNodes[j], CFOVNDAT::NCS::YES);
					F::NODE::MutualyAddFOVNodesAsNeighbours(origFOVNodes[i], origFOVNodes[j]);
				}
				else
					DAT->SetNodesConnectStatus(origFOVNodes[i], origFOVNodes[j], CFOVNDAT::NCS::NO);
			}
		}
	}
}

void CFieldOfView::UpdateOrigNodeToNLSNodeConnections( const vector<CPath*> &paths )
{
	CFOVNode* current;
	for(int i=0; i< edgdeFOVNodes.size(); i++)
	{
		current = edgdeFOVNodes[i];

		if(current->origNode)
			continue;

		if(!current->OrigNodeConnectionDAT)
			current->OrigNodeConnectionDAT = new CNLSFOVNDAT(current, this);

		current->OrigNodeConnectionDAT->UpdateConnections(paths);
		current->OrigNodeConnectionDAT->MutualyAddNeighbours();
	}
}

void CFieldOfView::UpdateFOVNodesConnections( const vector<CPath*> &paths )
{
	ClearConnectionsToNLSNodes();
	UpdateOrigNodeToOrigNodeConnections(paths);
	UpdateOrigNodeToNLSNodeConnections(paths);
}

void CFieldOfView::ClearConnectionsToNLSNodes()
{
	for(int i=0; i<origFOVNodes.size(); i++)
		origFOVNodes[i]->NLSNeighbours.clear();

	for(int i=0; i<edgdeFOVNodes.size(); i++)
	{
		if(edgdeFOVNodes[i]->origNode)
			continue;

		edgdeFOVNodes[i]->origNeighbours.clear();
	}
}

void CFieldOfView::DrawNodeConnections()
{
	glLineWidth(1);
	glColor4f(0,1,1,0.7);
	CLineSegment ls;
	for(int i=0; i<origFOVNodes.size(); i++)
		origFOVNodes[i]->DrawConnections();

	for(int i=0; i<edgdeFOVNodes.size(); i++)
	{
		if(edgdeFOVNodes[i]->origNode)
			continue;

		edgdeFOVNodes[i]->DrawConnections();
	}
}

void CFieldOfView::Draw()
{
	glColor4f(1,0,0,0.7);
	for(int i=0; i<edgdeFOVNodes.size(); i++)
		CPCShapes::DrawShape(CPCShapes::triangle50, edgdeFOVNodes[i]->position);

	DrawVisibleEdges();


	glColor4f(1,0,0,0.4);
	for(int i=0; i<obstacles.size(); i++)
		obstacles[i]->DrawBody();

	//DrawNodeConnections();
}

void CFieldOfView::DrawVisibleEdges()
{
	glLineWidth(2);
	glColor3f(0,1,1);
	
	for(int i=0; i<visitedNLSs.size(); i++)
		Draw::Line(visitedNLSs[i]->down->position, visitedNLSs[i]->up->position);
	
	CFOVNode **fovNodes;
	int fovNodesNumber;

	for(list<CNodeLineSegment*>::iterator iter = intersectedNLSs.begin(); iter!=intersectedNLSs.end(); iter++)
	{
		fovNodesNumber = DAT->GetFOVNodesFromNLS(*iter, &fovNodes);

		if(!fovNodesNumber)
			continue;

		for(int j=0; j<fovNodesNumber; j++)
		{
			if(fovNodes[j]->tagged)
				continue;

			CFOVNode *otherFNode = F::NODE::GetAnyConnection(fovNodes[j]);

			if(otherFNode)
			{
				if(otherFNode->origNLS)
					otherFNode->tagged = true;

				Draw::Line(otherFNode->position, fovNodes[j]->position);
			}
		}

		UntagNLSFOVNodes(*iter);
	}

	glLineWidth(1);
}

void CFieldOfView::DetermineWholeObstacles()
{
	CFOVNode* current, *start;

	for(int i=0; i<obstacles.size(); i++)
		TaggObstacleFOVNodes(*obstacles[i]);
	
	for(int i=0; i<origFOVNodes.size(); i++)
	{
		current = origFOVNodes[i];

		if(current->tagged)
			continue;

		start = current;
		bool closed = false;

		while(1)
		{
			current->tagged = true;
			current = current->next;

			if(!current)
				break;

			if(current == start)
			{
				closed = true;
				break;
			}

			if(current->tagged || current->origNLS)
				break;
		}

		if(closed)
		{
			if(current->origNode)
			if(current->origNode->polygon)
			obstacles.push_back(current->origNode->polygon);
		}
	}

	F::MISC::UntagPointerContainer(origFOVNodes);
}

void CFieldOfView::TaggObstacleFOVNodes( CObstacle& _obstacle )
{
	 CNode *currentNode,*firstNode;
     currentNode = firstNode = _obstacle.nodeLines[0].up;

	do 
	{
		DAT->GetFOVNodeFromNode(currentNode)->tagged = true;
		currentNode = currentNode->next;
	}while(currentNode != firstNode);
}

bool CFieldOfView::PointInVisibleObstacles( CVector &_point )
{
	CObstacle* obstacle = U_FAG->PointInObstacle(_point);

	if(obstacle)
	{
		for(int i=0; i<obstacles.size(); i++)
		{
			if(obstacles[i] == obstacle)
				return true;
		}
	}

	return false;
}

CNodeLineSegment* CFieldOfView::GetLineSegmentIntersectsVisibleObstaclesClosestToPointB( const CLineSegment &_ls )
{
	if(!U_FAG->LineSegmentCollisionWithObstacles(_ls))
		return 0;

	vector<CNearestCandidate> collisionPointNLSs;

	CVector interPoint;
	CLineSegment ls;
	for(int i=0; i<visitedNLSs.size(); i++)
	{
		ls.Set(*visitedNLSs[i]);

		if(F::INTERSECTION::LineSegmentIntersection(_ls, ls, &interPoint))
			collisionPointNLSs.push_back(CNearestCandidate(0, F::DISTANCE::GetDistanceSq(interPoint, _ls.b), visitedNLSs[i]));
	}

	CFOVNode **fovNodes;
	int fovNodesNumber;

	for(list<CNodeLineSegment*>::iterator iter = intersectedNLSs.begin(); iter!=intersectedNLSs.end(); iter++)
	{
		fovNodesNumber = DAT->GetFOVNodesFromNLS(*iter, &fovNodes);

		if(!fovNodesNumber)
			continue;

		for(int j=0; j<fovNodesNumber; j++)
		{
			if(fovNodes[j]->tagged)
				continue;

			CFOVNode *otherFNode = F::NODE::GetAnyConnection(fovNodes[j]);

			if(otherFNode)
			{
				if(otherFNode->origNLS)
					otherFNode->tagged = true;

				ls.Set(otherFNode->position, fovNodes[j]->position);

				if(F::INTERSECTION::LineSegmentIntersection(_ls, ls, &interPoint))
					collisionPointNLSs.push_back(CNearestCandidate(0, F::DISTANCE::GetDistanceSq(interPoint, _ls.b), *iter));
			}
		}

		UntagNLSFOVNodes(*iter);
	}

	if(collisionPointNLSs.size())
	{
		CNearestCandidate* nearest;
		nearest =  F::MISC::GetSmallestValueCandidate(collisionPointNLSs);
		return (CNodeLineSegment*)(nearest->data);
	}

	return 0;
}
