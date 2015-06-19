#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/arMulti.h>

/* set up the video format globals */

extern char	*vconf;

int xsize, ysize;
extern int thresh;
extern int count;

extern char *cparam_name;
ARParam         cparam;

extern char *config_name;
ARMultiMarkerInfoT  *config;

/* scene settings */
extern int showBuildings;

#endif
