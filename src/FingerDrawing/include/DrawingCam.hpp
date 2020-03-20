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

// Constant for waiting for partner message
#define WAITING_MSG "Connected! Waiting for Partner..."
#define DEFAULT_PORT 1234 // default port
#define DEFAULT_BRUSH_SIZE 5 // default brush size

// using command...
using std::vector;
using std::thread;
using std::string;

class DrawingCam
{
private:
    int cam_id; // camera port id
    cv::VideoCapture cam; // Cam object
    string _ip; // the ip
    int _port = -1; // the port

    Rect region_of_interest; // region of interest
    cv::Mat frame, canvas, foreground, skinMask, roi; // various frames

    cv::Point currentPointerPos; // current position of mouse
    cv::Scalar brushColor, eraserColor;  // brush and eraser color
    Color currentColor = BLUE; // current brush color
    int brushSize; // brush size

    vector<cv::Point> fingerPoints; // vector of points where fingers were detected
    vector<std::string> textToShow; // text to display top left

    ForegroundExtractor foregroundExtractor; // foreground extractor object
    SkinDetector skinDetector; // skin detector

    ConnectionManager connectionManager; // connection manager
    bool drawingMode = true; // current mode
    Timer<DrawingCam> timer = Timer<DrawingCam>(); // Timer object
    int countdown = 5; // countdown for rock paper scissors
    bool finishedCountdown = false; // whether or not the countdown is finished

    void draw(); // draws at current color, size and mouse pointer

    void sendPoint(const DrawPoint &p); // sends point to server

    void getPoints(); // gets point loop

public:
    // constructor
    DrawingCam(int id = 0, string ip = "127.0.0.1", int port = DEFAULT_PORT);

    // destructor
    ~DrawingCam();

    // calculates next frame details
    Mat getNextFrame(bool shouldFlip);

    // samples skin color of user and saves it
    void sampleSkinColor();

    // resets previously sampled skin color
    void resetSkinColor();

    // calibrates backgound
    void calibrateBackground();

    // resets drawings on canvas
    void resetCanvas(bool send = true);

    // Sets brush color
    void setColor(Color color);

    // Sets brush size
    void setSize(int size);

    // increments brush size by 1
    void incSize();

    // decrements size by 1
    void decSize();

    // tries to connect to server
    bool tryConnect(string ip, int port);

    // disconnects
    void disconnect();

    // toggles drawing mode
    void toggleMode(bool send = true);

    // checks if connected
    bool connected();
};

#endif //FINGERDRAWING_DRAWINGCAM_HPP
