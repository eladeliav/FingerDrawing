//
// Created by elad on 23/08/2019.
//
#include "../include/DrawingCam.hpp"

// constructor
DrawingCam::DrawingCam(int id, string ip, int port)
{
    // sets up camera
    cam_id = id;
    brushSize = DEFAULT_BRUSH_SIZE; // default brush size
    this->_ip = ip;
    this->_port = port;

    // sets everything to default values
    currentPointerPos = cv::Point(0, 0);

    eraserColor = ERASER_SCALAR;
    brushColor = BLUE_SCALAR;

    // starts up camera
    cam = cv::VideoCapture(cam_id);

    // checks camera opened properly
    if (!cam.isOpened())
        throw std::runtime_error("Failed to open camera");

    // gets intial frame to setup sizes
    cam >> frame;

    //checks frame is valid
    if (frame.empty())
        throw std::runtime_error("Received empty frame");

    // flips frame if needed
    flip(frame, frame, 1);
    // sets up region of interest
    region_of_interest = Rect(frame.cols / 2, 0, frame.cols / 2, frame.rows / 1.5);
    roi = frame(region_of_interest);


    // inits foreground extractor and skin detector
    foregroundExtractor = ForegroundExtractor();
    skinDetector = SkinDetector();

    // more default values
    canvas = cv::Mat(roi.size(), CV_8UC3);
    skinMask = cv::Mat(roi.size(), CV_8UC3);
    skinMask = eraserColor;
    canvas = eraserColor;
    // calibrates foreground extractor
    foregroundExtractor.calibrate(roi);
    // starts connection manager
    connectionManager = ConnectionManager();
}

// sends point via connection manager
void DrawingCam::sendPoint(const DrawPoint &p)
{
    // if can send, and in drawing mode
    if (!connectionManager.waiting() && drawingMode && !finishedCountdown)
    {
        // send point
        connectionManager.sendPoint(p);
    }
}

// Constantly gets points from server and applies to canvas
void DrawingCam::getPoints()
{
    // while connected
    while (connectionManager.connected())
    {
        // get next point
        DrawPoint p = connectionManager.getPoint();
        if(!connectionManager.connected()) // disconnect if not connected
        {
            textToShow.clear();
            textToShow.push_back("Disconnected");
            return;
        }
        if (p.toggle) // if toggles mode, locally change mode
            toggleMode(false);
        if (!drawingMode) // if not in drawing mode, don't draw (clearing socket buffer without drawing)
            continue;
        // -1, -1 point symbolizes a clear canvas command
        if (p.x == -1 && p.y == -1)
            canvas = eraserColor;
        else
            // draw point in given coords and color and size
            cv::circle(canvas, Point(p.x, p.y), p.size, COLOR_TO_SCALAR.at(p.color), FILLED);
    }
}

// draws if only one finger up (at point of finger tip)
void DrawingCam::draw()
{
    // only one finger up
    if (fingerPoints.size() == 1)
    {
        currentPointerPos = fingerPoints.at(0); // fingertip point
        if (!Helpers::closePointExists(frame, currentPointerPos, 5))
        {
            // draws at point with current size and color
            cv::circle(canvas, currentPointerPos, brushSize, brushColor, FILLED);
            sendPoint(DrawPoint{currentPointerPos.x, currentPointerPos.y, brushSize, currentColor}); // sends to server
        }

    }
}

// destructor that releases camera and closes opencv debug windows
DrawingCam::~DrawingCam()
{
    cv::destroyAllWindows();
    cam.release();
}

// calculates next frame to display
Mat DrawingCam::getNextFrame(bool shouldFlip)
{
    // webcam is opened
    if (!cam.isOpened())
        return frame;
    cam >> frame;
    // get frame
    if(frame.empty()) // check frame is valid
        return frame;
    if (shouldFlip) // flip if should
        flip(frame, frame, 1);
    roi = frame(region_of_interest); // set up roi to new frame

    // draw roi so user can see
    rectangle(frame, region_of_interest, Scalar(255, 0, 0));

    // display text from vector of textToShow
    int y = 40;
    for (const auto &s : textToShow)
    {
        putText(frame, s, Point(0, y),
                FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 255));
        y += 30; // move down
    }
    Mat displayCanvas; // canvas to display

    foreground = foregroundExtractor.extractForeground(roi); // extracts foreground
    if(foreground.empty()) // makes sure is valid
        return frame;

    if (!skinDetector.sampled) // show skin sampelrs if needed
    {
        skinDetector.drawSampler(roi);
        skinMask = Mat(roi.size(), CV_8UC1);
        skinMask = eraserColor;
    } else
        skinMask = skinDetector.genMask(foreground); // otherwise generate skin mask from sampled skin color


    FacesRemover::removeFaces(roi, frame); // detects faces in roi, and tells user to remove
    displayCanvas = canvas.clone(); // copies canvas
    // counts fingers
    fingerPoints = FingersDetector::countFingers(skinMask, vector<Mat *>{&displayCanvas, &roi});

    if (drawingMode)
    {
        // drawingmode -> draw and copy the drawing canvas to the display frame
        draw();
        Mat transparent;
        cv::inRange(canvas, ERASER_SCALAR, ERASER_SCALAR, transparent);
        canvas.copyTo(roi, 255 - transparent);
    } else if (finishedCountdown) // finishedcountdown (rock paper scissors mode)
    {
        // check results
        int fingerNum = fingerPoints.size();
        std::cout << "FingerNum: " << fingerNum << std::endl;
        HandShape shape = INVALID;
        if (fingerNum == 0) // 0 fingers up is rock
        {
            shape = ROCK;
        }else if(fingerNum == 1 || fingerNum > 2) // 1 or more than 2 fingers up is paper
        {
            shape = PAPER;
        } else if (fingerNum == 2) // 2 fingers  is scissors
        {
            shape = SCISSORS;
        }
        // display text
        textToShow.push_back("The Shape you made is: " + SHAPE_TO_STRING.at(shape));
        connectionManager.sendHandShape(shape); // send shape to server
        HandShape otherShape = connectionManager.getHandShape(); // receive partners shape
        std::string otherShapeStr = SHAPE_TO_STRING.at(otherShape); // converst to string to display
        textToShow.push_back("Opponent played: " + otherShapeStr);
        for (const auto &p : SHAPE_TO_STRING)
        {
            if (p.second == otherShapeStr)
                otherShape = p.first;
        }
        // check winner
        if (otherShape == ROCK && shape == PAPER)
            textToShow.push_back("You Win!");
        else if (otherShape == ROCK && shape == SCISSORS)
            textToShow.push_back("You Lose!");
        else if (otherShape == PAPER && shape == ROCK)
            textToShow.push_back("You Lose!");
        else if (otherShape == PAPER && shape == SCISSORS)
            textToShow.push_back("You Win!");
        else if (otherShape == SCISSORS && shape == ROCK)
            textToShow.push_back("You Win!");
        else if (otherShape == SCISSORS && shape == PAPER)
            textToShow.push_back("You Lose!");
        else
            textToShow.push_back("Draw!");
        // reset variables to drawing mode
        finishedCountdown = false;
        drawingMode = true;
    }
    return frame;
}

