#!/bin/bash

xhost +

docker run -ti --rm --privileged \
           -e DISPLAY \
           -v /tmp/.X11-unix:/tmp/.X11-unix \
           -v /home/fabio/.Xauthority:/root/.Xauthority \
           --device /dev/video0 \
           -v /home/fabio/projects/faculdade/pucrs-ar-road:/tmp/src \
           -w /tmp/src \
           fgrehm/artoolkit:2.72.1

xhost -
