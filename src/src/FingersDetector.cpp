//
// Created by elade on 9/4/2019.
//

#include "../include/FingersDetector.hpp"
#include <iostream>
#include <math.h>

vector<Point> FingersDetector::countFingers(const Mat &frame)
{
    if(frame.empty())
        return vector<Point>();

    Mat debugImage(frame.rows, frame.cols, CV_8UC3, Scalar(0, 0, 0));
    vector<vector<Point>> contours;
    vector<Point> maxContour;
    vector<Vec4i> hierarchy;
    int maxIndex = -1;
    contours = getContours(frame, hierarchy, maxIndex);

    if (maxIndex < 0 || contourArea(contours[maxIndex], false) <= 1000)
        return vector<Point>();

    maxContour = contours[maxIndex];
    drawContours(debugImage, contours, maxIndex, Scalar(0, 255, 0));
    vector<Point> hull_points;
    vector<int> hull_ints;

    convexHull(maxContour, hull_points, true);
    convexHull(maxContour, hull_ints, false);
    drawContours(debugImage, vector<vector<Point>>(1, hull_points), 0, Scalar(0, 0, 255));
    Rect boundingRectangle = boundingRect(hull_points);
    Point handCenter(
            (boundingRectangle.tl().x + boundingRectangle.br().x) / 2,
            (boundingRectangle.tl().y + boundingRectangle.br().y) / 2
            );
    rectangle(debugImage, boundingRectangle, Scalar(255, 0, 0));
    circle(debugImage, handCenter, 10, Scalar(255, 255, 0));

    vector<Vec4i> defects;
    if (hull_ints.size() <= 3)
        return vector<Point>();

    convexityDefects(Mat(maxContour), hull_ints, defects);
    if (defects.empty())
        return vector<Point>();

    int fingerNum = 0;
    vector<Point> fingerPoints;
    for (auto & defect : defects)
    {
        if(fingerNum == 4)
            break;
        Point start = maxContour[defect.val[0]];
        Point end = maxContour[defect.val[1]];
        Point far = maxContour[defect.val[2]];

        if(pointsDistance(start, end) <= 10 || pointsDistance(end, far) <=  10 || pointsDistance(start, far) <=  10)
            continue;

        int a = sqrt(std::pow((end.x - start.x), 2) + std::pow(end.y - start.y, 2));
        int b = sqrt(std::pow((far.x - start.x), 2) + std::pow(far.y - start.y, 2));
        int c = sqrt(std::pow((end.x - far.x), 2) + std::pow(end.y - far.y, 2));
        float angle = acos((std::pow(b, 2) + std::pow(c, 2) - std::pow(a, 2)) / (2 * b * c));

        if(angle <= M_PI / 2 && far.y < handCenter.y)
        {
            fingerNum++;
            circle(debugImage, start, 8, Scalar(0, 0, 255));
            circle(debugImage, end, 8, Scalar(0, 255, 0));
            circle(debugImage, far, 8, Scalar(255, 0, 0));
        }
    }

    if(fingerNum == 0)
    {
        for (auto & defect : defects)
        {
            if(fingerNum == 4)
                break;
            Point start = maxContour[defect.val[0]];
            Point end = maxContour[defect.val[1]];
            Point far = maxContour[defect.val[2]];

            if(pointsDistance(start, end) <= 10 || pointsDistance(end, far) <=  10 || pointsDistance(start, far) <=  10)
                continue;

            int a = sqrt(std::pow((end.x - start.x), 2) + std::pow(end.y - start.y, 2));
            int b = sqrt(std::pow((far.x - start.x), 2) + std::pow(far.y - start.y, 2));
            int c = sqrt(std::pow((end.x - far.x), 2) + std::pow(end.y - far.y, 2));
            float angle = acos((std::pow(b, 2) + std::pow(c, 2) - std::pow(a, 2)) / (2 * b * c));

            if(far.y < handCenter.y && angle < M_PI)
            {
                fingerNum++;
                circle(debugImage, start, 8, Scalar(0, 0, 255));
                circle(debugImage, end, 8, Scalar(0, 255, 0));
                circle(debugImage, far, 8, Scalar(255, 0, 0));
            }
        }
    }

    std::cout << fingerNum + 1 << ", hand size: " << contourArea(maxContour) << std::endl;
    imshow("debug", debugImage);
    return vector<Point>();
}

vector<vector<Point>> FingersDetector::getContours(const Mat &mask, vector<Vec4i> &hierarchy, int &maxIndex)
{
    Mat thresh = threshImage(mask);
    if (thresh.empty() || thresh.channels() != 1)
    {
        maxIndex = -1;
        return vector<vector<Point>>();
    }

    vector<vector<Point>> contours;
    findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    maxIndex = -1;
    double maxArea = 0;

    for (int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i], false);
        if (area > maxArea)
        {
            maxArea = area;
            maxIndex = i;
        }
    }

    return contours;
}

double FingersDetector::pointsDistance(const Point& a, const Point& b)
{
    Point difference = a - b;
    return sqrt(difference.ddot(difference));
}

Mat FingersDetector::threshImage(const Mat& frame)
{
    Mat gray, blur, thresh;
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, Size(41, 41), 0);
    threshold(blur, thresh, 80, 255, THRESH_BINARY);

    imshow("thresh", thresh);
    return thresh;
}
