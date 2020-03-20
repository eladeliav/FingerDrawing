//
// Created by elad on 23/08/2019.
//

#include "../include/ForegroundExtractor.hpp"

// constructor
ForegroundExtractor::ForegroundExtractor()
{
    background = nullptr;
}

// calibrates background
void ForegroundExtractor::calibrate(Mat frame)
{
    background = createBackgroundSubtractorMOG2(0, THRESHHOLD);
    calibrated = true;
}

// extracts foreground
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