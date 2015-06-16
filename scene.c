#include <AR/ar.h>
#include <AR/gsub_lite.h>

#include "globals.h"
#include "scene.h"

// Something to look at, draw a rotating colour cube.
void DrawCube(void)
{
  // Colour cube data.
  static GLuint polyList = 0;
  float fSize = 0.5f;
  long f, i;
  const GLfloat cube_vertices [8][3] = {
    {1.0, 1.0, 1.0}, {1.0, -1.0, 1.0}, {-1.0, -1.0, 1.0}, {-1.0, 1.0, 1.0},
    {1.0, 1.0, -1.0}, {1.0, -1.0, -1.0}, {-1.0, -1.0, -1.0}, {-1.0, 1.0, -1.0} };
  const GLfloat cube_vertex_colors [8][3] = {
    {1.0, 1.0, 1.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 1.0},
    {1.0, 0.0, 1.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0} };
  GLint cube_num_faces = 6;
  const short cube_faces [6][4] = {
    {3, 2, 1, 0}, {2, 3, 7, 6}, {0, 1, 5, 4}, {3, 0, 4, 7}, {1, 2, 6, 5}, {4, 5, 6, 7} };

  if (!polyList) {
    polyList = glGenLists (1);
    glNewList(polyList, GL_COMPILE);
    glBegin (GL_QUADS);
    for (f = 0; f < cube_num_faces; f++)
      for (i = 0; i < 4; i++) {
        glColor3f (cube_vertex_colors[cube_faces[f][i]][0], cube_vertex_colors[cube_faces[f][i]][1], cube_vertex_colors[cube_faces[f][i]][2]);
        glVertex3f(cube_vertices[cube_faces[f][i]][0] * fSize, cube_vertices[cube_faces[f][i]][1] * fSize, cube_vertices[cube_faces[f][i]][2] * fSize);
      }
    glEnd ();
    glColor3f (0.0, 0.0, 0.0);
    for (f = 0; f < cube_num_faces; f++) {
      glBegin (GL_LINE_LOOP);
      for (i = 0; i < 4; i++)
        glVertex3f(cube_vertices[cube_faces[f][i]][0] * fSize, cube_vertices[cube_faces[f][i]][1] * fSize, cube_vertices[cube_faces[f][i]][2] * fSize);
      glEnd ();
    }
    glEndList ();
  }

  glPushMatrix(); // Save world coordinate system.
  glTranslatef(0.5, 0.5, 0.5); // Place base of cube on marker surface.
  glRotatef(gDrawRotateAngle, 0.0, 0.0, 1.0); // Rotate about z axis.
  glDisable(GL_LIGHTING);	// Just use colours.
  glCallList(polyList);	// Draw the cube.
  glPopMatrix();	// Restore world coordinate system.

}

void DrawToten(void) {
  double    gl_para[16];
  GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
  GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
  GLfloat   mat_flash_shiny[] = {50.0};
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

  glPushMatrix(); // Save world coordinate system.

  // trick start here *********************
  glTranslatef(1.6, 1.6, 0);
  int i;
  for (i = 0; i < 4; i++) {
    glTranslatef(0, 0, 0.6);
    glutSolidCube(1.2);
  }

  glPopMatrix();	// Restore world coordinate system.

  glDisable( GL_LIGHTING );
}

void DrawSphere(void) {
  double    gl_para[16];
  GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
  GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
  GLfloat   mat_flash_shiny[] = {50.0};
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

  glPushMatrix(); // Save world coordinate system.

  // trick start here *********************
  glTranslatef(0.5, 0.5, 1);
  glRotatef(gDrawRotateAngle*5, 0, 0, 1);
  glTranslatef(-2, -2, 0);
  glutSolidSphere(0.6, 24, 24);

  glPopMatrix();	// Restore world coordinate system.

  glDisable( GL_LIGHTING );
}
