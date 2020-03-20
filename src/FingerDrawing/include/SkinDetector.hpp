//
// Created by elad on 23/08/2019.
//

#ifndef FINGERDRAWING_SKINDETECTOR_HPP
#define FINGERDRAWING_SKINDETECTOR_HPP

#include <opencv2/opencv.hpp>

using namespace cv;

// Low and high thresholds for skin color detection
#define OFFSET_LOW_THRESH 60
#define OFFSET_HIGH_THRESH 60

class SkinDetector
{
private:

    // hue low and high
    int hLow = 0;
    int hHigh = 0;

    // saturation low and high
    int sLow = 0;
    int sHigh = 0;

    // value low and high
    int vLow = 0;
    int vHigh = 0;
    // sampling rectangles (points on screen we will sample skin color)
    Rect sampler1, sampler2;

public:
    bool sampled = false; // whether or not already sampled skin color
    explicit SkinDetector(); // default constructor

    // draws the two sampling rectangles
    void drawSampler(Mat frame);

    // samples skin color from the samplers
    void sample(Mat frame);

    // generates mask from the sampled skin color
    Mat genMask(Mat frame);

    // calculates thresholds between two samples
    void calcThresholds(Mat sample1, Mat sample2);

    // resets thresholds of skin color
    void resetThresholds();
};

#endif //FINGERDRAWING_SKINDETECTOR_HPP