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
#include "scene.h"

#ifdef _WIN32
char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
char			*vconf = "";
#endif

int thresh = 100;
int count = 0;

char *config_name = "Data/multi/marker.dat";
char *cparam_name    = "Data/camera_para.dat";

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

    /* turn on and off the debug mode with right mouse */
    if( key == 'd' ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        arDebug = 1 - arDebug;
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
    }
    else {
        argDispImage( dataPtr, 1, 1 );
        if( arImageProcMode == AR_IMAGE_PROC_IN_HALF )
            argDispHalfImage( arImage, 0, 0 );
        else
            argDispImage( arImage, 0, 0);

        glColor3f( 1.0, 0.0, 0.0 );
        glLineWidth( 1.0 );
        for( i = 0; i < marker_num; i++ ) {
            argDrawSquare( marker_info[i].vertex, 0, 0 );
        }
        glLineWidth( 1.0 );
    }

    arVideoCapNext();

    if( (err=arMultiGetTransMat(marker_info, marker_num, config)) < 0 ) {
        argSwapBuffers();
        return;
    }
    if (arDebug) {
      printf("err = %f\n", err);
    }
    if(err > 100.0 ) {
        argSwapBuffers();
        return;
    }

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
    //for( i = 0; i < config->marker_num; i++ ) {
    //    if( config->marker[i].visible >= 0 ) draw( config->trans, config->marker[i].trans, 0 );
    //    else                                 draw( config->trans, config->marker[i].trans, 1 );
    //}

    for( i = 0; i < config->marker_num; i++ ) {
        if (config->marker[i].visible < 0) continue;

        printf("Will draw relative to %d\n", i);
        drawMarker(config->trans, config->marker[i].trans, 0);
        drawCube(config->trans, config->marker[i].trans, 100, 0.0);
        drawCube(config->trans, config->marker[i].trans, 0.0, 100);

        break;
    }

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

    if( (config = arMultiReadConfigFile(config_name)) == NULL ) {
        printf("config data load error !!\n");
        exit(0);
    }

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
