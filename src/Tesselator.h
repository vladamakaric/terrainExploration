#ifndef TESSELATOR_H
#define TESSELATOR_H


#include "global.h"
#include <iostream>
#include <list>
#include "Polygon.h"
#include "Vector.h"

class CTesselator
{
public:
	static int Tesselate(CPolygon* p);
	static int Tesselate(vector<vector<CVector*>> &_contours);
};

void tessErrorCB(GLenum errorCode);
void tessBeginCB(GLenum which);
void tessEndCB();
void tessVertexCB(const GLvoid *data);
void tessCombineCB(const GLdouble newVertex[3], const GLdouble *neighborVertex[4], const GLfloat neighborWeight[4], GLdouble **outData);

#endif
