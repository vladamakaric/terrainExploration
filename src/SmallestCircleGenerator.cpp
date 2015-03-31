#include "SmallestCircleGenerator.h"
#include "f_distance.h"
#include "f_misc.h"
#include "f_angle.h"
#include <iostream>

using namespace std;

vector<CNearestCandidate> CSmallestCircleGenerator::pointsInfo;
vector<CVector*> *CSmallestCircleGenerator::convexHullPoints;
int CSmallestCircleGenerator::l1;
int	CSmallestCircleGenerator::l2; 
int	CSmallestCircleGenerator::v;
bool CSmallestCircleGenerator::diametrical;


void CSmallestCircleGenerator::Generate(vector<CVector*> *_chull, CCircle &_c)
{
	diametrical = false;
	convexHullPoints = _chull;
	l1 = 0;
	l2 = 1;
	TryLine();
	
	if(diametrical)
	{
		_c.position = (*(*convexHullPoints)[l1] + *(*convexHullPoints)[l2])/2;
		_c.r = F::DISTANCE::GetDistance( *(*convexHullPoints)[l1], *(*convexHullPoints)[l2])/2;
	}
	else
	//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	F::MISC::CalculateCircleFrom3Points(*(*convexHullPoints)[l1], *(*convexHullPoints)[l2], *(*convexHullPoints)[v], _c);
}

void CSmallestCircleGenerator::GeneratePointInfo()
{
	pointsInfo.clear();
	double baseLenght = F::DISTANCE::GetDistance(*(*convexHullPoints)[l1], *(*convexHullPoints)[l2]);
	double side2, side1;
	double subAngle;

	for(int i =0; i < convexHullPoints->size(); i++ )
	{
		if( i == l1 || i == l2 )
			continue;

		side1 = F::DISTANCE::GetDistance(*(*convexHullPoints)[i], *(*convexHullPoints)[l1]);
		side2 = F::DISTANCE::GetDistance(*(*convexHullPoints)[i], *(*convexHullPoints)[l2]);
		subAngle = F::ANGLE::GetAngleBetweenTwoSidesOfATriangle(side1,side2,baseLenght);

		pointsInfo.push_back(CNearestCandidate( i, subAngle));
	}
}

bool CSmallestCircleGenerator::AnyPointAnglesObtuse()
{
	double vLenght = F::DISTANCE::GetDistance(*(*convexHullPoints)[l1], *(*convexHullPoints)[l2]);
	double side2, side1;

	side1 = F::DISTANCE::GetDistance(*(*convexHullPoints)[v], *(*convexHullPoints)[l1]);
	side2 = F::DISTANCE::GetDistance(*(*convexHullPoints)[v], *(*convexHullPoints)[l2]);

	//subAngle = F::GetAngleBetweenTwoLines(side1,side2,baseLenght);
	//double angleV = F::GetAngleBetweenTwoLines(side1, side2, vLenght);
	double angleL1 = F::ANGLE::GetAngleBetweenTwoSidesOfATriangle(vLenght, side1, side2);
	double angleL2 = F::ANGLE::GetAngleBetweenTwoSidesOfATriangle(vLenght, side2, side1);

	if(angleL1 >= PI/2)
	{
		l1 = v;
		return true;
	}
	else if(angleL2 >= PI/2)
	{
		l2 = v;
		return true;
	}
	
	return false;
}

void CSmallestCircleGenerator::TryLine()
{
	GeneratePointInfo();
	CNearestCandidate* smallest = F::MISC::GetSmallestValueCandidate(pointsInfo);

	v = smallest->index;

	if(smallest->value >= PI/2)
	{
		diametrical = true;
		return;
	}
	
	if(!AnyPointAnglesObtuse())
	{
		diametrical = false;
		return;
	}
	else
		TryLine();
}