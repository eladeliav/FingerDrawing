//
// Created by elad on 23/08/2019.
//

#include "../include/ForegroundExtractor.hpp"

ForegroundExtractor::ForegroundExtractor()
{
    background = nullptr;
}

void ForegroundExtractor::calibrate(Mat frame)
{
    background = createBackgroundSubtractorMOG2(0, THRESHHOLD);
    calibrated = true;
}

Mat ForegroundExtractor::extractForeground(Mat frame)
{
    Mat mask, foreground;
    if(!background)
        return Mat();
    background->apply(frame, mask, 0);

    //TODO: do some eroding/dilating as needed

    bitwise_and(frame, frame, foreground, mask);
    return foreground;
}