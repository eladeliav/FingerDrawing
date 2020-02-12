//
// Created by elad on 23/08/2019.
//
#include "../include/DrawingCam.hpp"

template<typename T>
T CLAMP(const T &value, const T &low = 0, const T &high = 255)
{
    return value < low ? low : (value > high ? high : value);
}

void overlayImage(Mat *src, Mat *overlay, const Point &location = Point(0, 0))
{
    for (int y = max(location.y, 0); y < src->rows; ++y)
    {
        int fY = y - location.y;

        if (fY >= overlay->rows)
            break;

        for (int x = max(location.x, 0); x < src->cols; ++x)
        {
            int fX = x - location.x;

            if (fX >= overlay->cols)
                break;

            double opacity = ((double) overlay->data[fY * overlay->step + fX * overlay->channels() + 3]) / 255;

            for (int c = 0; opacity > 0 && c < src->channels(); ++c)
            {
                unsigned char overlayPx = overlay->data[fY * overlay->step + fX * overlay->channels() + c];
                unsigned char srcPx = src->data[y * src->step + x * src->channels() + c];
                src->data[y * src->step + src->channels() * x + c] = srcPx * (1. - opacity) + overlayPx * opacity;
            }
        }
    }
}

DrawingCam::DrawingCam(int id, string ip, int port)
{
    cam_id = id;
    brushSize = 5;
    this->_ip = ip;
    this->_port = port;

    currentPointerPos = cv::Point(0, 0);

    eraserColor = ERASER_SCALAR;
    brushColor = BLUE_SCALAR;

    cam = cv::VideoCapture(cam_id);

    if (!cam.isOpened())
        throw std::runtime_error("Failed to open camera");

    cam >> frame;
    flip(frame, frame, 1);
    region_of_interest = Rect(frame.cols / 2, 0, frame.cols / 2, frame.rows / 1.5);
    roi = frame(region_of_interest);

    if (frame.empty())
        throw std::runtime_error("Received empty frame");

    foregroundExtractor = ForegroundExtractor();
    skinDetector = SkinDetector();

    canvas = cv::Mat(roi.size(), CV_8UC3);
    skinMask = cv::Mat(roi.size(), CV_8UC3);
    skinMask = eraserColor;
    canvas = eraserColor;
    foregroundExtractor.calibrate(roi);
    connectionManager = ConnectionManager();
}

void DrawingCam::sendPoint(const DrawPoint &p)
{
    if(!connectionManager.waiting() && drawingMode && !finishedCountdown)
    {
        connectionManager.sendPoint(p);
    }
}

void DrawingCam::getPoints()
{
    while(!connectionManager.waiting())
    {
        if(!drawingMode)
            continue;
        if(finishedCountdown)
            continue;
        DrawPoint p = connectionManager.getPoint();
        if(p.x == -1 && p.y == -1)
            canvas = eraserColor;
        else
            cv::circle(canvas, Point(p.x, p.y), p.size, p.color, FILLED);
    }
}

//void DrawingCam::start()
//{
//    for (char user_input = cv::waitKey(10); user_input != 27; user_input = cv::waitKey(10))
//    {
//        cam >> frame;
//        flip(frame, frame, 1);
//        roi = frame(region_of_interest);
//
//        rectangle(frame, region_of_interest, Scalar(255, 0, 0));
//        Mat displayCanvas;
//
//        foreground = foregroundExtractor.extractForeground(roi);
//
//        if(!skinDetector.sampled)
//            skinDetector.drawSampler(roi);
//        else
//            skinMask = skinDetector.genMask(foreground);
//
//
//        FacesRemover::removeFaces(roi, frame);
//        displayCanvas = canvas.clone();
//        fingerPoints = FingersDetector::countFingers(skinMask, vector<Mat *>{&displayCanvas, &roi});
//
//        draw();
//        overlayImage(&roi, &canvas);
//
//        std::string sizeAndColor = "Size: " + std::to_string(brushSize);
//        putText(displayCanvas, sizeAndColor, Point(0, 50), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255, 255));
//
//        cv::imshow(WINDOW_NAME, frame);
//        imshow("Foreground", foreground);
//        imshow("canvas", displayCanvas);
//        imshow("skin", skinMask);
//
//        if (user_input == '=' && brushSize < 25)
//            brushSize += 5;
//        else if (user_input == '-' && brushSize > 1)
//            brushSize -= 5;
//        else if (user_input == 'r')
//        {
//            canvas = eraserColor;
//            if(connected)
//                sendPoint(Point(-1, -1));
//        }
//        else if (user_input == 'e')
//            brushColor = eraserColor;
//        else if (user_input == 'b')
//            brushColor = cv::Scalar(250, 10, 10);
//        else if (user_input == 'c')
//            foregroundExtractor.calibrate(frame);
//        else if (user_input == 's')
//            skinDetector.sample(foreground);
//    }
//}

