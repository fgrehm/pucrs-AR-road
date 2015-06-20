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

ARParam  cparam;

int xsize, ysize;

char *config_name = "Data/road-markers/marker.dat";
char *cparam_name = "Data/camera_para.dat";

char         *model_name = "Data/road-markers/buildings.dat";
ObjectData_T *object;
int          objectnum;
static int draw( ObjectData_T *object, int objectnum );
static int  draw_object( int obj_id, double gl_para[16] );

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

    if( key == 'b' ) {
        showBuildings = !showBuildings;
        printf("*** showBuildings toggled to %d\n", showBuildings);
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
    static int ms_prev;
    int ms;
    float s_elapsed;

    // Find out how long since mainLoop() last ran.
    ms = glutGet(GLUT_ELAPSED_TIME);
    s_elapsed = (float)(ms - ms_prev) * 0.001;
    if (s_elapsed < 0.01f) return; // Don't update more often than 100 Hz.
    ms_prev = ms;

    if (arDebug) {
      printf("*** %d (ms running)\n", ms);
    }

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

    glLineWidth( 1.5 );
    for( i = 0; i < marker_num; i++ ) {
        glColor3f( green[0], green[1], green[2] );
        argDrawSquare( marker_info[i].vertex, 0, 0 );
    }

    /* check for known patterns */
    for( i = 0; i < objectnum; i++ ) {
        int k = -1;
        int j;
        for( j = 0; j < marker_num; j++ ) {
            if( object[i].id == marker_info[j].id) {
              /* you've found a pattern */
              //printf("Found pattern: %d ",patt_id);
              if( k == -1 ) k = j;
              else /* make sure you have the best pattern (highest confidence factor) */
                if( marker_info[k].cf < marker_info[j].cf ) k = j;
            }
        }
        if( k == -1 ) {
            object[i].visible = 0;
            continue;
        }

        /* calculate the transform for each marker */
        if( object[i].visible == 0 ) {
            arGetTransMat(&marker_info[k],
                object[i].marker_center, object[i].marker_width,
                object[i].trans);
        }
        else {
          arGetTransMatCont(&marker_info[k], object[i].trans,
                object[i].marker_center, object[i].marker_width,
                object[i].trans);
        }
        object[i].visible = 1;
    }

    arVideoCapNext();

    /* draw the AR graphics */
    draw( object, objectnum );

    if( (err=arMultiGetTransMat(marker_info, marker_num, multiMarkerConfig)) < 0 ) {
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

    drawScene(ms);

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
    if( (object=read_ObjData(model_name, &objectnum)) == NULL ) {
        printf("ObjData data load error !!\n");
        exit(0);
    }
    printf("Objectfile num = %d\n", objectnum);

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

/* draw the the AR objects */
static int draw( ObjectData_T *object, int objectnum )
{
    int     i;
    double  gl_para[16];

	glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);

    /* calculate the viewing parameters - gl_para */
    for( i = 0; i < objectnum; i++ ) {
        if( object[i].visible == 0 ) continue;
        argConvGlpara(object[i].trans, gl_para);
        draw_object( object[i].id, gl_para);
    }

	glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );

    return(0);
}

/* draw the user object */
static int  draw_object( int obj_id, double gl_para[16])
{
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

	if(obj_id == 0){
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash_collide);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_collide);
		/* draw a cube */
		glTranslatef( 0.0, 0.0, 30.0 );
		glutSolidSphere(30,12,6);
	}
	else {
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		/* draw a cube */
		glTranslatef( 0.0, 0.0, 30.0 );
		glutSolidCube(60);
	}

    argDrawMode2D();

    return 0;
}
