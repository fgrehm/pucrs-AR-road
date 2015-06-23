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
static double buildingRelativePosition[3][2] = {
  // 1
  {310, 0},
  // 4
  {160, -220},
  // 5
  {610, -220},
};

/* colors */
static GLfloat blue[]   = {0.0, 0.0, 1.0, 1.0};
static GLfloat red[]    = {1.0, 0.0, 0.0, 1.0};
static GLfloat green[]  = {0.0, 1.0, 0.0, 1.0};
static GLfloat yellow[] = {1.0, 1.0, 0.0, 1.0};

/* Debugging */
extern int debugLevel;

#endif
