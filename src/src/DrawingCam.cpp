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

void DrawingCam::initCamera()
{
    cam = cv::VideoCapture(cam_id);

    if (!cam.isOpened())
        throw std::runtime_error("Failed to open camera");

    cam >> frame;

    if (frame.empty())
        throw std::runtime_error("Received empty frame");

    canvas = cv::Mat(frame.size(), CV_8UC3);
    canvas = eraserColor;
}

DrawingCam::DrawingCam(int id) //: communicator(communicator1)
{
    cam_id = id;
    initCamera();
}

void DrawingCam::start()
{
    for (char user_input = cv::waitKey(10); user_input != 27; user_input = cv::waitKey(10))
    {
        cam >> frame;
        Mat closing, debug = cv::Mat(frame.size(), CV_8UC3), displayCanvas;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        inRange(hsv, lower, upper, gloveMask);

        Mat element = getStructuringElement(MARKER_CROSS, Size(15, 15));
        morphologyEx(gloveMask, gloveMask, MORPH_CLOSE, element);


        displayCanvas = canvas.clone();
        fingerPoints = FingersDetector::countFingers(gloveMask, vector<Mat *>{&displayCanvas, &frame});

        draw();
        overlayImage(&frame, &canvas);

        std::string sizeAndColor = "Size: " + std::to_string(brushSize);
        putText(displayCanvas, sizeAndColor, Point(0, 50), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255, 255));

        cv::imshow(WINDOW_NAME, frame);
        //imshow("mask", gloveMask);
        imshow("canvas", displayCanvas);
        FrameAndValues data(&hsv, &lower, &upper);
        setMouseCallback(WINDOW_NAME, mouseCallBack, &data);

        if (user_input == '=' && brushSize < 25)
            brushSize += 5;
        else if (user_input == '-' && brushSize > 1)
            brushSize -= 5;
        else if (user_input == 'r')
            canvas = eraserColor;
        else if (user_input == 'e')
            brushColor = eraserColor;
        else if(user_input == 'b')
            brushColor = cv::Scalar(250, 10, 10);
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
            //communicator.sendPoint(DrawPoint(currentPointerPos, brushSize));
        }
    }
}

//void DrawingCam::getPeerPoints()
//{
//    while(true)
//    {
//        DrawPoint p = DrawPoint();
//        try
//        {
//            DrawPoint p = communicator.getPoint();
//        }catch(UniSocketException& e)
//        {
//            std::cout << e << std::endl;
//            break;
//        }
//
//        if(p.p != Point(-1, -1) && p.size != -1)
//        {
//            cv::circle(canvas, p.p, p.size, brushColor, FILLED);
//        }
//
//    }
//}

void mouseCallBack(int event, int x, int y, int flags, void *frameAndValues)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        auto *data = (FrameAndValues *) frameAndValues;
        Vec3b px = (*data->frame).at<Vec3b>(y, x);
        int lh = CLAMP(px.val[0] - OFFSET);
        int ls = CLAMP(px.val[1] - OFFSET);
        int lv = CLAMP(px.val[2] - OFFSET);
        int uh = CLAMP(px.val[0] + OFFSET);
        int us = CLAMP(px.val[1] + OFFSET);
        int uv = CLAMP(px.val[2] + OFFSET);
        *data->lower = Scalar(lh, ls, lv);
        *data->upper = Scalar(uh, us, uv);
    }
}
