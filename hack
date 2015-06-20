#!/bin/bash
set -x

xhost +

DEVICE="${DEVICE:-0}"

docker run -ti --rm \
           -e DISPLAY \
           -v /tmp/.X11-unix:/tmp/.X11-unix \
           -v /home/fabio/.Xauthority:/root/.Xauthority \
           --device /dev/video${DEVICE}:/dev/video0 \
           -v /home/fabio/projects/faculdade/pucrs-ar-road:/tmp/src \
           -w /tmp/src \
           fgrehm/artoolkit:2.72.1 \
           bash -c 'make && ./pucrs-ar-road'

xhost -
