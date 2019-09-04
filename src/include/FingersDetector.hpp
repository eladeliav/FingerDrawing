//
// Created by elade on 9/4/2019.
//

#ifndef FINGERDRAWING_FINGERSDETECTOR_HPP
#define FINGERDRAWING_FINGERSDETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>

using std::vector;
using namespace cv;

class FingersDetector
{
public:
    static vector<Point> countFingers(const Mat& frame);
private:
    static vector<vector<Point>> getContours(const Mat& mask, vector<Vec4i> &hierarchy, int &maxIndex);
    static double pointsDistance(const Point& a, const Point& b);
    static Mat threshImage(const Mat& frame);
};


#endif //FINGERDRAWING_FINGERSDETECTOR_HPP
