#pragma once
#include <vector>
#include "Vector.h"
#include "NearestCandidate.h"
#include "Circle.h"

using namespace std;

class CSmallestCirclePointInfo
{
public:
	CSmallestCirclePointInfo(double _sa, int _indx) : subAngle(_sa), indx(_indx) {}
	double subAngle;
	int indx; 
};

class CSmallestCircleGenerator
{
public:
	static bool diametrical;
	static void Generate(vector<CVector*> *_chull, CCircle &_c);
	static void TryLine();
	static void GeneratePointInfo();
	static vector<CVector*> *convexHullPoints;
	static vector<CNearestCandidate> pointsInfo;
	static bool AnyPointAnglesObtuse();
	static int l1, l2, v;
};