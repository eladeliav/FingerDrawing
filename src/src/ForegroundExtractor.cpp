//
// Created by elad on 23/08/2019.
//

#include "../include/ForegroundExtractor.hpp"

ForegroundExtractor::ForegroundExtractor()
{
    background = cv::Mat();
    calibrated = false;
}

void ForegroundExtractor::calibrate(cv::Mat frame)
{
    cv::cvtColor(frame, background, cv::COLOR_BGR2GRAY);
    calibrated = true;
}

cv::Mat ForegroundExtractor::extractForeground(cv::Mat frame)
{
    cv::Mat foregroundMask = extractForegroundMask(frame);

    cv::Mat foreground;
    frame.copyTo(foreground, foregroundMask);

    return foreground;
}

cv::Mat ForegroundExtractor::extractForegroundMask(cv::Mat frame)
{
    cv::Mat foregroundMask;

    if (!calibrated)
    {
        foregroundMask = cv::Mat::zeros(frame.size(), CV_8UC1);
        return foregroundMask;
    }

    cvtColor(frame, foregroundMask, cv::COLOR_BGR2GRAY);

    removeBackground(foregroundMask);

    return foregroundMask;
}

void ForegroundExtractor::removeBackground(cv::Mat frame)
{
    for (int i = 0; i < frame.rows; i++)
    {
        for (int j = 0; j < frame.cols; j++)
        {
            uchar framePx = frame.at<uchar>(i, j);
            uchar bgPx = background.at<uchar>(i, j);

            if (framePx >= bgPx - THRESHHOLD && framePx <= bgPx + THRESHHOLD)
                frame.at<uchar>(i, j) = 0;
            else
                frame.at<uchar>(i, j) = 255;
        }
    }
}
