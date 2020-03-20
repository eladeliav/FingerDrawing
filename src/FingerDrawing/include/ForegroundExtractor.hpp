//
// Created by elad on 23/08/2019.
//

#ifndef FINGERDRAWING_FOREGROUNDEXTRACTOR_HPP
#define FINGERDRAWING_FOREGROUNDEXTRACTOR_HPP
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>

using namespace cv;

// Threshold for
#define THRESHHOLD 50

class ForegroundExtractor
{
public:
    ForegroundExtractor();

    void calibrate(Mat frame);

    Mat extractForeground(Mat frame);

private:
    Ptr<BackgroundSubtractorMOG2> background;
    bool calibrated = false;
};


#endif //FINGERDRAWING_FOREGROUNDEXTRACTOR_HPP