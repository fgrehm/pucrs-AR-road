#ifndef GLOBALS_H
#define GLOBALS_H

#include <AR/arMulti.h>

#include "object.h"

/* Road multi marker configs */
ARMultiMarkerInfoT  *multiMarkerConfig;

/* Independent buildings settings */
ObjectData_T *buildingMarkers;
int           totalBuildingMarkers;

/* scene settings */
extern int showBuildings;

/* colors */
static GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
static GLfloat red[]   = {1.0, 0.0, 0.0, 1.0};
static GLfloat green[] = {0.0, 1.0, 0.0, 1.0};

#endif
