#include "Terrain.h"
#include "Vector.h"
#include "ConcaveHullGenerator.h"
#include <stdio.h>
#include "draw.h"
#include "NearestCandidate.h"
#include <algorithm>
#include <iostream>
#include "Simulation.h"
#include "f_intersection.h"
#include "f_distance.h"
#include <stdlib.h>
#include "simulationDefinitions.h"
#include "f_node.h"

CTerrain::CTerrain(int _clusterNum, int percentageOfLinks, int _HR1, int _HR2, int _minDistBetweenC, int _minDistBetweenCP, int _chainClusterIntersection, int _minDistBetweenCM, int _maxChainLinks, double _numOfCPKoef, int _k , int _levelWidth, int _levelHeight)
{
	DebugMode = false;

	pointClusters = new vector<CPointCluster*>();
	CVector randVec;
	bool chain=false;

	CPointCluster* lastAddedCluster;
	int chainMemberNumber=1;

	int numOfSingleClusterChainTries=0;
	for(int i=0; i < _clusterNum; i++ )
	{
nest:
		if(pointClusters->size())
		{
			chain=false;
			if(rand()%100>(100-percentageOfLinks)-1)
			{
				chain=true;
				if((!lastAddedCluster->previous && numOfSingleClusterChainTries>2) || chainMemberNumber>=_maxChainLinks)
				{
					CPointCluster* tempCluster = GetSingleCluster();
					if(tempCluster)
					{
						chainMemberNumber = 1;
						lastAddedCluster =  tempCluster;
					}
					else
						if(chainMemberNumber>=_maxChainLinks)
							chain=false;
				}
			}
		}

		int hRadius = _HR1 + rand()%(_HR2 - _HR1);

		if(!chain)
		{
			for(unsigned int j=0; j<20; j++)
			{
				randVec.Set(rand()%(_levelWidth-hRadius*2 - _minDistBetweenC*2) + hRadius + _minDistBetweenC, rand()%(_levelHeight-hRadius*2 - _minDistBetweenC*2) + hRadius + _minDistBetweenC);
				if(IsClusterGood(randVec,hRadius, _minDistBetweenC))
				{
					CPointCluster *temp = new CPointCluster(randVec, hRadius, int(hRadius*_numOfCPKoef), _minDistBetweenCP);

					if(temp->IsInvalid())
					{
						delete temp;
						break;
					}

					pointClusters->push_back(temp);
					lastAddedCluster = (*pointClusters)[pointClusters->size()-1];
					chainMemberNumber = 1;
					numOfSingleClusterChainTries = 0;
					break;
				}
			}
		}
		else
		{
			double distance = lastAddedCluster->hRadius + hRadius - _chainClusterIntersection;
			double xStep = distance/5; 

			bool a[6]={0,0,0,0,0};

			for(unsigned int j=0; j<6; j++)
			{
				int xSign=1,ySign=1;
				if(rand()%10>5) xSign=-1;
				if(rand()%10<5) ySign=-1;

				int koefXStep=rand()%6;

				while(1)
				{
					if(!a[koefXStep])
						break;

					koefXStep++;
					if(koefXStep>5)
						koefXStep=0;
				}

				a[koefXStep]=1;

				double x = koefXStep*xStep;
				double y = sqrt(double(distance*distance - x*x));
				randVec.Set(x*xSign, y*ySign); 

				CVector randVecTemp = randVec;
				for(unsigned int k=0; k< 4; k++)
				{
					randVec = randVecTemp;

					if(k==1) randVec.x *= -1;
					else
						if(k==2) randVec.y *= -1;
						else
							if(k==3) { randVec.x *= -1; randVec.y *= -1; }

							CVector updatedRand(randVec + lastAddedCluster->position);

							if(!F::INTERSECTION::CircleInSquare(CCircle(updatedRand, hRadius), CSquare(CVector(0+_minDistBetweenC,0+_minDistBetweenC),CVector(_levelWidth-_minDistBetweenC, _levelHeight-_minDistBetweenC))))
								continue;

							if(IsClusterGood(updatedRand, hRadius, _minDistBetweenC, lastAddedCluster, _minDistBetweenCM))
							{

								CPointCluster *temp = new CPointCluster(updatedRand, hRadius, int(hRadius*_numOfCPKoef), _minDistBetweenCP);

								if(temp->IsInvalid())
								{
									delete temp;
									i++;////////////
									goto nest;
								}

								lastAddedCluster->next = temp;
								lastAddedCluster->next->previous = lastAddedCluster;
								lastAddedCluster = lastAddedCluster->next;
								numOfSingleClusterChainTries = 0;
								chainMemberNumber++;
								i++;/////////////
								goto nest;
							}
				}
			}

			numOfSingleClusterChainTries++;
		}
	}

	RemovePointsFromClusterChains(_minDistBetweenCP);

	for(unsigned int i=0; i < pointClusters->size(); i++)
		obstacles.push_back(CObstacle());

	CConcaveHullGenerator* CHG = new CConcaveHullGenerator();
	for(unsigned int i=0; i < pointClusters->size(); i++)
	{
		CHG->Generate((*pointClusters)[i], obstacles[i].pointsVector , _k);
	}
	delete CHG;

	OptimiseObstacles();
}

