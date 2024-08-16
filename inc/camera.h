#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <stdio.h>
#include <k4a/k4a.hpp>
#include <k4arecord/record.h>
#include <k4arecord/playback.h>
#include "OpencvHead.h"

#include <cstdlib>

class camera
{
public:
    void init_kinect(k4a::capture &capture, k4a::transformation &k4aTransformation, k4a::calibration &k4aCalibration);
    void pictureTransformation(k4a::capture &capture, cv::Mat &rgbFrame, cv::Mat &depthFrame, k4a::transformation &k4aTransformation);
    void stopCamera();
    void picture_update(k4a::capture &capture);
    cv::Mat *getpicture(k4a::capture &capture, cv::Mat &cv_color, k4a::transformation &k4aTransformation);

private:
    u_int32_t device_count;
    k4a::device device;
    k4a_device_configuration_t init;

    k4a::image k4a_color;
    k4a::image k4a_depth;
    k4a::image k4a_infrared;
    k4a::image k4a_tf_depth;
};
#endif