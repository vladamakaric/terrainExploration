#include "Simulation.h"
#include "global.h"
#include "f_vector.h"
#include "f_global.h"
#include "draw.h"
#include "ShortestPathGenerator.h"
#include "PathOptimizer.h"
#include <stdlib.h>
#include "Tesselator.h"
#include "PrecompiledShapes.h"

void Screendump(char *destFile, short W, short H) 
{ 
	FILE   *out = fopen(destFile, "w"); 
	char   *pixel_data = new char[3*W*H]; 
	short  TGAhead[] = {0, 2, 0, 0, 0, 0, W, H, 24}; 
	glReadBuffer(GL_FRONT); 
	glReadPixels(0, 0, W, H, GL_BGR, GL_UNSIGNED_BYTE, pixel_data); 
	fwrite(&TGAhead, sizeof(TGAhead), 1, out); 
	fwrite(pixel_data, 3*W*H, 1, out); fclose(out); 
}

int CSimulation::screenHeight;
int CSimulation::screenWidth;
int CSimulation::screenBPP;
CSimulation *CSimulation::simulation=0;
char *CSimulation::writeFileName=0;

using namespace std;

CSimulation::CSimulation(int _mapWidth, int _mapHeight)
{

	lstemp.Set(CVector(1000,1000), CVector(13000,17000));
	LSV = new CLineSegmentVisibility(lstemp);

	mapWidth = _mapWidth;
	mapHeight = _mapHeight;
	simAreaDiagonal = sqrt(double(mapHeight*mapHeight + mapWidth*mapWidth));

	simulationArea.Set(CVector(0,0), CVector(mapWidth, mapHeight));

	endNode = startNode = 0; 
	DELAY = 1;
	run = true;
	camera.Init(0,0,70,10);

	startPos.x = -100;
	//////////////////
	printf("dali zelite da ucitate teren iz fajla ili zelite da se avtomatski generise? (1:0): ");
	int readFromFile;
	scanf ("%d",&readFromFile);
	char *readFile=0;

	if(readFromFile)
	{
		readFile = new char[100];
		printf("\n kako se zove fajl iz kojeg ocete da ucitate teren?: ");
		scanf("%s", readFile);
	}
	else
	{
		
		int writeToFile;
		printf("\n dali zelite da se avtomatski generisan teren zapampti u fajl?: (1:0) ");
		scanf("%d", &writeToFile);

		if(writeToFile)
		{
			writeFileName = new char[100];
			printf("\n kako zelite da se zove fajl u koji ce biti zapisan avtomarski generisan teren?: ");
			scanf("%s", writeFileName);
		}
	}
	//////////////////////

	CSimulation::SetWindow(800,800,24);

	if(readFromFile)
		terrain = new CTerrain(readFile);
	else
		terrain = new CTerrain(1000, 96, 770, 1040, 1000, 700, 60 , 400, 15, 1.0/20.0, 8 , _mapWidth, _mapHeight); //Velicina talbe d0 20 000

	CEM = 0;
	CPCShapes::Initialize();
}

CSimulation::~CSimulation()
{
	delete terrain;
}

void CSimulation::InitGraphix()
{
	SDL_Init( SDL_INIT_EVERYTHING );
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 

	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1 );
	SDL_SetVideoMode( screenWidth, screenHeight, screenBPP, SDL_OPENGL );

    glClearColor( 0, 0, 0, 0 );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, screenWidth, 0, screenHeight, -1, 1 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0x0);




    SDL_WM_SetCaption( "Upravljanje jedinke: nova pozicija/pocetak(levi klik), novi cilj(desni klik), brzina(Q/A), velicina vidnog polja(+/-), delay(W/S) ", NULL );
}

void CSimulation::SetWindow(int _screenW, int _screenH, int _screenBPP)
{
	screenWidth = _screenW;
	screenHeight = _screenH;
	screenBPP = _screenBPP;
	InitGraphix();
}

void CSimulation::PostConstructionSetup()
{
	//CEM = new CConsciousEntityManager();
	terrain->PostConstructionSetup();
	
	FAG = new CFastAccesGrid(50,50,terrain);
	terrain->DetermineNodeConnectability();
	terrain->DetermineNodesToNLSsConnectability();
	terrain->AddNeighboursToTempNodes();
	
	//resourceManager = new CResourceManager(1,0,0,0,0,0);
}

void CSimulation::Run()
{
	PostConstructionSetup();

	while(run)
	{
		while( SDL_PollEvent( &event ) )
		{
			HandleEvents();
			terrain->DegugModeToggle();
		}
		Logic();
		Draw();
	}

	delete simulation;
}

