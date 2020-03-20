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
    // Checks if a point is within THRESH distance to a point in a given vector
    bool closePointExists(const vector<Point> &points, const Point &point, int thresh);
    // returns distance between two points
    double pointsDistance(const Point& a, const Point& b);
    // Checks if a point is within THRESH distance to a point in a given matrix
    bool closePointExists(const Mat& frame, const Point& point, int thresh);
};


#endif //FINGERDRAWING_HELPERS_HPP
