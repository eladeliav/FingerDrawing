//
// Created by elad on 23/08/2019.
//

#ifndef FINGERDRAWING_SKINDETECTOR_HPP
#define FINGERDRAWING_SKINDETECTOR_HPP

#include <opencv2/opencv.hpp>

using namespace cv;

#define OFFSET_LOW_THRESH 60
#define OFFSET_HIGH_THRESH 60

class SkinDetector
{
private:

    int hLow = 0;
    int hHigh = 0;

    int sLow = 0;
    int sHigh = 0;

    int vLow = 0;
    int vHigh = 0;
    Rect sampler1, sampler2;

public:
    bool sampled = false;
    explicit SkinDetector();

    void drawSampler(Mat frame);

    void sample(Mat frame);

    Mat genMask(Mat frame);

    void calcThresholds(Mat sample1, Mat sample2);

    void resetThresholds();
};

#endif //FINGERDRAWING_SKINDETECTOR_HPP