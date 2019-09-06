//
// Created by elad on 23/08/2019.
//

#include "../include/ForegroundExtractor.hpp"

ForegroundExtractor::ForegroundExtractor()
{

}

void ForegroundExtractor::calibrate(Mat frame)
{
    background = createBackgroundSubtractorMOG2(0, THRESHHOLD);
    calibrated = true;
}

Mat ForegroundExtractor::extractForeground(Mat frame)
{
    Mat mask, foreground;
    background->apply(frame, mask, 0);

    //TODO: do some eroding/dilating as needed

    bitwise_and(frame, frame, foreground, mask);
    return foreground;
}
