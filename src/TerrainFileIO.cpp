#include "Terrain.h"
#include <stdlib.h>
#include "Obstacle.h"
#include "simulationDefinitions.h"

// izbrisati CPOlygon, nepotreban
CTerrain::CTerrain( char *_fileName )
{
	FILE *F;
	int obstacleCount;
	F=fopen(_fileName,"r+");

	if(F==NULL)
		exit(1);

	fscanf(F,"%d",&obstacleCount);

	for(int i=0; i < obstacleCount; i++)
	{
		obstacles.push_back(CObstacle());
		double x,y,r;
		int vCount;

		fscanf(F,"%lf",&x);
		fscanf(F,"%lf",&y);
		fscanf(F,"%lf",&r);
		obstacles.rbegin()->boundingCircle.Set(CVector(x,y), r);

		fscanf(F,"%d",&vCount);
		for(int j=0; j < vCount; j++)
		{
			fscanf(F,"%lf",&x);
			fscanf(F,"%lf",&y);
			obstacles.rbegin()->pointsVector->push_back(new CVector(x,y));
		}

		obstacles.rbegin()->InitializeAfterLoadedFromFile();
	}

	fclose(F);
}

CTerrain::~CTerrain()
{
	if(!CSimulation::writeFileName)
		return;

	FILE *F1;
	F1 = fopen(CSimulation::writeFileName, "w+");



	fprintf(F1, "%d ", (int)obstacles.size());

	int pointNum;
	for(int i=0; i < obstacles.size(); i++)
	{
		fprintf(F1, "%f ", obstacles[i].boundingCircle.position.x);
		fprintf(F1, "%f ", obstacles[i].boundingCircle.position.y);
		fprintf(F1, "%f ", obstacles[i].boundingCircle.r);

		pointNum = obstacles[i].pointsVector->size();
		fprintf(F1, "%d ", pointNum);

		for(int j=0; j < pointNum; j++)
		{
			double x=  (*obstacles[i].pointsVector)[j]->x;
			double y=  (*obstacles[i].pointsVector)[j]->y;

			fprintf(F1, "%f ", x);
			fprintf(F1, "%f ", y);
		}
	}

	fclose(F1);
}