void DrawingCam::draw()
{
    if (fingerPoints.size() == 1)
    {
        currentPointerPos = fingerPoints.at(0);
        if (!Helpers::closePointExists(frame, currentPointerPos, 5))
        {
            cv::circle(canvas, currentPointerPos, brushSize, brushColor, FILLED);
            sendPoint(DrawPoint{currentPointerPos.x, currentPointerPos.y, brushSize, currentColor});
        }

    }
}

DrawingCam::~DrawingCam()
{
    cv::destroyAllWindows();
    cam.release();
}

Mat DrawingCam::getNextFrame(bool shouldFlip, Mat debugFrames[])
{
    if(!cam.isOpened())
        return Mat();
    cam >> frame;
    if(shouldFlip)
        flip(frame, frame, 1);
    roi = frame(region_of_interest);

    rectangle(frame, region_of_interest, Scalar(255, 0, 0));

    int y = 40;
    for(const auto& s : textToShow)
    {
        putText(frame, s, Point(0, y),
                FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255, 255));
        y += 30;
    }
    Mat displayCanvas;

    foreground = foregroundExtractor.extractForeground(roi);

    if(!skinDetector.sampled)
    {
        skinDetector.drawSampler(roi);
        skinMask = Mat(roi.size(), CV_8UC1);
        skinMask = eraserColor;
    }
    else
        skinMask = skinDetector.genMask(foreground);


    FacesRemover::removeFaces(roi, frame);
    displayCanvas = canvas.clone();
    fingerPoints = FingersDetector::countFingers(skinMask, vector<Mat *>{&displayCanvas, &roi});

    if(drawingMode)
    {
        draw();
        Mat transparent;
        cv::inRange(canvas, ERASER_SCALAR, ERASER_SCALAR, transparent);
        canvas.copyTo(roi, 255 - transparent);
    }
    else if(finishedCountdown)
    {
        int fingerNum = fingerPoints.size();
        std::cout << "FingerNum: " << fingerNum << std::endl;
        HandShape shape = INVALID;
        if(fingerNum == 0)
        {
            shape = ROCK;
        }else if(fingerNum == 1)
        {
            shape = PAPER;
        }
        else if(fingerNum == 2)
        {
            shape = SCISSORS;
        }
        textToShow.push_back("The Shape you made is: " + SHAPE_TO_STRING.at(shape));
        std::cout << "STRING SHAPE: " << SHAPE_TO_STRING.at(shape) << std::endl;
        connectionManager.sendHandShape(shape);
        HandShape otherShape = connectionManager.getHandShape();
        std::string otherShapeStr = SHAPE_TO_STRING.at(otherShape);
        textToShow.push_back("Opponent played: " + otherShapeStr);
        for(const auto& p : SHAPE_TO_STRING)
        {
            if(p.second == otherShapeStr)
                otherShape = p.first;
        }
        if(otherShape == ROCK && shape == PAPER)
            textToShow.push_back("You Win!");
        else if(otherShape == ROCK && shape == SCISSORS)
            textToShow.push_back("You Lose!");
        else if(otherShape == PAPER && shape == ROCK)
            textToShow.push_back("You Lose!");
        else if(otherShape == PAPER && shape == SCISSORS)
            textToShow.push_back("You Win!");
        else if(otherShape == SCISSORS && shape == ROCK)
            textToShow.push_back("You Win!");
        else if(otherShape == SCISSORS && shape == PAPER)
            textToShow.push_back("You Lose!");
        else
            textToShow.push_back("Draw!");
        finishedCountdown = false;
        drawingMode = true;
    }



    debugFrames[0] = Mat(foreground);
    debugFrames[1] = Mat(skinMask);
    return frame;
}

void DrawingCam::sampleSkinColor()
{
    skinDetector.sample(foreground);
}

void DrawingCam::resetSkinColor()
{
    skinDetector.resetThresholds();
}

void DrawingCam::calibrateBackground()
{
    foregroundExtractor.calibrate(roi);
}

void DrawingCam::resetCanvas(bool send)
{
    canvas = eraserColor;
    textToShow.clear();
    if(send)
        sendPoint({-1, -1, -1, BLUE});
}

bool DrawingCam::tryConnect(string ip, int port)
{
    return connectionManager.tryConnect(ip, port);
}

void DrawingCam::disconnect()
{
    return connectionManager.disconnect();
}

void DrawingCam::setColor(Color color)
{
    this->brushColor = COLOR_TO_SCALAR.at(color);
    currentColor = color;
}

void DrawingCam::toggleMode()
{
    if(connectionManager.waiting())
        return;
    this->drawingMode = false;
    resetCanvas(false);
    connectionManager.sendToggle();
    textToShow.clear();
    textToShow.push_back("Get Ready");
    textToShow.push_back(std::to_string(countdown));
    timer.setInterval(&DrawingCam::rockPaperCountdown, 1000, this);
}

void DrawingCam::rockPaperCountdown()
{
    std::cout << "Countdown: " << countdown << std::endl;
    textToShow.back() = std::to_string(countdown);
    countdown--;
    if (countdown == -1)
    {
        countdown = 5;
        finishedCountdown = true;
        textToShow.clear();
        timer.stop();
    }
}
