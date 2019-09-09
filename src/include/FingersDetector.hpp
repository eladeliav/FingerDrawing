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

#define CLOSE_POINTS_THRESHOLD 15
#define TOO_FAR_THRESHOLD 150
#define AREA_TOO_BIG 3000

class FingersDetector
{
public:
    static vector<Point> countFingers(const Mat& frame, vector<Mat*> outputFrames = vector<Mat*>());
private:
    static vector<vector<Point>> getContours(const Mat& mask, vector<Vec4i> &hierarchy, int &maxIndex);
    static Mat threshImage(const Mat &frame);
};


#endif //FINGERDRAWING_FINGERSDETECTOR_HPP
