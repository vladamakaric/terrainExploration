#include "PrecompiledShapes.h"
#include "draw.h"

GLint CPCShapes::triangle100;
GLint CPCShapes::triangle50;
GLint CPCShapes::circle100x100;

void CPCShapes::Initialize()
{
	triangle100 = glGenLists(1); 
	triangle50 = glGenLists(1); 
	circle100x100 = glGenLists(1); 

	glNewList(triangle100, GL_COMPILE);
	Draw::CircleFill(CVector(0,0), 100, 3);
	glEndList();

	glNewList(triangle50, GL_COMPILE);
	Draw::CircleFill(CVector(0,0), 50, 3);
	glEndList();

	glNewList(circle100x100, GL_COMPILE);
	Draw::CircleFill(CVector(0,0), 100, 100);
	glEndList();
}

void CPCShapes::DrawShape( GLint shape, CVector& _position )
{
	glPushMatrix();
	glTranslatef(_position.x, _position.y, 0);
	glCallList(shape);
	glPopMatrix();
}

void CPCShapes::DrawShapeScaled( GLint shape, double scale, CVector& _position )
{
	glPushMatrix();
	glTranslatef(_position.x, _position.y, 0);
	glScalef(scale, scale, 1);
	glCallList(shape);
	glPopMatrix();
}
