#include "AIS_GoToUnseenPosition.h"
#include "f_vector.h"
#include "f_global.h"
#include "f_misc.h"
#include "f_distance.h"
#include "f_intersection.h"
#include "f_node.h"
#include "draw.h"
#include "FollowNode.h"
#include "FollowNLS.h"
#include "simulationDefinitions.h"
#include "LineSegmentVisibility.h"
#include "AIB_ExploreNewTerritory.h"
#include "ShortestPathGenerator.h"

CAIS_GoToUnseenPosition::CAIS_GoToUnseenPosition(const CVector &_position,CAIBehaviour* _behaviour) 
{
	directionFree = false;
	behaviour = _behaviour;  
	USPosition = CVector(-1,-23);//bzvz
	follow = 0;
	moveAlongPath = new CMoveAlongPath(this->behaviour->CE, &path);
	USPNManager.Initialize(behaviour->CE->FOV);
}

bool CAIS_GoToUnseenPosition::Run()
{
	CEPosFNUpdated = false;
	moveAlongPath->path = &path;
	moveAlongPath->CE = behaviour->CE;

	if(F::GLOBAL::globalGoal)
	{
		if(USPosition != *F::GLOBAL::globalGoal)
		{
			USPosition = *F::GLOBAL::globalGoal;

			if(behaviour->CE->currentPath.firstPoint)
			behaviour->CE->changeVelocity = true;

			USPositionFN.position = USPosition;
			USPNManager.InitializeNewUSPN(&USPositionFN);
		}
	}
	else return true;
	
	direction = F::VECTOR::GetVectorFromAToB(behaviour->CE->positionViewRadius.position, USPosition).Direction();
	behaviour->CE->UpdateFOV();
	USPNManager.UpdateConnectivity();

	if(behaviour->CE->FOV->PointInVisibleObstacles(USPosition))
	{
		delete F::GLOBAL::globalGoal;
		F::GLOBAL::globalGoal = 0;
		return true;
	}

	if(F::DISTANCE::GetDistanceSq(behaviour->CE->positionViewRadius.position, USPosition) < behaviour->CE->speed*behaviour->CE->speed )
	{
		directionFree = false;
		if(follow) delete follow;

		follow = 0;
		behaviour->CE->changeVelocity = true;
		behaviour->CE->velocity = CVector(0,0);
		delete F::GLOBAL::globalGoal;
		F::GLOBAL::globalGoal = 0;
		return true;
	}

	if(DirectionFree())
	{
		ClearFollow();
		behaviour->CE->velocity = direction;

		if(!directionFree)
		{
			behaviour->CE->changeVelocity = true;
			directionFree = true;
		}

		return true;
	}

	directionFree = false;

	UpdateFollow();
	moveAlongPath->Update();
	lenghtToGoal = moveAlongPath->GetLenghtToGoal();

	return true;
}

bool CAIS_GoToUnseenPosition::DirectionFree()
{
	CLineSegment ls(USPosition, behaviour->CE->positionViewRadius.position);
	return !behaviour->CE->FOV->LineSegmentIntersectsVisibleObstacles(ls);
}


void CAIS_GoToUnseenPosition::UpdateTempCEPosFNWithoutLSITest(CFollow* _follow)
{
	CFieldOfView* FOV = behaviour->CE->FOV;

	int origFOVNodesSize = FOV->origFOVNodes.size();
	int allFOVNodesSize = origFOVNodesSize + FOV->edgdeFOVNodes.size();

	CFOVNode* current;
	for(int i=0; i<allFOVNodesSize; i++)
	{
		if(i<origFOVNodesSize)
			current = FOV->origFOVNodes[i];
		else
		{
			current = FOV->edgdeFOVNodes[i-origFOVNodesSize];
			if(current->origNode)
				continue;
		}

		if(_follow)
		if(_follow->DiscardFNode(current))
			continue;

		AddNeighbourToCEPositionFN(current, false);
	}
}