void CTerrain::PostConstructionSetup()
{
	GenerateLevelEdges();
	CountNodesAndAssignIndicesToNodesAndNLSs();
}

bool CTerrain::IsClusterGood(const CVector &_position, int _HR, int _minDistBetweenC, CPointCluster* _parrentCluster, int _minDistBetweenCM)
{
	if(!pointClusters->size())
		return true;

	for(unsigned int i=0; i < pointClusters->size(); i++)
	{
		CPointCluster* nextC = (*pointClusters)[i];

		if(_parrentCluster)
		{
			if(nextC->ID == _parrentCluster->ID)
				continue;

			if(IsInSameChain(nextC,_parrentCluster))
			{
				do
				{
					if(nextC->ID == _parrentCluster->ID)
						break;

					double _CenterDistance = (_position - nextC->position).Length();

					if(_CenterDistance < nextC->hRadius + _HR + _minDistBetweenCM)
						return false;

				}while(nextC = nextC->next); 

				continue;
			}
		}

		do
		{
			double _CenterDistance = (_position - nextC->position).Length();

			if(_CenterDistance < nextC->hRadius + _HR + _minDistBetweenC)
				return false;

		}while(nextC = nextC->next);
	}

	return true;
}

void CTerrain::Draw()
{
	for(unsigned int i=0; i < obstacles.size(); i++)
		obstacles[i].Draw();
}

bool CTerrain::IsInSameChain(CPointCluster* _possibleChainMember, CPointCluster* _parrentCluster)
{
	CPointCluster* nextPC=_possibleChainMember;
	do
	{
		if(nextPC->ID == _parrentCluster->ID)
			return true;

	}while(nextPC = nextPC->next);

	return false;
}

CPointCluster* CTerrain::GetSingleCluster()
{
	vector<int> availableClusters;
	for(unsigned int i=0; i < pointClusters->size(); i++ )
	{
		if(!(*pointClusters)[i]->next)
			availableClusters.push_back(i);
	}

	if(!availableClusters.size())
		return 0;

	return  (*pointClusters)[rand()%availableClusters.size()];
}

void CTerrain::RemovePointsFromClusterChains(int _minDist)
{
	for(int i=0; i < pointClusters->size(); i++)
	{
		RemovePointsFromClusterChain((*pointClusters)[i], _minDist);
	}
}

void CTerrain::RemovePointsFromClusterChain(CPointCluster* _first, int _minDist)
{
	if(!_first->next)
		return;

	for(int i=0; i < _first->points->size(); i++)
	{
		for(int j=0; j < _first->next->points->size(); j++)
		{
			double distance = (*(*_first->points)[i] - *(*_first->next->points)[j]).Length();

			if(distance<_minDist)
			{
				_first->points->erase(_first->points->begin() + i);
				i--;
				break;
			}
		}
	}
}