void CSimulation::HandleEvents()
{
	camera.Events();
	SetPathStartEndEvents();

	if( event.type == SDL_QUIT )
	{
        run = false;
    }
}

void CSimulation::Logic()
{
	camera.Update(screenWidth,screenHeight,mapWidth,mapHeight);
	F::GLOBAL::TIME++;

	if(F::GLOBAL::TIME % DELAY == 0)
	if(CEM)
	CEM->Update();

	// if(keystates[SDLK_i])
	// 	Screendump("scrinshot2.tga", 800, 800);
    //
	// if(keystates[SDLK_s])
	// {
	// 	DELAY++;
	// }
    //
	// if(keystates[SDLK_w])
	// {
	// 	DELAY--;
	// 	if(DELAY==0)
	// 		DELAY=1;
	// }

	//if(lstemp.b.LengthSq())
	//{
	//	glColor3f(0,1,0);

	//	vector<CFAGTile*> ftiles;
	//	FAG->GetLineSegmentTiles2(lstemp, ftiles);
	//}
}

void CSimulation::Draw()
{
	glClear( GL_COLOR_BUFFER_BIT );

	glLoadIdentity();

	glDisable(GL_STENCIL_TEST);
	
	camera.Apply();
	///////////////////////////
	glColor3d(103.0/255, 120.0/255, 129.0/255);
	simulationArea.DrawFill();

	glLineWidth(1);
	glColor3d(0.0/255, 67.0/255, 88.0/255);
	Draw::Border(mapWidth,mapHeight,20, CVector(0,0));

	terrain->Draw();

	Draw::CircleOutline(fovCirc.position, fovCirc.r, 20);

	if(terrain->DebugMode)
	{
		for(int i=0; i< fagLineTiles.size(); i++)
		{
			fagLineTiles[i]->Draw();
		}
	}

	/*
	lstemp.Set(CVector(400,400), CVector(8000,8000));

	if(lstemp.b.LengthSq())
	{
		glColor3f(0,1,0);

		lstemp.Draw();

		vector<CFAGTile*> ftiles;
		FAG->GetLineSegmentTiles_ReturnTrueIfTileIsInObstacle(lstemp, ftiles);

		for(int i=0; i<ftiles.size(); i++)
			ftiles[i]->Draw();
	}*/

	if(CEM)
	CEM->Draw();

	//FAG->Draw();
	/*
	if(CEM)
	{
		LSV->TestUpdate((*(CEM->consciousEntities).begin())->paths);
		LSV->Draw();
	}

	lstemp.Draw();
	*/
	glEnable(GL_STENCIL_TEST);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);

	glColor4d(0,0,0, 0.5);
	simulationArea.DrawFill();
	
	SDL_GL_SwapBuffers();
}

///////////////////////////////////////////////////////////////////////////

void CSimulation::SetPathStartEndEvents()
{
	if( event.type == SDL_KEYDOWN )
	{               
		switch( event.key.keysym.sym )
		{
		case SDLK_SPACE: 
			{

			}break;
		}
	}

	if(event.type == SDL_MOUSEMOTION)
	{
		//if(lstemp.a.LengthSq())
		//	lstemp.b = GetWorldCoordinate(event.button.x, event.button.y);
	}

	if( event.type == SDL_MOUSEBUTTONDOWN )
	{
		if(event.button.button == SDL_BUTTON_RIGHT)
		{
			CVector worldCoord = GetWorldCoordinate(event.button.x, event.button.y);
			if(F::GLOBAL::globalGoal)
				delete F::GLOBAL::globalGoal;

			F::GLOBAL::globalGoal = new CVector(worldCoord);
		}
			else
				if( event.button.button == SDL_BUTTON_LEFT )
				{
					CVector worldCoord = GetWorldCoordinate(event.button.x, event.button.y);
					if(CEM) delete CEM;
					CEM = new CConsciousEntityManager();
					CEM->consciousEntities.push_back(new CConsciousEntity(worldCoord, F::GLOBAL::FOVCIRCLE));
				}
	}
}

CVector CSimulation::GetWorldCoordinate(int _x, int _y)
{
	return CVector(_x/camera.zoom - camera.offset.x/camera.zoom, (screenHeight - _y)/camera.zoom - camera.offset.y/camera.zoom);
}

void CSimulation::CreateSimulation(int _mapWidth, int _mapHeight)
{
	if(simulation)
		delete simulation;

	simulation = new CSimulation(_mapWidth, _mapHeight);
}

CSimulation* CSimulation::Instance()
{
	return simulation;
}
