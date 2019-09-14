//
// Created by elad on 23/08/2019.
//

#ifndef FINGERDRAWING_DRAWINGCAM_HPP
#define FINGERDRAWING_DRAWINGCAM_HPP

#include <iostream>
#include <opencv2/opencv.hpp>
#include <UniSockets/Core.hpp>
#include "FingersDetector.hpp"
#include "ForegroundExtractor.hpp"
#include "SkinDetector.hpp"
#include "FacesRemover.hpp"
#include <vector>
#include <thread>
#include <string>

#define WINDOW_NAME "Frame"
#define OFFSET 60

using std::vector;
using std::thread;
using std::string;

class DrawingCam
{
private:
    int cam_id;
    cv::VideoCapture cam;

    Rect region_of_interest;
    cv::Mat frame, canvas, foreground, skinMask, roi;

    cv::Point currentPointerPos;
    cv::Scalar brushColor, eraserColor;
    int brushSize;

    vector<cv::Point> fingerPoints;

    ForegroundExtractor foregroundExtractor;
    SkinDetector skinDetector;

    UniSocket sock;

    void draw();

    void sendPoint(const Point& p);
    void getPoints();

public:
    DrawingCam(int id = 0, string ip="127.0.0.1", int port=1234);
    ~DrawingCam();
    void start();
    Mat getNextFrame(bool shouldFlip, bool showDebug);
    void sampleSkinColor();
    void resetSkinColor();
    void calibrateBackground();
    void resetCanvas();
};

#endif //FINGERDRAWING_DRAWINGCAM_HPP