void CTerrain::OptimiseObstacles()
{
	for(unsigned int i=0; i < obstacles.size(); i++)
		obstacles[i].OptimiseObstacle();
}

void CTerrain::DegugModeToggle()
{
	if( event.type == SDL_KEYDOWN )
	{

	}
}

CObstacle* CTerrain::PointInObstacles(CVector &_p)
{
	for(unsigned int i=0; i < obstacles.size(); i++)
	{
		if(F::INTERSECTION::PointInCircle(_p, obstacles[i].boundingCircle))
		{
			if(obstacles[i].polygon->PointInside(_p))
				return &obstacles[i];
		}
	}

	return 0;
}

void CTerrain::DetermineNodeConnectability()
{
	nodeConnectability = new bool *[nodeNum];
	nodeConnections = new vector<CNode*>[nodeNum];

	for(int i =0; i < nodeNum; i++)
		nodeConnectability[i] = new bool[nodeNum];
	
	CLineSegment ls;
	for(int i =0; i < nodeNum; i++)
	{
		for(int j = i+1; j < nodeNum; j++)
		{
			bool connectable=F::NODE::NodesConnected(nodes[i], nodes[j]); 
			if(!connectable)
			{
				ls.Set(nodes[i]->position, nodes[j]->position);
				connectable = !U_FAG->LineSegmentCollisionWithObstacles_ForbiddenNodes(ls, nodes[i], nodes[j]);
			}

			nodeConnectability[i][j] = connectable;
			nodeConnectability[j][i] = connectable;
		}
	}

	for(int i =0; i < nodeNum; i++)
	{
		for(int j =0; j < nodeNum; j++)
		{
			if(j==i)
				continue;

			if(nodeConnectability[i][j])
				nodeConnections[i].push_back(nodes[j]);
		}
	}
}

void CTerrain::CountNodesAndAssignIndicesToNodesAndNLSs()
{
	nodeNum = 4;
	CNode* currentNode;
	CNode* firstNode;

	for(int i=0; i<obstacles.size(); i++)
	{
		currentNode = firstNode = obstacles[i].nodeLines[0].up;
		
		do 
		{
			currentNode->terrainIndx = nodeNum;
			currentNode->polygon = &obstacles[i];
			nodeNum++;
			currentNode = currentNode->next;
		}while(currentNode != firstNode);
	}
	
	nodes = new CNode*[nodeNum];

	for(int i=0; i<obstacles.size(); i++)
	{
		currentNode = firstNode = obstacles[i].nodeLines[0].up;

		do 
		{
			nodes[currentNode->terrainIndx] = currentNode;
			currentNode = currentNode->next;
		}while(currentNode != firstNode);
	}

	int NLSNum=4; 
	CNodeLineSegment *current;
	CNodeLineSegment *start;
	nodeLineSegments = new CNodeLineSegment *[nodeNum];

	for(int i=0; i<obstacles.size(); i++)
	{
		current = start = obstacles[i].nodeLines;

		do 
		{
			nodeLineSegments[NLSNum] = current;
			current->polygon = &obstacles[i];
			current->terrainIndx = NLSNum;

			NLSNum++;
			current = current->next;
		}while(current != start);
	}	

	currentNode = firstNode = levelEdges[0]->down;
	do 
	{
		nodes[currentNode->terrainIndx] = currentNode;
		currentNode = currentNode->next;
	}while(currentNode != firstNode);

	current = start = levelEdges[0];
	do 
	{
		nodeLineSegments[current->terrainIndx] = current;
		current = current->next;
	}while(current != start);
}

