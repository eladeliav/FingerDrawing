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
#include "ConnectionManager.hpp"
#include "timer.hpp"
#include <vector>
#include <thread>
#include <string>
#include <map>

#define WINDOW_NAME "Frame"

using std::vector;
using std::thread;
using std::string;

class DrawingCam
{
private:
    int cam_id;
    cv::VideoCapture cam;
    string _ip;
    int _port = -1;

    Rect region_of_interest;
    cv::Mat frame, canvas, foreground, skinMask, roi;

    cv::Point currentPointerPos;
    cv::Scalar brushColor, eraserColor;
    Color currentColor = BLUE;
    int brushSize;

    vector<cv::Point> fingerPoints;
    vector<std::string> textToShow;

    ForegroundExtractor foregroundExtractor;
    SkinDetector skinDetector;

    ConnectionManager connectionManager;
    bool drawingMode = true;
    Timer<DrawingCam> timer = Timer<DrawingCam>();
    int countdown = 5;
    bool finishedCountdown = false;

    void draw();

    void sendPoint(const DrawPoint &p);
    void getPoints();
    void rockPaperCountdown();

public:
    DrawingCam(int id = 0, string ip="127.0.0.1", int port=1234);
    ~DrawingCam();
    void start();
    Mat getNextFrame(bool shouldFlip, Mat debugFrames[]);
    void sampleSkinColor();
    void resetSkinColor();
    void calibrateBackground();
    void resetCanvas(bool send=true);
    void setColor(Color color);
    bool tryConnect(string ip, int port);
    void disconnect();
    void toggleMode(bool send=true);
    bool connected();
};

#endif //FINGERDRAWING_DRAWINGCAM_HPP
