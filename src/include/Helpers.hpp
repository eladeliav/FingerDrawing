//
// Created by elade on 9/7/2019.
//

#ifndef FINGERDRAWING_HELPERS_HPP
#define FINGERDRAWING_HELPERS_HPP

#include <opencv2/core.hpp>
#include <vector>

using std::vector;
using namespace cv;

namespace Helpers
{
    bool closePointExists(const vector<Point> &points, const Point &point, int thresh);
    double pointsDistance(const Point& a, const Point& b);
    bool closePointExists(const Mat& frame, const Point& point, int thresh);
    bool pointTooFarFromOthers(const vector<Point>& points, const Point& point, int thresh);
};


#endif //FINGERDRAWING_HELPERS_HPP
