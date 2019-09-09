//
// Created by elad on 23/08/2019.
//

#ifndef FINGERDRAWING_DRAWINGCAM_HPP
#define FINGERDRAWING_DRAWINGCAM_HPP

#include <iostream>
#include <opencv2/opencv.hpp>
#include "FingersDetector.hpp"
#include "Communicator.hpp"
#include <vector>
#include <thread>

#define WINDOW_NAME "Frame"
#define OFFSET 60

using std::vector;

void mouseCallBack(int event, int x, int y, int flags, void *userdata);

struct FrameAndValues
{
    FrameAndValues(Mat *frame, Scalar *lower, Scalar *upper) : frame(frame), lower(lower), upper(upper)
    {}

    cv::Mat *frame;
    Scalar *lower;
    Scalar *upper;
};

class DrawingCam
{
private:
    int cam_id;
    cv::VideoCapture cam;

    cv::Mat frame, canvas, gloveMask, hsv;

    cv::Point currentPointerPos = Point(0, 0);
    cv::Scalar brushColor = cv::Scalar(250, 10, 10), eraserColor = cv::Scalar(0, 0, 0);
    int brushSize = 5;

    vector<cv::Point> fingerPoints;

    Scalar lower = Scalar(0, 0, 0);
    Scalar upper = Scalar(255, 255, 255);

    //Communicator communicator;

    void draw();
    void initCamera();
    void getPeerPoints();

public:
    DrawingCam(int id = 1);

    void start();
};

#endif //FINGERDRAWING_DRAWINGCAM_HPP
