INC_DIR= /opt/ARToolKit/include
LIB_DIR= /opt/ARToolKit/lib

LDFLAG=-pthread -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0 -L/usr/X11R6/lib -L/usr/local/lib -L$(LIB_DIR)
LIBS= -lARgsub -lARvideo -lARMulti -lAR -lpthread -lglut -lGLU -lGL -lXi -lX11 -lm -pthread -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0
CFLAG= -O -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -g -I$(INC_DIR)

OBJS = scene.o object.o
HEADDERS = globals.h scene.h object.h

all: pucrs-ar-road

pucrs-ar-road: main.o $(OBJS)
	cc -o pucrs-ar-road main.o $(OBJS) $(LDFLAG) $(LIBS)

main.o: main.c $(HEADDERS) $(OBJS)
	cc -c $(CFLAG) main.c scene.o

scene.o: scene.c $(HEADDERS)
	cc -c $(CFLAG) scene.c

object.o: object.c $(HEADDERS)
	cc -c $(CFLAG) object.c

clean:
	rm -f *.o
	rm -f pucrs-ar-road
