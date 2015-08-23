#include "Tesselator.h"
#include <iostream>

void tessErrorCB(GLenum errorCode)
{
    const GLubyte *errorStr;
    errorStr = gluErrorString(errorCode);
	cout << errorStr << endl;
}

void tessBeginCB(GLenum which)
{
    glBegin(which);
}

void tessEndCB()
{
    glEnd();
}

void tessVertexCB(const GLvoid *data)
{
    const GLdouble *ptr = (const GLdouble*)data;
/*	
	GLfloat R = GLfloat((rand()%100))/100.0f;
	GLfloat G = GLfloat((rand()%100))/100.0f;
	GLfloat B = GLfloat((rand()%100))/100.0f;
	glColor3f(R, G, B);*/

	glVertex3dv(ptr);
}

void tessCombineCB(const GLdouble newVertex[3], const GLdouble *neighborVertex[4],
                            const GLfloat neighborWeight[4], GLdouble **outData)
{
    // copy new intersect vertex to local array
    // Because newVertex is temporal and cannot be hold by tessellator until next
    // vertex callback called, it must be copied to the safe place in the app.
    // Once gluTessEndPolygon() called, then you can safly deallocate the array.

   GLdouble vertices[3]; //= ///newVertex[0];

    vertices[0] = newVertex[0];
    vertices[1] = newVertex[1];
	vertices[2] = newVertex[2];

    // compute vertex color with given weights and colors of 4 neighbors
    // the neighborVertex[4] must hold required info, in this case, color.
    // neighborVertex was actually the third param of gluTessVertex() and is
    // passed into here to compute the color of the intersect vertex.
    /*vertices[vertexIndex][3] = neighborWeight[0] * neighborVertex[0][3] +   // red
                               neighborWeight[1] * neighborVertex[1][3] +
                               neighborWeight[2] * neighborVertex[2][3] +
                               neighborWeight[3] * neighborVertex[3][3];
    vertices[vertexIndex][4] = neighborWeight[0] * neighborVertex[0][4] +   // green
                               neighborWeight[1] * neighborVertex[1][4] +
                               neighborWeight[2] * neighborVertex[2][4] +
                               neighborWeight[3] * neighborVertex[3][4];
    vertices[vertexIndex][5] = neighborWeight[0] * neighborVertex[0][5] +   // blue
                               neighborWeight[1] * neighborVertex[1][5] +
                               neighborWeight[2] * neighborVertex[2][5] +
                               neighborWeight[3] * neighborVertex[3][5];*/


    // return output data (vertex coords and others)
    *outData = vertices;   // assign the address of new intersect vertex

  // increase index for next vertex 
}

int CTesselator::Tesselate(vector<vector<CVector*>> &_contours)
{
	GLint displayList = glGenLists(1); 
    GLUtesselator *tess = gluNewTess();
	gluTessNormal(tess,0,0,1);

	vector<GLdouble**> inputContours;

	for(int i=0; i < _contours.size(); i++)
		inputContours.push_back(new GLdouble*[_contours[i].size()]);

	for(int i=0; i < _contours.size(); i++)
	{
		for(int j=0; j<_contours[i].size(); j++)
		{
			inputContours[i][j] = new GLdouble[3];
			inputContours[i][j][0] = _contours[i][j]->x;
			inputContours[i][j][1] = _contours[i][j]->y;
			inputContours[i][j][2] = 0;
		}
	}

    gluTessCallback(tess, GLU_TESS_BEGIN, (void (*)())tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void (*)())tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void (*)())tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (*)())tessVertexCB);
	gluTessCallback(tess, GLU_TESS_COMBINE, (void (*)())tessCombineCB);

	if(_contours.size()>1)
	{
		int ysdf=43;
	}

	gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
    glNewList(displayList, GL_COMPILE);
    gluTessBeginPolygon(tess, 0);    

	for(int i=0; i < _contours.size(); i++)
	{
		gluTessBeginContour(tess);

		for(int j=0; j<_contours[i].size(); j++)
			gluTessVertex(tess, inputContours[i][j], inputContours[i][j]);
	//	gluTessVertex(tess, inputContours[i][0], inputContours[i][0]);

		gluTessEndContour(tess);
	}
    gluTessEndPolygon(tess);
    glEndList();
    gluDeleteTess(tess); 

	for(int i=0; i < _contours.size(); i++)
	{
		for(int j=0; j<_contours[i].size(); j++)
		{
			delete[] inputContours[i][j];
		}
		delete [] inputContours[i];
	}

	return displayList;
}

int CTesselator::Tesselate(CPolygon* _p)
{
    GLint displayList = glGenLists(1); 
    GLUtesselator *tess = gluNewTess();
	gluTessNormal(tess,0,0,1);

	GLdouble **inputPoints = new GLdouble*[_p->pointNum];

	for(int i=0; i < _p->pointNum; i++)
	{
		inputPoints[i]= new GLdouble[3];
		inputPoints[i][0] = _p->points[i].x;
		inputPoints[i][1] = _p->points[i].y;
		inputPoints[i][2] = 0;
	}

    gluTessCallback(tess, GLU_TESS_BEGIN, (void (*)())tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void (*)())tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void (*)())tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (*)())tessVertexCB);
	//gluTessCallback(tess, GLU_TESS_COMBINE, (void (CALLBACK *)())tessCombineCB);

	gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
    glNewList(displayList, GL_COMPILE);
    gluTessBeginPolygon(tess, 0);    
        gluTessBeginContour(tess);
			for(int i=0; i < _p->pointNum; i++)
				gluTessVertex(tess, inputPoints[i], inputPoints[i]);
			gluTessVertex(tess, inputPoints[0], inputPoints[0]);
        gluTessEndContour(tess);
    gluTessEndPolygon(tess);
    glEndList();
    gluDeleteTess(tess); 

	for(int i=0; i < _p->pointNum; i++)
		delete[] inputPoints[i];
	delete[] inputPoints;

	return displayList;
}


