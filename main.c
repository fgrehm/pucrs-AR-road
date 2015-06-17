/*
 *  simpleLite.c
 *
 *  Some code to demonstrate use of gsub_lite's argl*() functions.
 *  Shows the correct GLUT usage to read a video frame (in the idle callback)
 *  and to draw it (in the display callback).
 *
 *  Press '?' while running for help on available key commands.
 *
 *  Copyright (c) 2001-2007 Philip Lamb (PRL) phil@eden.net.nz. All rights reserved.
 *
 *	Rev		Date		Who		Changes
 *	1.0.0	20040302	PRL		Initial version, simple test animation using GLUT.
 *	1.0.1	20040721	PRL		Correctly sets window size; supports arVideoDispOption().
 *
 */
/*
 *
 * This file is part of ARToolKit.
 *
 * ARToolKit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ARToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ARToolKit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


// ============================================================================
//	Includes
// ============================================================================

#include <stdio.h>
#include <stdlib.h>					// malloc(), free()
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>			// arParamDisp()
#include <AR/ar.h>
#include <AR/gsub_lite.h>

#include "scene.h"
#include "globals.h"

// ============================================================================
//	Functions
// ============================================================================

static int setupCamera(const char *cparam_name, char *vconf, ARParam *cparam)
{
  ARParam			wparam;
  int				xsize, ysize;

  // Open the video path.
  if (arVideoOpen(vconf) < 0) {
    fprintf(stderr, "setupCamera(): Unable to open connection to camera.\n");
    return (FALSE);
  }

  // Find the size of the window.
  if (arVideoInqSize(&xsize, &ysize) < 0) return (FALSE);
  fprintf(stdout, "Camera image size (x,y) = (%d,%d)\n", xsize, ysize);

  // Load the camera parameters, resize for the window and init.
  if (arParamLoad(cparam_name, 1, &wparam) < 0) {
    fprintf(stderr, "setupCamera(): Error loading parameter file %s for camera.\n", cparam_name);
    return (FALSE);
  }
  arParamChangeSize(&wparam, xsize, ysize, cparam);
  fprintf(stdout, "*** Camera Parameter ***\n");
  arParamDisp(cparam);

  arInitCparam(cparam);

  if (arVideoCapStart() != 0) {
    fprintf(stderr, "setupCamera(): Unable to begin camera data capture.\n");
    return (FALSE);
  }

  return (TRUE);
}

static int setupMarker(const char *patt_name, int *patt_id)
{
  // Loading only 1 pattern in this example.
  if ((*patt_id = arLoadPatt(patt_name)) < 0) {
    fprintf(stderr, "setupMarker(): pattern load error !!\n");
    return (FALSE);
  }

  return (TRUE);
}

// Report state of ARToolKit global variables arFittingMode,
// arImageProcMode, arglDrawMode, arTemplateMatchingMode, arMatchingPCAMode.
static void debugReportMode(const ARGL_CONTEXT_SETTINGS_REF arglContextSettings)
{
  if (arFittingMode == AR_FITTING_TO_INPUT) {
    fprintf(stderr, "FittingMode (Z): INPUT IMAGE\n");
  } else {
    fprintf(stderr, "FittingMode (Z): COMPENSATED IMAGE\n");
  }

  if (arImageProcMode == AR_IMAGE_PROC_IN_FULL) {
    fprintf(stderr, "ProcMode (X)   : FULL IMAGE\n");
  } else {
    fprintf(stderr, "ProcMode (X)   : HALF IMAGE\n");
  }

  if (arglDrawModeGet(arglContextSettings) == AR_DRAW_BY_GL_DRAW_PIXELS) {
    fprintf(stderr, "DrawMode (C)   : GL_DRAW_PIXELS\n");
  } else if (arglTexmapModeGet(arglContextSettings) == AR_DRAW_TEXTURE_FULL_IMAGE) {
    fprintf(stderr, "DrawMode (C)   : TEXTURE MAPPING (FULL RESOLUTION)\n");
  } else {
    fprintf(stderr, "DrawMode (C)   : TEXTURE MAPPING (HALF RESOLUTION)\n");
  }

  if (arTemplateMatchingMode == AR_TEMPLATE_MATCHING_COLOR) {
    fprintf(stderr, "TemplateMatchingMode (M)   : Color Template\n");
  } else {
    fprintf(stderr, "TemplateMatchingMode (M)   : BW Template\n");
  }

  if (arMatchingPCAMode == AR_MATCHING_WITHOUT_PCA) {
    fprintf(stderr, "MatchingPCAMode (P)   : Without PCA\n");
  } else {
    fprintf(stderr, "MatchingPCAMode (P)   : With PCA\n");
  }
}

static void Quit(void)
{
  arglCleanup(gArglSettings);
  arVideoCapStop();
  arVideoClose();
  exit(0);
}

static void Keyboard(unsigned char key, int x, int y)
{
  int mode;
  switch (key) {
    case 0x1B:						// Quit.
    case 'Q':
    case 'q':
      Quit();
      break;
    case ' ':
      gDrawRotate = !gDrawRotate;
      break;
    case 'C':
    case 'c':
      mode = arglDrawModeGet(gArglSettings);
      if (mode == AR_DRAW_BY_GL_DRAW_PIXELS) {
        arglDrawModeSet(gArglSettings, AR_DRAW_BY_TEXTURE_MAPPING);
        arglTexmapModeSet(gArglSettings, AR_DRAW_TEXTURE_FULL_IMAGE);
      } else {
        mode = arglTexmapModeGet(gArglSettings);
        if (mode == AR_DRAW_TEXTURE_FULL_IMAGE)	arglTexmapModeSet(gArglSettings, AR_DRAW_TEXTURE_HALF_IMAGE);
        else arglDrawModeSet(gArglSettings, AR_DRAW_BY_GL_DRAW_PIXELS);
      }
      fprintf(stderr, "*** Camera - %f (frame/sec)\n", (double)gCallCountMarkerDetect/arUtilTimer());
      gCallCountMarkerDetect = 0;
      arUtilTimerReset();
      debugReportMode(gArglSettings);
      break;
    case 'D':
    case 'd':
      arDebug = !arDebug;
      break;
    case '?':
    case '/':
      printf("Keys:\n");
      printf(" q or [esc]    Quit demo.\n");
      printf(" c             Change arglDrawMode and arglTexmapMode.\n");
      printf(" d             Activate / deactivate debug mode.\n");
      printf(" ? or /        Show this help.\n");
      printf("\nAdditionally, the ARVideo library supplied the following help text:\n");
      arVideoDispOption();
      break;
    default:
      break;
  }
}

static void Idle(void)
{
  static int ms_prev;
  int ms;
  float s_elapsed;
  ARUint8 *image;

  ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
  int             marker_num;						// Count of number of markers detected.
  int             j, k;

  // Find out how long since Idle() last ran.
  ms = glutGet(GLUT_ELAPSED_TIME);
  s_elapsed = (float)(ms - ms_prev) * 0.001;
  if (s_elapsed < 0.01f) return; // Don't update more often than 100 Hz.
  ms_prev = ms;

  // Update drawing.
  DrawCubeUpdate(s_elapsed);

  // Grab a video frame.
  if ((image = arVideoGetImage()) != NULL) {
    gARTImage = image;	// Save the fetched image.

    gCallCountMarkerDetect++; // Increment ARToolKit FPS counter.

    // Detect the markers in the video frame.
    if (arDetectMarker(gARTImage, gARTThreshhold, &marker_info, &marker_num) < 0) {
      exit(-1);
    }

    // Check through the marker_info array for highest confidence
    // visible marker matching our preferred pattern.
    k = -1;
    for (j = 0; j < marker_num; j++) {
      if (marker_info[j].id == gPatt_id) {
        if (k == -1) k = j; // First marker detected.
        else if(marker_info[j].cf > marker_info[k].cf) k = j; // Higher confidence marker detected.
      }
    }

    if (k != -1) {
      // Get the transformation between the marker and the real camera into gPatt_trans.
      arGetTransMat(&(marker_info[k]), gPatt_centre, gPatt_width, gPatt_trans);
      gPatt_found = TRUE;
    } else {
      gPatt_found = FALSE;
    }

    // Tell GLUT the display has changed.
    glutPostRedisplay();
  }
}

//
//	This function is called on events when the visibility of the
//	GLUT window changes (including when it first becomes visible).
//
static void Visibility(int visible)
{
  if (visible == GLUT_VISIBLE) {
    glutIdleFunc(Idle);
  } else {
    glutIdleFunc(NULL);
  }
}

//
//	This function is called when the
//	GLUT window is resized.
//
static void Reshape(int w, int h)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Call through to anyone else who needs to know about window sizing here.
}

//
// This function is called when the window needs redrawing.
//
static void Display(void)
{
  GLdouble p[16];
  GLdouble m[16];

  // Select correct buffer for this context.
  glDrawBuffer(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers for new frame.

  arglDispImage(gARTImage, &gARTCparam, 1.0, gArglSettings);	// zoom = 1.0.
  arVideoCapNext();
  gARTImage = NULL; // Image data is no longer valid after calling arVideoCapNext().

  // Projection transformation.
  arglCameraFrustumRH(&gARTCparam, VIEW_DISTANCE_MIN, VIEW_DISTANCE_MAX, p);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(p);
  glMatrixMode(GL_MODELVIEW);

  // Viewing transformation.
  glLoadIdentity();
  // Lighting and geometry that moves with the camera should go here.
  // (I.e. must be specified before viewing transformations.)
  //none

  if (gPatt_found) {

    // Calculate the camera position relative to the marker.
    // Replace VIEW_SCALEFACTOR with 1.0 to make one drawing unit equal to 1.0 ARToolKit units (usually millimeters).
    arglCameraViewRH(gPatt_trans, m, VIEW_SCALEFACTOR);
    glLoadMatrixd(m);

    // All lighting and geometry to be drawn relative to the marker goes here.
    if (!arDebug) {
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }
    DrawToten();
    if (!arDebug) {
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    DrawCube();
    DrawSphere();

  } // gPatt_found

  // Any 2D overlays go here.
  //none

  glutSwapBuffers();
}

int main(int argc, char** argv)
{
  char glutGamemode[32];
  const char *cparam_name = "Data/camera_para.dat";
  //
  // Camera configuration.
  //
#ifdef _WIN32
  char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
  char			*vconf = "";
#endif
  const char *patt_name  = "Data/patt.hiro";

  // ----------------------------------------------------------------------------
  // Library inits.
  //

  glutInit(&argc, argv);

  // ----------------------------------------------------------------------------
  // Hardware setup.
  //

  if (!setupCamera(cparam_name, vconf, &gARTCparam)) {
    fprintf(stderr, "main(): Unable to set up AR camera.\n");
    exit(-1);
  }

  // ----------------------------------------------------------------------------
  // Library setup.
  //

  // Set up GL context(s) for OpenGL to draw into.
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  if (!prefWindowed) {
    if (prefRefresh) sprintf(glutGamemode, "%ix%i:%i@%i", prefWidth, prefHeight, prefDepth, prefRefresh);
    else sprintf(glutGamemode, "%ix%i:%i", prefWidth, prefHeight, prefDepth);
    glutGameModeString(glutGamemode);
    glutEnterGameMode();
  } else {
    glutInitWindowSize(prefWidth, prefHeight);
    glutCreateWindow(argv[0]);
  }

  // Setup argl library for current context.
  if ((gArglSettings = arglSetupForCurrentContext()) == NULL) {
    fprintf(stderr, "main(): arglSetupForCurrentContext() returned error.\n");
    exit(-1);
  }
  debugReportMode(gArglSettings);
  glEnable(GL_DEPTH_TEST);
  arUtilTimerReset();

  if (!setupMarker(patt_name, &gPatt_id)) {
    fprintf(stderr, "main(): Unable to set up AR marker.\n");
    Quit();
  }

  // Register GLUT event-handling callbacks.
  // NB: Idle() is registered by Visibility.
  glutDisplayFunc(Display);
  glutReshapeFunc(Reshape);
  glutVisibilityFunc(Visibility);
  glutKeyboardFunc(Keyboard);

  glutMainLoop();

  return (0);
}
