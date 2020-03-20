//
// Created by elad on 23/08/2019.
//

#ifndef FINGERDRAWING_FOREGROUNDEXTRACTOR_HPP
#define FINGERDRAWING_FOREGROUNDEXTRACTOR_HPP
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>

using namespace cv;

// Threshold for background vs foreground
#define THRESHHOLD 50

class ForegroundExtractor
{
public:
    // default constructor
    ForegroundExtractor();

    // calibrates background
    void calibrate(Mat frame);

    // extracts the foreground
    Mat extractForeground(Mat frame);

private:
    // variable for the background
    Ptr<BackgroundSubtractorMOG2> background;
    bool calibrated = false; // if calibrated already
};


#endif //FINGERDRAWING_FOREGROUNDEXTRACTOR_HPP