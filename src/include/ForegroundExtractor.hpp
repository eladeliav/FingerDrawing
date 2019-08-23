//
// Created by elad on 23/08/2019.
//

#ifndef FINGERDRAWING_FOREGROUNDEXTRACTOR_HPP
#define FINGERDRAWING_FOREGROUNDEXTRACTOR_HPP

#include <opencv2/opencv.hpp>

#define THRESHHOLD 10

class ForegroundExtractor
{
public:
    ForegroundExtractor();

    void calibrate(cv::Mat frame);

    cv::Mat extractForeground(cv::Mat frame);

private:
    cv::Mat background;
    bool calibrated = false;

private:

    cv::Mat extractForegroundMask(cv::Mat frame);

    void removeBackground(cv::Mat frame);
};


#endif //FINGERDRAWING_FOREGROUNDEXTRACTOR_HPP
