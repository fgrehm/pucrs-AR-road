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

#include "globals.h"
#include "object.h"
#include "scene.h"

#ifdef _WIN32
char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
char			*vconf = "";
#endif

int thresh = 100;
int count = 0;

int debugLevel = 0;

ARParam  cparam;

int xsize, ysize;

char *config_name = "Data/road-markers/marker.dat";
char *cparam_name = "Data/camera_para.dat";

char *model_name = "Data/road-markers/buildings.dat";

static void   init(void);
static void   cleanup(void);
static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  init();

  arVideoCapStart();
  argMainLoop( NULL, keyEvent, mainLoop );
  return (0);
}

static void   keyEvent( unsigned char key, int x, int y)
{
  /* quit if the ESC key is pressed */
  if( key == 0x1b ) {
    printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
    cleanup();
    exit(0);
  }

  if( key == 't' ) {
    printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
    printf("Enter new threshold value (current = %d): ", thresh);
    scanf("%d",&thresh); while( getchar()!='\n' );
    printf("\n");
    count = 0;
  }

  /* turn on and off the debug mode with 'd' */
  if( key == 'd' ) {
    debugLevel = (debugLevel + 1) % 4;
    printf("*** Debug level set to %d\n", debugLevel);

    showBuildings = (debugLevel >= 2);
    printf("*** showBuildings set to %d\n", showBuildings);

    arDebug = debugLevel == 3;
    printf("*** arDebug set to %d\n", arDebug);
    if( arDebug == 0 ) {
      glClearColor( 0.0, 0.0, 0.0, 0.0 );
      glClear(GL_COLOR_BUFFER_BIT);
      argSwapBuffers();
      glClear(GL_COLOR_BUFFER_BIT);
      argSwapBuffers();
    }
    count = 0;
  }

}

/* main loop */
static void mainLoop(void)
{
  static int ms_prev;
  int ms;
  float s_elapsed;

  // Find out how long since mainLoop() last ran.
  ms = glutGet(GLUT_ELAPSED_TIME);
  s_elapsed = (float)(ms - ms_prev) * 0.001;
  if (s_elapsed < 0.01f) return; // Don't update more often than 100 Hz.
  ms_prev = ms;

  ARUint8         *dataPtr;
  ARMarkerInfo    *marker_info;
  int             marker_num;
  double          err;
  int             i;

  /* grab a vide frame */
  if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
    arUtilSleep(2);
    return;
  }
  if( count == 0 ) arUtilTimerReset();
  count++;

  /* detect the markers in the video frame */
  if( arDetectMarkerLite(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
    cleanup();
    exit(0);
  }

  argDrawMode2D();
  if( !arDebug ) {
    argDispImage( dataPtr, 0,0 );
  } else {
    argDispImage( dataPtr, 1, 1 );
    if( arImageProcMode == AR_IMAGE_PROC_IN_HALF )
      argDispHalfImage( arImage, 0, 0 );
    else
      argDispImage( arImage, 0, 0);
  }

  if (debugLevel > 0) {
    glLineWidth( 1.5 );
    for( i = 0; i < marker_num; i++ ) {
      glColor3f( green[0], green[1], green[2] );
      argDrawSquare( marker_info[i].vertex, 0, 0 );
    }
  }

  /* check for known patterns */
  for( i = 0; i < totalBuildingMarkers; i++ ) {
    int k = -1;
    int j;
    for( j = 0; j < marker_num; j++ ) {
      if( buildingMarkers[i].id == marker_info[j].id) {
        /* you've found a pattern */
        //printf("Found pattern: %d ",patt_id);
        if( k == -1 ) k = j;
        else /* make sure you have the best pattern (highest confidence factor) */
          if( marker_info[k].cf < marker_info[j].cf ) k = j;
      }
    }
    if( k == -1 ) {
      buildingMarkers[i].visible = 0;
      continue;
    }

    /* calculate the transform for each marker */
    if( buildingMarkers[i].visible == 0 ) {
      arGetTransMat(&marker_info[k],
          buildingMarkers[i].marker_center, buildingMarkers[i].marker_width,
          buildingMarkers[i].trans);
    }
    else {
      arGetTransMatCont(&marker_info[k], buildingMarkers[i].trans,
          buildingMarkers[i].marker_center, buildingMarkers[i].marker_width,
          buildingMarkers[i].trans);
    }
    buildingMarkers[i].visible = 1;
  }

  arVideoCapNext();

  if( (err=arMultiGetTransMat(marker_info, marker_num, multiMarkerConfig)) < 0 ) {
    drawBuildingsThatHaveMarkers();
    argSwapBuffers();
    return;
  }
  // if (arDebug) {
  //   printf("err = %f\n", err);
  // }
  if(err > 100.0 ) {
    drawBuildingsThatHaveMarkers();
    argSwapBuffers();
    return;
  }

  drawCarsAndStaticBuildings(ms);
  drawBuildingsThatHaveMarkers();

  argSwapBuffers();
}

static void init( void )
{
  ARParam  wparam;

  /* open the video path */
  if( arVideoOpen( vconf ) < 0 ) exit(0);
  /* find the size of the window */
  if( arVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
  printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

  /* set the initial camera parameters */
  if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
    printf("Camera parameter load error !!\n");
    exit(0);
  }
  arParamChangeSize( &wparam, xsize, ysize, &cparam );
  arInitCparam( &cparam );
  printf("*** Camera Parameter ***\n");
  arParamDisp( &cparam );

  if( (multiMarkerConfig = arMultiReadConfigFile(config_name)) == NULL ) {
    printf("multiMarkerConfig data load error !!\n");
    exit(0);
  }
  if( (buildingMarkers=read_ObjData(model_name, &totalBuildingMarkers)) == NULL ) {
    printf("ObjData data load error !!\n");
    exit(0);
  }
  printf("Objectfile num = %d\n", totalBuildingMarkers);

  /* open the graphics window */
  argInit( &cparam, 1.0, 0, 1, 0, 0 );
  arFittingMode   = AR_FITTING_TO_IDEAL;
  arImageProcMode = AR_IMAGE_PROC_IN_HALF;
  argDrawMode     = AR_DRAW_BY_TEXTURE_MAPPING;
  argTexmapMode   = AR_DRAW_TEXTURE_HALF_IMAGE;
}

/* cleanup function called when program exits */
static void cleanup(void)
{
  arVideoCapStop();
  arVideoClose();
  argCleanup();
}
