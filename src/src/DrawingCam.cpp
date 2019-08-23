//
// Created by elad on 23/08/2019.
//
#include "../include/DrawingCam.hpp"

DrawingCam::DrawingCam(int id)
{
    cam_id = id;
    brushSize = 5;

    currentPointerPos = lastPointerPos = cv::Point(0, 0);

    eraserColor = cv::Scalar(0, 0, 0);
    brushColor = cv::Scalar(250, 10, 10);

    cam = cv::VideoCapture(cam_id);

    foregroundExtractor = ForegroundExtractor();
    skinDetector = SkinDetector();
    fingerCounter = FingerCounter();

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

        if (!skinDetector.alreadySampled())
            skinDetector.drawSampler(frameClone);
        foreground = foregroundExtractor.extractForeground(frame);

        FacesRemover::removeFaces(frame, foreground);

        skinMask = skinDetector.genMask(foreground);
        fingerPoints = fingerCounter.findFingers(skinMask, frameClone);

        draw();
        frameClone |= canvas;

        cv::imshow(WINDOW_NAME, frameClone);
        cv::imshow("Canvas", canvas);

        if (user_input == '+' && brushSize < 25)
        {
            brushSize++;
        } else if (user_input == '-' && brushSize > 1)
        {
            brushSize--;
        } else if (user_input == 'b')
            foregroundExtractor.calibrate(frame);
        else if (user_input == 's')
            skinDetector.sample(frame);
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
        lastPointerPos = currentPointerPos;
        currentPointerPos = fingerPoints.at(0);

        //cv::line(canvas, lastPointerPos, currentPointerPos, brushColor, brushSize);
        cv::circle(canvas, currentPointerPos, brushSize, brushColor, brushSize);
    }
}