// sample skin color on foreground
void DrawingCam::sampleSkinColor()
{
    skinDetector.sample(foreground);
}

// resets sampled skin color
void DrawingCam::resetSkinColor()
{
    skinDetector.resetThresholds();
}

// calibrates background
void DrawingCam::calibrateBackground()
{
    foregroundExtractor.calibrate(roi);
}

// reset canvas
void DrawingCam::resetCanvas(bool send)
{
    // do nothing if rock paper scissors mode
    if(finishedCountdown || !drawingMode)
        return;
    // erase canvas
    canvas = eraserColor;
    // clear text on screen
    if(!textToShow.empty() && textToShow.front() != WAITING_MSG)
        textToShow.clear();
    // send if needed to
    if (send)
        sendPoint({-1, -1, -1, BLUE});
}

// tries to connect to server
bool DrawingCam::tryConnect(string ip, int port)
{
    // try to connect on connection manager
    bool s = connectionManager.tryConnect(ip, port);
    textToShow.clear(); // clears text
    if (s) // successfully connected
    {
        // start get points thread
        thread getPointsThread(&DrawingCam::getPoints, this);
        getPointsThread.detach();
        textToShow.push_back(WAITING_MSG); // waiting for players message
        timer.setInterval([](DrawingCam *c) // check if partner connected every second until partner connected
                      {
                          if(!c->connectionManager.waiting())
                          {
                              c->textToShow.front() = "Partner Connected!"; // display that partner connected
                              c->timer.stop();
                          }
                      }, 1000, this);
    } else
        textToShow.push_back("Failed to Connect"); // failed to connect

    return s;
}

void DrawingCam::disconnect()
{
    // disconnect (if connected)
    if (connected())
    {
        textToShow.clear();
        textToShow.push_back("Disconnected");
        connectionManager.disconnect();
    }

}

// sets color to a new given color
void DrawingCam::setColor(Color color)
{
    // sets brush color
    this->brushColor = COLOR_TO_SCALAR.at(color);
    currentColor = color;
}

// toggles mode (drawing to rock paper scissors)
void DrawingCam::toggleMode(bool send)
{
    if (connectionManager.waiting()) // if no partner, ignore
        return;
    this->drawingMode = false;  // turns of drawing mode
    resetCanvas(false); // resets canvas locally
    if (send) // sends toggle mode to partner (if needed)
        connectionManager.sendToggle();
    textToShow.clear(); // show needed text
    textToShow.push_back("Get Ready");
    textToShow.push_back(std::to_string(countdown));
    timer.setInterval([](DrawingCam *c) // start countdown
                      {
                          std::cout << "Countdown: " << c->countdown << std::endl;
                          c->textToShow.back() = std::to_string(c->countdown); // show countdown
                          c->countdown--;
                          if (c->countdown == -1) // reached 0 (in display)
                          {
                              c->countdown = 5; // reset variables and turn on finished countdown
                              c->finishedCountdown = true;
                              c->textToShow.clear();
                              c->timer.stop();
                          }
                      }, 1000, this);
}

// checks if connected to server
bool DrawingCam::connected()
{
    return connectionManager.connected();
}

// sets brush size
void DrawingCam::setSize(int size)
{
    if(size < 5 || size > 20)
        return;
    this->brushSize = size;
}

// increments brush size by 1
void DrawingCam::incSize()
{
    if(brushSize + 1 <= 20)
        brushSize++;
}

// decreases brush size by 1
void DrawingCam::decSize()
{
    if(brushSize -1 >= 5)
        brushSize--;
}
