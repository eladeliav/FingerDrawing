//
// Created by elad on 23/08/2019.
//

#include "../include/SkinDetector.hpp"
// empty constructor
SkinDetector::SkinDetector()
{
}

// draws the sampelrs on screen (pink rectangles)
void SkinDetector::drawSampler(Mat frame)
{
    int frameWidth = frame.size().width;
    int frameHeight = frame.size().height;

    int rectSize = 40;
    Scalar rectColor(255, 0, 255);

    sampler1 = Rect(frameWidth / 2, frameHeight / 2, rectSize, rectSize);
    sampler2 = Rect(frameWidth / 2, frameHeight / 2 + 50, rectSize, rectSize);

    rectangle(frame, sampler1, rectColor);
    rectangle(frame, sampler2, rectColor);
}

// samples skin color from samplers
void SkinDetector::sample(Mat frame)
{
    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);

    Mat sample1 = Mat(hsv, sampler1);
    Mat sample2 = Mat(hsv, sampler2);

    calcThresholds(sample1, sample2);

    sampled = true;
}

// generates mask from sampled skin color
Mat SkinDetector::genMask(Mat frame)
{
    Mat hsv, blur, thresh;

    if (!sampled)
    {
        thresh = Mat::zeros(frame.size(), CV_8UC1);
        return thresh;
    }



    // blurs for better results
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    GaussianBlur(hsv, blur, Size(41, 41), 0);
    inRange(blur, Scalar(hLow, sLow, vLow), Scalar(hHigh, sHigh, vHigh), thresh);

    // "opens" up some rough parts of image for better results
    Mat element = getStructuringElement(MARKER_CROSS, Size(15, 15));
    morphologyEx(thresh, thresh, MORPH_CLOSE, element);

//    Mat hsv;
//    cvtColor(frame, hsv, COLOR_BGR2HSV);
//
//    inRange(hsv, Scalar(hLow, sLow, vLow), Scalar(hHigh, sHigh, vHigh), mask);
//    Mat structuringElement = getStructuringElement(MORPH_ELLIPSE, {15, 15});
//    morphologyEx(mask, mask, MORPH_OPEN, structuringElement);
//    dilate(mask, mask, Mat(), Point(-1, -1), 3);

    return thresh;
}

// calculates thresholds from samplers with the low and high offsets
void SkinDetector::calcThresholds(Mat sample1, Mat sample2)
{
    Scalar hsvMeansSample1 = mean(sample1);
    Scalar hsvMeansSample2 = mean(sample2);

    hLow = min(hsvMeansSample1[0], hsvMeansSample2[0]) - OFFSET_LOW_THRESH;
    hHigh = max(hsvMeansSample1[0], hsvMeansSample2[0]) + OFFSET_HIGH_THRESH;

    sLow = min(hsvMeansSample1[1], hsvMeansSample2[1]) - OFFSET_LOW_THRESH;
    sHigh = max(hsvMeansSample1[1], hsvMeansSample2[1]) + OFFSET_HIGH_THRESH;

    // the V channel shouldn't be used. By ignoring it, shadows on the hand wouldn't interfere with segmentation.
    // Unfortunately there's a bug somewhere and not using the V channel causes some problem. This shouldn't be too hard to fix.
    vLow = min(hsvMeansSample1[2], hsvMeansSample2[2]) - OFFSET_LOW_THRESH;
    vHigh = max(hsvMeansSample1[2], hsvMeansSample2[2]) + OFFSET_HIGH_THRESH;
//    vLow = 0;
//    vHigh = 255;
}

// resets thresholds
void SkinDetector::resetThresholds()
{
    sampled = false;
    hLow = 0;
    hHigh = 0;

    sLow = 0;
    sHigh = 0;

    vLow = 0;
    vHigh = 0;
}
