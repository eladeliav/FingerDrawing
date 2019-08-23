//
// Created by elad on 23/08/2019.
//

#ifndef FINGERDRAWING_DRAWINGCAM_HPP
#define FINGERDRAWING_DRAWINGCAM_HPP

#include <iostream>
#include <opencv2/opencv.hpp>
#include "ForegroundExtractor.hpp"
#include "SkinDetector.hpp"
#include "FacesRemover.hpp"
#include "FingerCounter.hpp"

#define WINDOW_NAME "Frame"


class DrawingCam
{
private:
    int cam_id;
    cv::VideoCapture cam;

    cv::Mat frame, frameClone, canvas, foreground, skinMask;

    cv::Point currentPointerPos, lastPointerPos;
    cv::Scalar brushColor, eraserColor;
    int brushSize;

    ForegroundExtractor foregroundExtractor;
    SkinDetector skinDetector;
    FingerCounter fingerCounter;

    vector<cv::Point> fingerPoints;

    void draw();

public:
    DrawingCam(int id = 0);

    void start();
};

#endif //FINGERDRAWING_DRAWINGCAM_HPP
