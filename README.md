# AR-road

Code for an Augmented Reality assignment for the PUCRS Computer Graphics II 2015.1 course.
Built with [ARToolKit](http://www.hitl.washington.edu/artoolkit/).

## Building project using Docker

```sh
./hack

# From within the container:
make
```

## Notes and links for usage with Docker

```
http://www.upubuntu.com/2012/04/webcamstudio-free-virtual-webcam.html
mapping one cam to another in the container

https://github.com/umlaeute/v4l2loopback

http://askubuntu.com/questions/186003/anything-better-than-cheese-for-video-capture


sudo apt-get install v4l2loopback-dkms

sudo apt-add-repository ppa:mc3man/trusty-media
sudo apt-get update
sudo apt-get install -y ffmpeg

ffmpeg -re -i FILE -f v4l2 /dev/video1
slow motion
ffmpeg -re -i samples/a-to-f-back.avi -vf "setpts=0.05*PTS" -f v4l2 /dev/video1
```
