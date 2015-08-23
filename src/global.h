#ifndef GLOBAL_H
#define GLOBAL_H

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <typeinfo>

extern SDL_Event event;
extern const double PI;
extern Uint8 *keystates;// = SDL_GetKeyState( NULL );

#endif
