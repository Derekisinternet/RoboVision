#!/bin/sh
# an automated way to make CMakeLists, because I'm lazy:
PROJECT="roboVision"       
FILE=CMakeLists.txt
if [ -e $FILE ]
    then
        rm $FILE
fi
# Assumes the build location is set in an environment variable.
OPENCV_LOCATION="$OPENCV_LOCATION"
if [ $PROJECT == roboVision ]
    then
        printf 'file (GLOB SOURCES\n    src/*.cpp\n)\n' >> $FILE
fi
printf 'cmake_minimum_required(VERSION 2.8)\n' >> $FILE
printf "project( $PROJECT )\n" >> $FILE
if [ $PROJECT == roboVision ]
    then
        printf 'include_directories( include )\n' >> $FILE
fi
printf "set(OpenCV_DIR $OPENCV_LOCATION)\n" >> $FILE
printf 'find_package( OpenCV REQUIRED )\n' >> $FILE
printf "add_executable( $PROJECT \${SOURCES} )\n" >> $FILE
printf "add_definitions(-std=c++0x)\n" >> $FILE
printf "target_link_libraries( $PROJECT" >> $FILE
printf ' ${OpenCV_LIBS} )\n'  >> $FILE
