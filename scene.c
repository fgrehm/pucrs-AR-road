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

#include "globals.h"
#include "object.h"
#include "scene.h"

double distance = 0;
int showBuildings = 0;

static void drawCubeBasedOnRoadMarker(double trans1[3][4], double trans2[3][4], double x, double y, double z, GLfloat color[3], GLdouble size);
static void drawBuildingBasedOnRoadMarker(double trans1[3][4], double trans2[3][4], double x, double y, GLfloat color[3]);
static void drawBuildingBasedOnMarker(int markerId, double gl_para[16]);
static void drawSolidCube(double x, double y, double z, GLfloat color[], GLdouble size);

void drawBuildingsThatHaveMarkers() {
  int     i;
  double  gl_para[16];

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_LIGHTING);

  /* calculate the viewing parameters - gl_para */
  for (i = 0; i < totalBuildingMarkers; i++) {
    if (buildingMarkers[i].visible == 0) continue;
    argConvGlpara(buildingMarkers[i].trans, gl_para);
    if (!showBuildings) {
      glDepthMask(GL_TRUE);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }
    drawBuildingBasedOnMarker(buildingMarkers[i].id, gl_para);
    if (!showBuildings) {
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
  }

  glDisable( GL_LIGHTING );
  glDisable( GL_DEPTH_TEST );
}

void drawCarsAndStaticBuildings(int executionTime) {
  int i;

  if (!showBuildings) {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  }
  drawBuildingBasedOnRoadMarker(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, -60, -110, green);
  drawBuildingBasedOnRoadMarker(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, 810, -110, green);

  for (i = 0; i < totalBuildingMarkers; i++) {
    // Skip visible markers
    if (buildingMarkers[i].visible == 1) continue;

    if (!showBuildings) {
      glDepthMask(GL_TRUE);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }
    double *position = buildingRelativePosition[i];

    drawBuildingBasedOnRoadMarker(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, position[0], position[1], red);
    if (!showBuildings) {
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
  }

  if (!showBuildings) {
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  }

  double speed = 0.29;
  distance = (executionTime % 3000) * speed;
  if (debugLevel >= 3) {
    printf("distance = %f\n", distance);
  }

  // Cars
  drawCubeBasedOnRoadMarker(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, -55+distance, -90, 0, blue, 30);
  drawCubeBasedOnRoadMarker(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, -100+distance, -90, 0, red, 30);
  drawCubeBasedOnRoadMarker(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, 780-distance, -140, 0, red, 30);
  drawCubeBasedOnRoadMarker(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, 825-distance, -140, 0, blue, 30);
}

void drawBuildingBasedOnMarker(int markerId, double gl_para[16]) {
  /* load the camera transformation matrix */
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd( gl_para );

  drawSolidCube(0, 0, -100, yellow, 100);
  drawSolidCube(0, 0, -200, yellow, 100);
}

void drawBuildingBasedOnRoadMarker(double trans1[3][4], double trans2[3][4], double x, double y, GLfloat color[3]) {
  drawCubeBasedOnRoadMarker(trans1, trans2, x, y, 0, color, 100);
  drawCubeBasedOnRoadMarker(trans1, trans2, x, y, 100, color, 100);
}

void drawCubeBasedOnRoadMarker(double trans1[3][4], double trans2[3][4], double x, double y, double z, GLfloat color[], GLdouble size) {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  /* load the camera transformation matrix */
  double    gl_para[16];
  glMatrixMode(GL_MODELVIEW);
  argConvGlpara(trans1, gl_para);
  glLoadMatrixd( gl_para );
  argConvGlpara(trans2, gl_para);
  glMultMatrixd( gl_para );

  drawSolidCube(x, y, z, color, size);

  glDisable( GL_DEPTH_TEST );
}

void drawSolidCube(double x, double y, double z, GLfloat color[], GLdouble size) {
  GLfloat   mat_ambient[]     = {color[0], color[1], color[2], color[3]};
  GLfloat   mat_flash[]       = {color[0], color[1], color[2], color[3]};
  GLfloat   mat_flash_shiny[] = {90.0};
  GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
  GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
  GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);

  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glTranslatef( x, y, size/2 + z );
  if( debugLevel < 3 )
    glutSolidCube(size);
  else
    glutWireCube(size);
  glDisable( GL_LIGHTING );

  glPopMatrix();
}
