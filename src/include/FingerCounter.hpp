//
// Created by elad on 23/08/2019.
//

#ifndef FINGERDRAWING_FINGERCOUNTER_HPP
#define FINGERDRAWING_FINGERCOUNTER_HPP


#include <opencv2/opencv.hpp>
#include <vector>

#define LIMIT_ANGLE_SUP 60
#define LIMIT_ANGLE_INF 5
#define BOUNDING_RECT_FINGER_SIZE_SCALING 0.3
#define BOUNDING_RECT_NEIGHBOR_DISTANCE_SCALING 0.05
#define FINGERS_TOO_CLOSE_THRESH BOUNDING_RECT_NEIGHBOR_DISTANCE_SCALING * 1.5

using namespace cv;
using std::vector;

class FingerCounter
{
public:
    FingerCounter();

    vector<Point> findFingers(Mat mask, Mat frame);

private:
    Scalar color_blue;
    Scalar color_green;
    Scalar color_red;
    Scalar color_black;
    Scalar color_white;
    Scalar color_yellow;
    Scalar color_purple;

    static double findPointsDistance(Point a, Point b);

    static vector<Point> compactOnNeighborhoodMedian(vector<Point> points, double max_neighbor_distance);

    double findAngle(Point a, Point b, Point c);

    bool isFinger(Point a, Point b, Point c, double limit_angle_inf, double limit_angle_sup, Point palm_center,
                  double min_distance_from_palm);

    vector<Point> findClosestOnX(vector<Point> points, Point pivot);

    double findPointsDistanceOnX(Point a, Point b);

    void drawVectorPoints(Mat image, vector<Point> points, Scalar color, bool with_numbers);

    vector<vector<Point>> getContours(Mat mask, vector<Vec4i> &heirarchy, int &maxIndex);
};


#endif //FINGERDRAWING_FINGERCOUNTER_HPP
