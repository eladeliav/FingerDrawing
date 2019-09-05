//
// Created by elad on 23/08/2019.
//
#include "../include/DrawingCam.hpp"

void overlayImage(Mat* src, Mat* overlay, const Point& location)
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

            double opacity = ((double)overlay->data[fY * overlay->step + fX * overlay->channels() + 3]) / 255;

            for (int c = 0; opacity > 0 && c < src->channels(); ++c)
            {
                unsigned char overlayPx = overlay->data[fY * overlay->step + fX * overlay->channels() + c];
                unsigned char srcPx = src->data[y * src->step + x * src->channels() + c];
                src->data[y * src->step + src->channels() * x + c] = srcPx * (1. - opacity) + overlayPx * opacity;
            }
        }
    }
}

DrawingCam::DrawingCam(int id)
{
    cam_id = id;
    brushSize = 5;

    currentPointerPos = cv::Point(0, 0);

    eraserColor = cv::Scalar(0, 0, 0);
    brushColor = cv::Scalar(250, 10, 10);

    cam = cv::VideoCapture(cam_id);

    foregroundExtractor = ForegroundExtractor();
    skinDetector = SkinDetector();

    if (!cam.isOpened())
        throw std::runtime_error("Failed to open camera");

    cam >> frame;
    frameClone = frame.clone();

    if (frame.empty())
        throw std::runtime_error("Received empty frame");

    canvas = cv::Mat(frame.size(), CV_8UC3);

    canvas = eraserColor;

    foregroundExtractor.calibrate(frame);

}

void DrawingCam::start()
{
    for (char user_input = cv::waitKey(10); user_input != 27; user_input = cv::waitKey(10))
    {
        cam >> frame;


        frameClone = frame.clone();


        foreground = foregroundExtractor.extractForeground(frame);
        FacesRemover::removeFaces(frame, foreground);
        FingersDetector::countFingers(foreground);

        draw();
        overlayImage(&frameClone, &canvas, Point(0, 0));

        cv::imshow(WINDOW_NAME, frameClone);
        cv::imshow("Foreground", foreground);

        if (user_input == '+' && brushSize < 25)
        {
            brushSize++;
        } else if (user_input == '-' && brushSize > 1)
        {
            brushSize--;
        } else if (user_input == 'b')
            foregroundExtractor.calibrate(frame);
        else if(user_input == 'r')
            canvas = eraserColor;
    }
    cv::destroyAllWindows();
    cam.release();
}

void DrawingCam::draw()
{
    if (fingerPoints.size() == 1)
    {
        currentPointerPos = fingerPoints.at(0);

        //cv::line(canvas, lastPointerPos, currentPointerPos, brushColor, brushSize);
        cv::circle(canvas, currentPointerPos, brushSize, brushColor, brushSize);
    }
}