void CTerrain::GenerateLevelEdges()
{
	double SEO = 1; //simulation edge offset
	CNode* levelEdgeNodes[4];
	levelEdgeNodes[0] = new CNode(CVector(SEO, SEO));
	levelEdgeNodes[1] = new CNode(CVector(CSimulation::simulation->mapWidth-SEO, SEO));
	levelEdgeNodes[2] = new CNode(CVector(CSimulation::simulation->mapWidth-SEO, CSimulation::simulation->mapHeight-SEO));
	levelEdgeNodes[3] = new CNode(CVector(SEO, CSimulation::simulation->mapHeight-SEO));

	levelEdgeNodes[0]->terrainIndx = 0;
	levelEdgeNodes[1]->terrainIndx = 1;
	levelEdgeNodes[2]->terrainIndx = 2;
	levelEdgeNodes[3]->terrainIndx = 3; 

	levelEdgeNodes[0]->polygon = 0;
	levelEdgeNodes[1]->polygon = 0;
	levelEdgeNodes[2]->polygon = 0;
	levelEdgeNodes[3]->polygon = 0; 

	levelEdgeNodes[0]->next = levelEdgeNodes[1];
	levelEdgeNodes[0]->previous = levelEdgeNodes[3];
	levelEdgeNodes[1]->next = levelEdgeNodes[2];
	levelEdgeNodes[1]->previous = levelEdgeNodes[0];
	levelEdgeNodes[2]->next = levelEdgeNodes[3];
	levelEdgeNodes[2]->previous = levelEdgeNodes[1];
	levelEdgeNodes[3]->next = levelEdgeNodes[0];
	levelEdgeNodes[3]->previous = levelEdgeNodes[2];

	levelEdges[0] = new CNodeLineSegment();
	levelEdges[1] = new CNodeLineSegment();
	levelEdges[2] = new CNodeLineSegment();
	levelEdges[3] = new CNodeLineSegment();

	levelEdges[0]->terrainIndx = 0;
	levelEdges[1]->terrainIndx = 1;
	levelEdges[2]->terrainIndx = 2;
	levelEdges[3]->terrainIndx = 3;

	levelEdges[0]->polygon = 0;
	levelEdges[1]->polygon = 0;
	levelEdges[2]->polygon = 0;
	levelEdges[3]->polygon = 0;

	levelEdges[0]->up = levelEdgeNodes[0];
	levelEdges[0]->down = levelEdgeNodes[1];
	levelEdges[1]->down = levelEdgeNodes[1];
	levelEdges[1]->up = levelEdgeNodes[2];
	levelEdges[2]->up = levelEdgeNodes[2];
	levelEdges[2]->down = levelEdgeNodes[3];
	levelEdges[3]->up = levelEdgeNodes[3];
	levelEdges[3]->down = levelEdgeNodes[0];

	levelEdges[0]->left = levelEdgeNodes[0];
	levelEdges[0]->right = levelEdgeNodes[1];
	levelEdges[1]->left = levelEdgeNodes[1];
	levelEdges[1]->right = levelEdgeNodes[2];
	levelEdges[2]->right = levelEdgeNodes[2];
	levelEdges[2]->left = levelEdgeNodes[3];
	levelEdges[3]->right = levelEdgeNodes[3];
	levelEdges[3]->left = levelEdgeNodes[0];

	levelEdges[0]->next = levelEdges[1];
	levelEdges[0]->previous = levelEdges[3];
	levelEdges[1]->next = levelEdges[2];
	levelEdges[1]->previous = levelEdges[0];
	levelEdges[2]->next = levelEdges[3];
	levelEdges[2]->previous = levelEdges[1];
	levelEdges[3]->next = levelEdges[0];
	levelEdges[3]->previous = levelEdges[2];
}

bool CTerrain::NodesConnectable( CNode* _node1, CNode* _node2 )
{
	return nodeConnectability[_node1->terrainIndx][_node2->terrainIndx];
}

bool CTerrain::NodesByIndexConnectable( int _node1Indx, int _node2Indx )
{
	return NodesConnectable(nodes[_node1Indx], nodes[_node2Indx]);
}

void CTerrain::AddNeighboursToTempNodes()
{
	for(int i =0; i < nodeNum; i++)
	{
		for(int j = i+1; j < nodeNum; j++)
		{
			if(nodeConnectability[i][j])
			{
				nodes[i]->tempNode.neighbours.push_back(&nodes[j]->tempNode);
				nodes[j]->tempNode.neighbours.push_back(&nodes[i]->tempNode);
			}
		}
	}
}

