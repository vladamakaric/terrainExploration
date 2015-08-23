#include "Camera.h"
#include "global.h"
#include <iostream>

using namespace std;

void CCamera::Init(double _x, double _y, int _cameraThreshold, int _cameraSpeed)
{
	offset.Set(_x,_y);
	cameraThreshold = _cameraThreshold;
	cameraSpeed = _cameraSpeed;
	zoom = 0.1;
	prevZoom = zoom;
}

void CCamera::Update(int _screenWidth, int _screenHeight, int _mapWidth, int _mapHeight)
{
	int x,y;
	SDL_GetMouseState(&x, &y);

	if(_mapHeight*zoom<_screenHeight)
		zoom = double(_screenHeight)/double(_mapHeight);

	if(_mapWidth*zoom<_screenWidth)
		zoom = double(_screenWidth)/double(_mapWidth);

	if(offset.x>0)
		offset.x=0;

	if(offset.y>0)
		offset.y=0;

	if(offset.y < -_mapHeight*zoom + _screenHeight)
		offset.y= -_mapHeight*zoom + _screenHeight;

	if(offset.x < -_mapWidth*zoom + _screenWidth)
		offset.x = -_mapWidth*zoom + _screenWidth;

	double zoomDiff =  zoom-prevZoom;

	offset.x -= (x + fabs(offset.x))/prevZoom*zoomDiff;
	offset.y -= ((_screenHeight - y) + fabs(offset.y))/prevZoom*zoomDiff; 

	prevZoom = zoom;

	if(_mapHeight*zoom<_screenHeight)
		zoom = double(_screenHeight)/double(_mapHeight);

	if(_mapWidth*zoom<_screenWidth)
		zoom = double(_screenWidth)/double(_mapWidth);

	if(x<cameraThreshold) offset.x+=cameraSpeed*sqrt(zoom); else if(x> _screenWidth - cameraThreshold) offset.x-=cameraSpeed*sqrt(zoom);
	if(y<cameraThreshold) offset.y-=cameraSpeed*sqrt(zoom); else if(y> _screenHeight - cameraThreshold) offset.y+=cameraSpeed*sqrt(zoom);

	if(offset.x>0)
		offset.x=0;

	if(offset.y>0)
		offset.y=0;

	if(offset.y < -_mapHeight*zoom + _screenHeight)
		offset.y= -_mapHeight*zoom + _screenHeight;

	if(offset.x < -_mapWidth*zoom + _screenWidth)
		offset.x = -_mapWidth*zoom + _screenWidth;
}

void CCamera::Events()
{
	if( event.type == SDL_KEYDOWN )
	{               
		switch( event.key.keysym.sym )
		{
			case SDLK_UP: 
			{
				zoom*=1.05;
			}break;
			case SDLK_DOWN: 
			{
				zoom*=0.95;
			}break;
		}
	}

	// if(event.type == SDL_MOUSEWHEEL){
	// 	if (event.wheel.y < 0){
	// 	} 
	// 	else{
	// 	}
	// }
}

void CCamera::Apply()
{
	glTranslated(offset.x,offset.y,0);
	glScaled(zoom,zoom,1);
}

