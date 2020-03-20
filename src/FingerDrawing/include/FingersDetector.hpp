//
// Created by elade on 9/4/2019.
//

#ifndef FINGERDRAWING_FINGERSDETECTOR_HPP
#define FINGERDRAWING_FINGERSDETECTOR_HPP
#include <opencv2/opencv.hpp>
#include <vector>
#include "Helpers.hpp"

using std::vector;
using namespace cv;

// Constants for points filtering
#define CLOSE_POINTS_THRESHOLD 20
#define TOO_FAR_THRESHOLD 30
#define AREA_TOO_BIG 3500

class FingersDetector
{
public:
    // Receives a mat and returns of vector of points that are thought to be fingers
    static vector<Point> countFingers(const Mat& frame, vector<Mat*> outputFrames = vector<Mat*>());
private:
    // gets contours in a frame
    static vector<vector<Point>> getContours(const Mat& mask, vector<Vec4i> &hierarchy, int &maxIndex);
    // turns a matrix black and white according to a threshold
    static Mat threshImage(const Mat &frame);
};


#endif //FINGERDRAWING_FINGERSDETECTOR_HPP
