#!/bin/bash

# TODO: make an install for OSX. Need brew -> ffmpeg 

echo "Running apt update . . ."
sudo apt update
echo "Running apt uprade . . . "
sudo apt upgrade

echo "Installing OpenCV dependencies . . . "
sudo apt install build-essential\
     gcc cmake git libgtk2.0-dev pkg-config \
     libavcodec-dev libavformat-dev libswscale-dev \
     python-dev python-numpy libtbb2 libtbb-dev \
     libjpeg-dev  libtiff-dev libjasper-dev libdc1394-22-dev

cd $HOME && git clone https://github.com/opencv/opencv.git

cd opencv && mkdir build && cd build

cmake -D CMAKE_BUILD_TYPE=RELEASE ..
make

echo "Installing Darknet . . ."
cd $HOME && git clone https://github.com/pjreddie/darknet.git
cd darknet && make

echo "Installation finished. You're all set."