//
// Created by elad on 23/08/2019.
//

#ifndef FINGERDRAWING_DRAWINGCAM_HPP
#define FINGERDRAWING_DRAWINGCAM_HPP

#include <iostream>
#include <opencv2/opencv.hpp>
#include "FingersDetector.hpp"

#define WINDOW_NAME "Frame"
#define OFFSET 60

void mouseCallBack(int event, int x, int y, int flags, void *userdata);

struct FrameAndValues
{
    FrameAndValues(Mat *frame, Scalar *lower, Scalar *upper) : frame(frame), lower(lower), upper(upper)
    {}

    cv::Mat* frame;
    Scalar* lower;
    Scalar* upper;
};

class DrawingCam
{
private:
    int cam_id;
    cv::VideoCapture cam;

    cv::Mat frame, canvas, gloveMask, hsv;

    cv::Point currentPointerPos;
    cv::Scalar brushColor, eraserColor;
    int brushSize;

    vector<cv::Point> fingerPoints;

    void draw();

    Scalar lower = Scalar(0, 0, 0);
    Scalar upper = Scalar(255, 255, 255);

public:
    DrawingCam(int id = 0);

    void start();
};

#endif //FINGERDRAWING_DRAWINGCAM_HPP