void CAIS_GoToUnseenPosition::SetUpTempNodes( CFollow* _follow)
{
	CEPosFN.ClearNeighbours();
	CEPosFN.position = behaviour->CE->positionViewRadius.position;

	CFieldOfView* FOV = behaviour->CE->FOV;

	int origFOVNodesSize = FOV->origFOVNodes.size();
	int allFOVNodesSize = origFOVNodesSize + FOV->edgdeFOVNodes.size();

	CLineSegment toGoalFromCE(behaviour->CE->positionViewRadius.position, USPosition);
	CNodeLineSegment* nearestToGoal = FOV->GetLineSegmentIntersectsVisibleObstaclesClosestToPointB(toGoalFromCE);
	CObstacle* obst = 0; 
	if(nearestToGoal)
		obst = nearestToGoal->polygon;

	CFOVNode* current;
	for(int i=0; i<allFOVNodesSize; i++)
	{
		if(i<origFOVNodesSize)
			current = FOV->origFOVNodes[i];
		else
		{
			current = FOV->edgdeFOVNodes[i-origFOVNodesSize];
			if(current->origNode)
				continue;
		}

		if(_follow)
		if(_follow->DiscardFNode(current))
			continue;

		AddNeighbourToCEPositionFN(current);
		USPNManager.ConnectNodeToUSPNode(current,true, obst);
	}


	if(!USPNManager.connectedToUSPNNodes.size())
	{
		for(int i=0; i<allFOVNodesSize; i++)
		{
			if(i<origFOVNodesSize)
				current = FOV->origFOVNodes[i];
			else
			{
				current = FOV->edgdeFOVNodes[i-origFOVNodesSize];
				if(current->origNode)
					continue;
			}

			if(_follow)
				if(_follow->DiscardFNode(current))
					continue;

			USPNManager.ConnectNodeToUSPNode(current);
		}
	}
}

void CAIS_GoToUnseenPosition::ClearFollow()
{
	if(follow)
	{
		delete follow;
		follow =0;
	}
}

void CAIS_GoToUnseenPosition::UpdateFollow()
{
	if(follow)
	{
		follow->Update();


		if(follow->FollowHasChanged())
			ChangeFollow();
	}

	if(!follow) CreateNewFollow();
}

void CAIS_GoToUnseenPosition::CreateNewFollow()
{
	behaviour->CE->UpdateFOVConnections();

	SetUpTempNodes();
	
	if(CEPosFNNotConnected())
		UpdateTempCEPosFNWithoutLSITest();

	path.clear();
	vector<CPathFindingNode*> nodesPath;
	CShortestPathGenerator::Generate(&CEPosFN, &USPositionFN, nodesPath);
	CShortestPathGenerator::SavePathGValues(nodesPath, moveAlongPath->pathGValues);
	ClearPathGenerationVariables();
	F::MISC::PathFindingNodePathToVectorPath(nodesPath, path);
	moveAlongPath->path = &path;
	moveAlongPath->lenght = CShortestPathGenerator::pathLenght;
	moveAlongPath->Reset();

	CFOVNode* lastFOVNInPath = dynamic_cast<CFOVNode*>(nodesPath[nodesPath.size()-2]);
	
	if(lastFOVNInPath->origNode)
		follow = new CFollowNode(lastFOVNInPath, this);
	else
		follow = new CFollowNLS(lastFOVNInPath, this);
}

void CAIS_GoToUnseenPosition::ChangeFollow()
{
	CFOVNode *fnodeToFollow = follow->lastPathFN;
	delete follow;

	if(fnodeToFollow->origNode)
		follow = new CFollowNode(fnodeToFollow, this);
	else
		follow = new CFollowNLS(fnodeToFollow, this);
}

void CAIS_GoToUnseenPosition::ClearPathGenerationVariables()
{
	CShortestPathGenerator::Clean();
	USPNManager.DisconectNodesFromUSPNode();
}

void CAIS_GoToUnseenPosition::AddNeighbourToCEPositionFN( CFOVNode *_fnode, bool _LSITest )
{
	bool visible = false;
	CLineSegment ls(_fnode->position, CEPosFN.position);

	if(_fnode->origNode)
		visible = !U_FAG->LineSegmentCollisionWithObstacles_ForbiddenNodes(ls, _fnode->origNode);
	else
		visible = !U_FAG->LineSegmentCollisionWithObstacles_ForbiddenNLSs(ls, _fnode->origNLS);

	if(visible)
	{
		bool visible2 = false;

		if(_LSITest)
		{
			if(F::INTERSECTION::PointInCircleWithRadiusOffset(_fnode->position, behaviour->CE->positionViewRadius, 1))
				visible2 = true;

			if(!visible2)
				if(CLineSegmentVisibility::Test(ls, behaviour->CE->paths))
					visible2 = true;
		}
		else visible2 = true;

		if(visible2)
		{
			if(_fnode->origNode)
				CEPosFN.origNeighbours.push_back(_fnode);
			else
				CEPosFN.NLSNeighbours.push_back(_fnode);
		}
	}
}

void CAIS_GoToUnseenPosition::Draw()
{
	CLineSegment ls(direction*behaviour->CE->positionViewRadius.r + behaviour->CE->positionViewRadius.position, behaviour->CE->positionViewRadius.position);

	ls.Draw();

	if(F::GLOBAL::globalGoal)
	{
		glColor3f(1,0,1);
		Draw::CircleFill(USPosition, 100,100);
	}

	Draw::VectorObjectContainerAsOpenLine(path);

	if(follow)
		follow->Draw();
}

bool CAIS_GoToUnseenPosition::CEPosFNNotConnected()
{
	return !CEPosFN.origNeighbours.size() && !CEPosFN.NLSNeighbours.size();
}
