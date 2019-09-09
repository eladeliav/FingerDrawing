//
// Created by elade on 9/7/2019.
//

#include "../include/Helpers.hpp"

bool Helpers::closePointExists(const vector<Point> &points, const Point &point, int thresh)
{
    for (auto const &p : points)
        if (pointsDistance(p, point) <= thresh)
            return true;
    return false;
}

double Helpers::pointsDistance(const Point &a, const Point &b)
{
    Point difference = a - b;
    return sqrt(difference.ddot(difference));
}

bool Helpers::closePointExists(const Mat &frame, const Point& point, int thresh)
{
    for(int row = 0; row < frame.rows; row++)
    {
        for(int col = 0; col < frame.cols;col++)
        {
            Point px = frame.at<Point>(row, col);
            if(pointsDistance(point, px) <= thresh)
            {
                return true;
            }
        }
    }
    return false;
}

bool Helpers::pointTooFarFromOthers(const vector<Point>& points, const Point& point, int thresh)
{
    for(auto const& p : points)
    {
        if(pointsDistance(p, point) > thresh)
            return true;
    }
    return false;
}
