#pragma once
#include "Camera.h"
#include "ConcaveHullGenerator.h"
#include "Terrain.h"
#include "FastAccesGrid.h"
#include "ConsciousEntityManager.h"
#include "Square.h"

#include "LineSegmentVisibility.h"
using namespace std;

class CSimulation
{
	CSimulation(int _mapWidth, int _mapHeight);
	void PostConstructionSetup();
	bool run;
	
	//////////////////////
	vector<CFAGTile*> fagLineTiles;
	CConsciousEntityManager *CEM;
	///////////
	vector<CVector> path;
	
	CLineSegmentVisibility *LSV;

	CCamera camera;
	CCircle fovCirc;
	CSquare simulationArea;

	CLineSegment lstemp;

	static int screenWidth, screenHeight, screenBPP;
public:
	CVector startPos, endPos;
	CNode* endNode;
	CNode* startNode;

	int DELAY;
	int mapWidth, mapHeight;
	double simAreaDiagonal;
	static char *writeFileName;

	static CSimulation *simulation;

	CTerrain *terrain;
	CFastAccesGrid *FAG;
	~CSimulation();
	
	////////////////////////////////
	void SetPathStartEndEvents();
	//////////////////////////////////

	
	void Run();
	void HandleEvents();
	void Logic();
	void Draw();
	void UpdateCamera();
	CVector GetWorldCoordinate(int _x, int _y);

	static void InitGraphix();
	static void CreateSimulation(int _mapWidth, int _mapHeight);
	static void SetWindow(int _screenW, int _screenH, int _screenBPP);
	static CSimulation* Instance();
};

