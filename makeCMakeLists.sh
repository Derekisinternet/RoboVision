#!/usr/bin/env bash
# an automated way to make CMakeLists, because I'm lazy:

if [ $# -eq 0 ]
    then
        echo 'No Project name specified'
    else
        while [[ $# -gt 0 ]]
        do
            key="$1"

            case $key in
                -n|--name)
                PROJECT="$2"
                shift
                shift
                ;;
                -r|--release)
                PROJECT="roboVision"
                shift
                ;;
            esac
        done
        
        if [ -z $PROJECT ]
            then
                echo "Project name not set"
                exit 1
        fi
        
        FILE=CMakeLists.txt
        if [ -e $FILE ]
            then
                rm $FILE
        fi

        OPENCV_LOCATION="$HOME/opencv/build"
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
fi
