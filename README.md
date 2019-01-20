# RoboVision

A computer vision app that uses the OpenCV framework.

## Dependencies

* [OpenCV](https://github.com/opencv/opencv) version 4.0.1
* [Darknet](https://pjreddie.com/darknet/)

## Setup

### Linux (Debian)

* run install_dependencies.sh
* `mkdir build && cd build`
* `make`

### Windows

* install linux
* follow the `Linux` install steps

## Training a Classifier

* `cd build/`
* `./roboVision -c <className>`
* this will open up a video capture window.
  * if the window is black, make sure you have a webcam and that it has adequate light to see.
* press `Spacebar` to start/pause video recording
* once you have enough footage, press `Esc` to exit the recording window, and enter the playback window.
* The playback window will display your recorded video frame by frame. For each frame, use your mouse to draw a box around the object you want to classify
  * Left-click to start/stop drawing a box
  * right-click inside a box to dismiss it.
* Once you have identified all the objects in that frame, press `Spacebar` to iterate to the next frame. The app will save that frame into a folder named `<className>` along with a text file with the coordinates of the bounding boxes.
  * if a frame has no bounding boxes when you iterate, it will not be saved.
* ???
* profit.
