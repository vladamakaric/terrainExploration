#include "FAGTile.h"
#include "Simulation.h"
#include "functions.h"
#include "global.h"
#include "draw.h"

void CFAGTile::SetUp(int _rNum, int _cNum, CFastAccesGrid* _parentFAG)
{
	parentFAG = _parentFAG;
	colNum=_cNum;
	rowNum=_rNum; 
}

void CFAGTile::Draw()
{
	glColor4f(0,0,1,0.3);

	if(intersected)
	glColor4f(1,0,0,0.3);
	glBegin(GL_QUADS);
	glVertex2d(colNum*parentFAG->tileWidth, rowNum*parentFAG->tileHeight);
	glVertex2d(colNum*parentFAG->tileWidth + parentFAG->tileWidth, rowNum*parentFAG->tileHeight);
	glVertex2d(colNum*parentFAG->tileWidth + parentFAG->tileWidth, rowNum*parentFAG->tileHeight + parentFAG->tileHeight);
	glVertex2d(colNum*parentFAG->tileWidth, rowNum*parentFAG->tileHeight + parentFAG->tileHeight);
	glEnd();


	glColor3f(1,0,1);
	for(int i=0; i < nodes.size(); i++)
	{
		Draw::CircleOutline(nodes[i]->position, 20, 10);
	}


		glLineWidth(10);
	glColor3f(1,0,0);
	glBegin(GL_LINES);
	for(int i=0; i < nodeLines.size(); i++)
	{
		Draw::PushVertex(nodeLines[i]->up->position);
		Draw::PushVertex(nodeLines[i]->down->position);	
	}
	glEnd();
	glLineWidth(1);
}

void CFAGTile::DetermineUniqueObstacles()
{
	for(int i=0; i< nodeLines.size(); i++)
	{
		if(nodeLines[i]->polygon)
		if(!nodeLines[i]->polygon->tagged)
		{
			nodeLines[i]->polygon->tagged = true;
			obstacles.push_back(nodeLines[i]->polygon);
		}
	}

	F::MISC::UntagPointerContainer(obstacles);
}
