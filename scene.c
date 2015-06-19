#include "globals.h"
#include "scene.h"

double distance = 0;

void drawScene(int executionTime) {
  int i;

  //for(i=0;i<3;i++) {
  //  int j;
  //    for(j=0;j<4;j++) printf("%10.5f ", config->trans[i][j]);
  //    printf("\n");
  //}
  //printf("\n");
  argDrawMode3D();
  argDraw3dCamera( 0, 0 );
  glClearDepth( 1.0 );
  glClear(GL_DEPTH_BUFFER_BIT);
  for( i = 0; i < config->marker_num; i++ ) {
      //printf("Marker %d, visible %d\n", i, config->marker[i].visible);
      if( config->marker[i].visible >= 0 ) drawMarker( config->trans, config->marker[i].trans, 0 );
      else                                 drawMarker( config->trans, config->marker[i].trans, 1 );
  }

  // for( i = 0; i < config->marker_num; i++ ) {
  //     if (config->marker[i].visible < 0) continue;

  //     // printf("Will draw relative to %d\n", i);
  //     drawMarker(config->trans, config->marker[i].trans, 0);
  //     drawCube(config->trans, config->marker[i].trans, 30, 0.0);
  //     drawCube(config->trans, config->marker[i].trans, 0.0, 30);

  //     break;
  // }

  // drawMarker(config->trans, config->marker[0].trans, 0);
  double speed = 0.08;
  distance = (executionTime % 4000) * speed;
  // if (distance >= 300) {
  //   distance = 0;
  // }
  if (arDebug) {
    printf("distance = %f\n", distance);
  }
  drawCube(config->trans, config->marker[0].trans, -50+distance, -35);
  drawCube(config->trans, config->marker[0].trans, 270-distance, -60);
}

void drawMarker(double trans1[3][4], double trans2[3][4], int mode) {
    double    gl_para[16];
    GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
    GLfloat   mat_ambient1[]    = {1.0, 0.0, 0.0, 1.0};
    GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
    GLfloat   mat_flash1[]      = {1.0, 0.0, 0.0, 1.0};
    GLfloat   mat_flash_shiny[] = {50.0};
    GLfloat   mat_flash_shiny1[]= {50.0};
    GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    /* load the camera transformation matrix */
    glMatrixMode(GL_MODELVIEW);
    argConvGlpara(trans1, gl_para);
    glLoadMatrixd( gl_para );
    argConvGlpara(trans2, gl_para);
    glMultMatrixd( gl_para );

    if( mode == 0 ) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    }
    else {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash1);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny1);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
    }
    glMatrixMode(GL_MODELVIEW);
    glTranslatef( 0.0, 0.0, 10.0 );
    if( !arDebug ) glutSolidCube(20.0);
     else          glutWireCube(20.0);
    glDisable( GL_LIGHTING );

    glDisable( GL_DEPTH_TEST );
}

void drawCube(double trans1[3][4], double trans2[3][4], double x, double y) {
    double    gl_para[16];
    GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
    GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
    GLfloat   mat_flash_shiny[] = {50.0};
    GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
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

    glMatrixMode(GL_MODELVIEW);
    glTranslatef( x, y, 10.0 );
    if( !arDebug ) glutSolidCube(20.0);
     else          glutWireCube(20.0);
    glDisable( GL_LIGHTING );

    glDisable( GL_DEPTH_TEST );
}
