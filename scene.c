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

static void drawMarker(double trans1[3][4], double trans2[3][4], int mode);
static void drawCube(double trans1[3][4], double trans2[3][4], double x, double y, double z, GLfloat color[3], GLdouble size);
static int  drawBuildingFromMarker(int markerId, double gl_para[16]);

void drawBuildingsThatHaveMarkers() {
  int     i;
  double  gl_para[16];

  glClearDepth( 1.0 );
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_LIGHTING);

  /* calculate the viewing parameters - gl_para */
  for (i = 0; i < totalBuildingMarkers; i++) {
    if (buildingMarkers[i].visible == 0) continue;
    argConvGlpara(buildingMarkers[i].trans, gl_para);
    drawBuildingFromMarker(buildingMarkers[i].id, gl_para);
  }

  glDisable( GL_LIGHTING );
  glDisable( GL_DEPTH_TEST );
}

void drawCarsAndStaticBuildings(int executionTime) {
  int i;

  //for(i=0;i<3;i++) {
  //  int j;
  //    for(j=0;j<4;j++) printf("%10.5f ", multiMarkerConfig->trans[i][j]);
  //    printf("\n");
  //}
  //printf("\n");
  argDrawMode3D();
  argDraw3dCamera( 0, 0 );
  glClearDepth( 1.0 );
  glClear(GL_DEPTH_BUFFER_BIT);

  // for( i = 0; i < multiMarkerConfig->marker_num; i++ ) {
  //     if (multiMarkerConfig->marker[i].visible < 0) continue;

  //     // printf("Will draw relative to %d\n", i);
  //     drawMarker(multiMarkerConfig->trans, multiMarkerConfig->marker[i].trans, 0);
  //     drawCube(multiMarkerConfig->trans, multiMarkerConfig->marker[i].trans, 30, 0.0);
  //     drawCube(multiMarkerConfig->trans, multiMarkerConfig->marker[i].trans, 0.0, 30);

  //     break;
  // }
  //

  if (!showBuildings) {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  }
  //for( i = 0; i < multiMarkerConfig->marker_num; i++ ) {
  //    //printf("Marker %d, visible %d\n", i, multiMarkerConfig->marker[i].visible);
  //    if( multiMarkerConfig->marker[i].visible >= 0 ) drawMarker( multiMarkerConfig->trans, multiMarkerConfig->marker[i].trans, 0 );
  //    else                                 drawMarker( multiMarkerConfig->trans, multiMarkerConfig->marker[i].trans, 1 );
  //}
  drawCube(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, 301, 0, 0, green, 100);
  drawCube(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, 301, 0, 100, green, 100);
  // drawCube(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, 451, 0, green, 100);
  // drawCube(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, 160, -230, green, 100);
  if (!showBuildings) {
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  }

  // drawMarker(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, 0);
  double speed = 0.3;
  distance = (executionTime % 3000) * speed;
  // if (distance >= 300) {
  //   distance = 0;
  // }
  if (debugLevel >= 3) {
    printf("distance = %f\n", distance);
  }
  // Cars
  drawCube(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, -100+distance, -90, 0, green, 30);
  drawCube(multiMarkerConfig->trans, multiMarkerConfig->marker[0].trans, 800-distance, -140, 0, red, 30);
}

void drawMarker(double trans1[3][4], double trans2[3][4], int mode) {
  if (mode == 1)
    drawCube(trans1, trans2, 0, 0, 0, red, 20);
  else
    drawCube(trans1, trans2, 0, 0, 0, blue, 20);
}

void drawCube(double trans1[3][4], double trans2[3][4], double x, double y, double z, GLfloat color[], GLdouble size) {
  double    gl_para[16];
  GLfloat   mat_ambient[]     = {color[0], color[1], color[2], color[3]};
  GLfloat   mat_flash[]       = {color[0], color[1], color[2], color[3]};
  GLfloat   mat_flash_shiny[] = {90.0};
  GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
  GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
  GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  /* load the camera transformation matrix */
  glMatrixMode(GL_MODELVIEW);
  argConvGlpara(trans1, gl_para);
  glLoadMatrixd( gl_para );
  argConvGlpara(trans2, gl_para);
  glMultMatrixd( gl_para );

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

  glDisable( GL_DEPTH_TEST );
  glPopMatrix();
}

int drawBuildingFromMarker(int markerId, double gl_para[16]) {
  GLfloat   mat_ambient[]				= {0.0, 0.0, 1.0, 1.0};
  GLfloat   mat_ambient_collide[]     = {1.0, 0.0, 0.0, 1.0};
  GLfloat   mat_flash[]				= {0.0, 0.0, 1.0, 1.0};
  GLfloat   mat_flash_collide[]       = {1.0, 0.0, 0.0, 1.0};
  GLfloat   mat_flash_shiny[] = {50.0};
  GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
  GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
  GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

  argDrawMode3D();
  argDraw3dCamera( 0, 0 );
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd( gl_para );

  /* set the material */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

  glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);

  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  /* draw a cube */
  glTranslatef( 0.0, 0.0, -50.0 );
  glutSolidCube(100);
  glTranslatef( 0.0, 0.0, -100.0 );
  glutSolidCube(100);

  argDrawMode2D();

  return 0;
}
