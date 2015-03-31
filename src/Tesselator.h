#pragma once
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

void CALLBACK tessErrorCB(GLenum errorCode);
void CALLBACK tessBeginCB(GLenum which);
void CALLBACK tessEndCB();
void CALLBACK tessVertexCB(const GLvoid *data);
void CALLBACK tessCombineCB(const GLdouble newVertex[3], const GLdouble *neighborVertex[4], const GLfloat neighborWeight[4], GLdouble **outData);
