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
}

void DrawingCam::sendPoint(const Point& p)
{
    if(sock.valid())
    {
        string msg = "X:" + std::to_string(p.x) + "Y:" + std::to_string(p.y) + "S:" + std::to_string(brushSize) + "C:" + COLOR_TO_STRING.at(currentColor) + "END";
        std::cout << "about to send point: " << msg << std::endl;
        try
        {
            sock.send(msg);
        }
        catch(UniSocketException& e)
        {
            std::cout << e << std::endl;
            sock.close();
            connected = false;
        }

    }
    else
    {
        std::cout << "Socket invalid can't send to peers" << std::endl;
    }
}

void DrawingCam::getPoints()
{
    static char buffer[DEFAULT_BUFFER_LEN] = {0};
    while(sock.valid() && connected)
    {
        memset(buffer, 0, sizeof(buffer));
        if(!drawingMode)
            continue;
        try
        {
            int bytesReceived = sock.recv(buffer);
            if(bytesReceived > 0)
            {
                string xS, yS, sS, cS;
                string msg = buffer;
                if(msg.find("TOGGLE") != std::string::npos)
                {
                    std::cout << "RECEIVED TOGGLE MSG" << std::endl;
                    this->toggleMode();
                    continue;
                }
                xS = msg.substr(msg.find("X:") + 2, msg.find("Y:"));
                yS = msg.substr(msg.find("Y:") + 2, msg.rfind("S:"));
                sS = msg.substr(msg.rfind("S:") + 2, msg.rfind("C:"));
                cS = msg.substr(msg.rfind("C:") + 2, msg.rfind("END"));
                int x, y, s;
                try
                {
                    x = std::stoi(xS);
                    y = std::stoi(yS);
                    s = std::stoi(sS);
                    Color newColor = currentColor;
                    for(auto it : COLOR_TO_STRING)
                        if(it.second == cS)
                            newColor = it.first;
                    this->brushColor = COLOR_TO_SCALAR.at(newColor);
                    if(x == -1 && y == -1)
                        canvas = eraserColor;
                    else
                        cv::circle(canvas, Point(x, y), s, brushColor, FILLED);
                }catch(std::invalid_argument& e)
                {
                    std::cout << e.what() << std::endl;
                }

            }
        }catch(UniSocketException& e)
        {
            if(e.getErrorType() != UniSocketException::TIMED_OUT)
            {
                std::cout << e << std::endl;
                sock.close();
                connected = false;
                //UniSocket::cleanup();
                //exit(1);
            }
        }


    }
}

void DrawingCam::start()
{
    for (char user_input = cv::waitKey(10); user_input != 27; user_input = cv::waitKey(10))
    {
        cam >> frame;
        flip(frame, frame, 1);
        roi = frame(region_of_interest);

        rectangle(frame, region_of_interest, Scalar(255, 0, 0));
        Mat displayCanvas;

        foreground = foregroundExtractor.extractForeground(roi);

        if(!skinDetector.sampled)
            skinDetector.drawSampler(roi);
        else
            skinMask = skinDetector.genMask(foreground);


        FacesRemover::removeFaces(roi, frame);
        displayCanvas = canvas.clone();
        fingerPoints = FingersDetector::countFingers(skinMask, vector<Mat *>{&displayCanvas, &roi});

        draw();
        overlayImage(&roi, &canvas);

        std::string sizeAndColor = "Size: " + std::to_string(brushSize);
        putText(displayCanvas, sizeAndColor, Point(0, 50), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255, 255));

        cv::imshow(WINDOW_NAME, frame);
        imshow("Foreground", foreground);
        imshow("canvas", displayCanvas);
        imshow("skin", skinMask);

        if (user_input == '=' && brushSize < 25)
            brushSize += 5;
        else if (user_input == '-' && brushSize > 1)
            brushSize -= 5;
        else if (user_input == 'r')
        {
            canvas = eraserColor;
            if(connected)
                sendPoint(Point(-1, -1));
        }
        else if (user_input == 'e')
            brushColor = eraserColor;
        else if (user_input == 'b')
            brushColor = cv::Scalar(250, 10, 10);
        else if (user_input == 'c')
            foregroundExtractor.calibrate(frame);
        else if (user_input == 's')
            skinDetector.sample(foreground);
    }
}

void DrawingCam::draw()
{
    if (fingerPoints.size() == 1)
    {
        currentPointerPos = fingerPoints.at(0);
        if (!Helpers::closePointExists(frame, currentPointerPos, 5))
        {
            cv::circle(canvas, currentPointerPos, brushSize, brushColor, FILLED);
            if(connected)
                sendPoint(currentPointerPos);
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
        HandShape shape = ROCK;
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
        if(connected)
        {
            this->sock.send(SHAPE_TO_STRING.at(shape));
            std::string otherShapeStr;
            HandShape otherShape = ROCK;
            char buf[DEFAULT_BUFFER_LEN]= {'\0'};
            this->sock.recv(buf);
            otherShapeStr = buf;
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
        }
        finishedCountdown = false;
        drawingMode = true;
    }



    debugFrames[0] = Mat(foreground);
    debugFrames[1] = Mat(skinMask);
//    cv::imshow(WINDOW_NAME, frame);
//    if(showDebug)
//    {
//        imshow("Foreground", foreground);
//        imshow("canvas", displayCanvas);
//        imshow("skin", skinMask);
//    }
//    else
//        destroyAllWindows();

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
    if(connected && send)
        sendPoint(Point(-1, -1));
}

bool DrawingCam::tryConnect(string ip, int port)
{
    try
    {
        sock = UniSocket(ip, port);
        if(sock.valid())
            connected = true;
    }catch(UniSocketException& e)
    {
        std::cout << e << std::endl;
        std::cout << "Staying in offline mode" << std::endl;
    }

    if(connected)
    {
        thread getPointsThread(&DrawingCam::getPoints, this);
        getPointsThread.detach();
    }
    return connected;
}

void DrawingCam::disconnect()
{
    connected = false;
    sock.close();
}

void DrawingCam::setColor(Color color)
{
    this->brushColor = COLOR_TO_SCALAR.at(color);
    currentColor = color;
}

void DrawingCam::toggleMode(bool send)
{
    this->drawingMode = false;
    resetCanvas(false);
    if(connected && send)
        this->sock.send("TOGGLE");
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
