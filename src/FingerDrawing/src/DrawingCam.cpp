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

    eraserColor = cv::Scalar(0, 0, 0);
    brushColor = cv::Scalar(250, 10, 10);

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

    sock = UniSocket(ip, port);
    thread getPointsThread(&DrawingCam::getPoints, this);
    getPointsThread.detach();

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
        string msg = "X:" + std::to_string(p.x) + "Y:" + std::to_string(p.y) + "S:" + std::to_string(brushSize) + "END";
        std::cout << "about to send point: " << msg << std::endl;
        sock.send(msg);
    }
    else
    {
        std::cout << "Socket invalid can't send to peers" << std::endl;
    }
}

void DrawingCam::getPoints()
{
    static char buffer[DEFAULT_BUFFER_LEN] = {0};
    while(sock.valid())
    {
        try
        {
            int bytesReceived = sock.recv(buffer);
            if(bytesReceived > 0)
            {
                string xS, yS, sS;
                string msg = buffer;
                xS = msg.substr(msg.find("X:") + 2, msg.find("Y:"));
                yS = msg.substr(msg.find("Y:") + 2, msg.rfind("S:"));
                sS = msg.substr(msg.rfind("S:") + 2, msg.rfind("END"));
                int x, y, s;
                try
                {
                    x = std::stoi(xS);
                    y = std::stoi(yS);
                    s = std::stoi(sS);
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
                UniSocket::cleanup();
                exit(1);
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

        if(!skinDetector.alreadySampled())
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
    cv::destroyAllWindows();
    cam.release();
}

void DrawingCam::draw()
{
    if (fingerPoints.size() == 1)
    {
        currentPointerPos = fingerPoints.at(0);
        if (!Helpers::closePointExists(frame, currentPointerPos, 5))
        {
            cv::circle(canvas, currentPointerPos, brushSize, brushColor, FILLED);
            sendPoint(currentPointerPos);
        }

    }
}