CNode* CTerrain::GetNearestNodeToPoint( CVector &point )
{
	double minDist = 9999999999999999, dist;
	CNode * minDistNode;

	for(int i=0; i<nodeNum; i++)
	{
		dist = F::DISTANCE::GetDistanceSq(nodes[i]->position, point);

		if(dist < minDist)
		{
			minDist = dist;
			minDistNode = nodes[i];
		}
	}

	return minDistNode;
}

void CTerrain::DetermineNodesToNLSsConnectability()
{
	nodesToNLSsConnectability = new bool *[nodeNum];
	for(int i =0; i < nodeNum; i++)
		nodesToNLSsConnectability[i] = new bool[nodeNum];

	for(int i=0; i < nodeNum; i++)
		for(int j=0; j < nodeNum; j++)
			nodesToNLSsConnectability[i][j] = false;

	for(int i=0; i<nodeNum; i++)
		DetermineNodeToNLSsConnectability(nodes[i]);
}

void CTerrain::DetermineNodeToNLSsConnectability( CNode* _node )
{
	CNodeLineSegment* NLS;
	for(int i=0; i<nodeNum; i++)
	{
		if(nodes[i] == _node)
			continue;

		if(!NodesConnectable(_node, nodes[i]))
			continue;

		if(!F::NODE::PointToNodeConnectionIsEdge(nodes[i], _node->position))
			continue;

		if(NLS = GetNodeToEdgeNodeInfiniteLineNLSIntersection(_node, nodes[i]))
		{
			if(F::NODE::NodeOnNLS(_node, NLS))
				continue;

			nodesToNLSsConnectability[_node->terrainIndx][NLS->terrainIndx] = true;
		}
	}

	for(int i=0; i<nodeNum; i++)
	{
		if(F::NODE::NodeOnNLS(_node, nodeLineSegments[i]))
			continue;

		if(!nodesToNLSsConnectability[_node->terrainIndx][i])
			nodesToNLSsConnectability[_node->terrainIndx][i] = NodeToNLSConnectableByMiddlePointAndBothNodes(_node, nodeLineSegments[i]);
	}
}

bool CTerrain::NodeToNLSConnectableByMiddlePointAndBothNodes( CNode* _node, CNodeLineSegment* _NLS )
{
	bool connectedToBothNodes = NodesConnectable(_node, _NLS->down) && NodesConnectable(_node, _NLS->up);

	CLineSegment ls;
	if(connectedToBothNodes)
	{
		ls.Set(*_NLS);
		CVector middlePoint = ls.MiddlePoint();

		ls.Set(middlePoint, _node->position);
		return !U_FAG->LineSegmentCollisionWithObstacles_ForbiddenNodeAndNLS(ls, _node, _NLS);
	}

	return false;
}

CNodeLineSegment* CTerrain::GetNodeToEdgeNodeInfiniteLineNLSIntersection( CNode* _node, CNode* _edgeNode )
{
	CVector nodeToEdgeNode = F::VECTOR::GetVectorFromAToB(_node->position, _edgeNode->position).Direction() * U_SIMULATION->simAreaDiagonal;

	CLineSegment ls(_node->position, _node->position + nodeToEdgeNode);
	CLineSegment nlsls;
	for(int i=0; i<4; i++)
	{
		if(_node == levelEdges[i]->down || _node == levelEdges[i]->up)
		continue;

		nlsls.Set(*levelEdges[i]);

		CVector intersection;
		if(F::INTERSECTION::LineSegmentIntersection(ls,nlsls, &intersection))
		{
			ls.b = intersection;
			ls.b += nodeToEdgeNode.Direction()/2;   //da bi mogo da se sudari sa levelEdge ali da ne ispadne iz table
			break;
		}
	}

	CCollisionPoint cp;
	if(U_FAG->GetNearestCollisionWithLineSegment(ls, _node->position, cp, _node, _edgeNode))
		return cp.nodeLine;

	return 0;
}

bool CTerrain::NodeToNLSConnectable( CNode* _node, CNodeLineSegment* _NLS )
{
	return nodesToNLSsConnectability[_node->terrainIndx][_NLS->terrainIndx];
}
