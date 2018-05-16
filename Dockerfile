# First attempt. Don't judge me too harshly

FROM alpine

MAINTAINER Derek 

# install tools
RUN apk add --update alpine-sdk cmake

# install opencv
RUN git clone https://github.com/opencv/opencv.git; cd opencv && \
    mkdir -p opencv/release; cd opencv/release; \
    cmake -D CMAKE_BUILD_TYPE=release ..; \
    make

# create working directory
WORKDIR /app
ADD . /app
RUN cd  /app/build && \
    ./makeCMakeLists.sh -r && \
    cmake -D CMAKE_INSTALL_PREFIX=/opencv/release .. && \
    make

# CMD ["/app/build/roboVision"]
