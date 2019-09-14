//
// Created by elade on 9/4/2019.
//
#define _USE_MATH_DEFINES
#include <cmath>
#include "../include/FingersDetector.hpp"
#include <iostream>


vector<Point> FingersDetector::countFingers(const Mat &frame, vector<Mat *> outputFrames)
{
    if (frame.empty())
        return vector<Point>();
    vector<vector<Point>> contours;
    vector<Point> maxContour;
    vector<Vec4i> hierarchy;
    int maxIndex = -1;
    contours = getContours(frame, hierarchy, maxIndex);

    if (maxIndex < 0 || contourArea(contours[maxIndex], false) <= 1000)
        return vector<Point>();

    maxContour = contours[maxIndex];
    vector<Point> hull_points;
    vector<int> hull_ints;

    convexHull(maxContour, hull_points, true);
    convexHull(maxContour, hull_ints, false);
    Rect boundingRectangle = boundingRect(hull_points);
    Point handCenter(
            (boundingRectangle.tl().x + boundingRectangle.br().x) / 2,
            (boundingRectangle.tl().y + boundingRectangle.br().y) / 2);

    vector<Vec4i> defects;
    if (hull_ints.size() <= 3)
        return vector<Point>();

    convexityDefects(Mat(maxContour), hull_ints, defects);
    if (defects.empty())
        return vector<Point>();
    vector<Point> fingerPoints;
    vector<vector<Point>> allFingerPoints;
    for (auto &defect : defects)
    {
        if (fingerPoints.size() == 5)
            break;
        Point start = maxContour[defect.val[0]];
        Point end = maxContour[defect.val[1]];
        Point far = maxContour[defect.val[2]];

        if (Helpers::pointsDistance(start, end) <= CLOSE_POINTS_THRESHOLD ||
            Helpers::pointsDistance(end, far) <= CLOSE_POINTS_THRESHOLD ||
            Helpers::pointsDistance(start, far) <= CLOSE_POINTS_THRESHOLD)
            continue;

        int a = sqrt(std::pow((end.x - start.x), 2) + std::pow(end.y - start.y, 2));
        int b = sqrt(std::pow((far.x - start.x), 2) + std::pow(far.y - start.y, 2));
        int c = sqrt(std::pow((end.x - far.x), 2) + std::pow(end.y - far.y, 2));
        float angle = acos((std::pow(b, 2) + std::pow(c, 2) - std::pow(a, 2)) / (2 * b * c));
        float decimalAngle = angle * (180 / M_PI);
        float area = b* c * sin(decimalAngle) / 2;

        if(std::abs(area) >= AREA_TOO_BIG)
            continue;

        if(start.y > far.y && end.y > far.y)
            continue;

        if (angle <= M_PI / 2)
        {
            if (!Helpers::closePointExists(fingerPoints, start, CLOSE_POINTS_THRESHOLD))
                fingerPoints.push_back(start);
            if (!Helpers::closePointExists(fingerPoints, end, CLOSE_POINTS_THRESHOLD))
                fingerPoints.push_back(end);
            allFingerPoints.push_back({start, end, far});
        } else if (angle <= M_PI && fingerPoints.empty())
        {
            if(start.y > handCenter.y && end.y > handCenter.y)
                continue;
            if(start.y < end.y)
            {
                fingerPoints.push_back(start);
            }
            else
                fingerPoints.push_back(end);
            allFingerPoints.push_back({start, end, far});
        }
    }

    for (auto &f : outputFrames)
    {
        drawContours(*f, contours, maxIndex, Scalar(0, 255, 0));
        drawContours(*f, vector<vector<Point>>(1, hull_points), 0, Scalar(0, 0, 255));
        rectangle(*f, boundingRectangle, Scalar(255, 0, 0));
        circle(*f, handCenter, 10, Scalar(255, 255, 0));
        for (auto const &p : fingerPoints)
        {
            circle(*f, p, 10, Scalar(255, 255, 0));
        }
        for (auto const &l : allFingerPoints)
        {
            circle(*f, l.at(0), 8, Scalar(0, 0, 255));
            line(*f, l.at(0), l.at(1), Scalar(255, 0, 0));
            circle(*f, l.at(1), 8, Scalar(255, 0, 0));
            line(*f, l.at(1), l.at(2), Scalar(255, 0, 0));
            line(*f, l.at(0), l.at(2), Scalar(255, 0, 0));
            circle(*f, l.at(2), 8, Scalar(0, 255, 0));
        }
        putText(*f, std::to_string(fingerPoints.size()), Point(handCenter.x, handCenter.y + CLOSE_POINTS_THRESHOLD),
                FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255, 255));
    }

    std::cout << fingerPoints.size() << std::endl;
    return fingerPoints;
}

vector<vector<Point>> FingersDetector::getContours(const Mat &mask, vector<Vec4i> &hierarchy, int &maxIndex)
{
    Mat thresh = mask; //threshImage(mask);
    if (thresh.empty() || thresh.channels() != 1)
    {
        maxIndex = -1;
        return vector<vector<Point>>();
    }

    vector<vector<Point>> contours;
    findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    maxIndex = -1;
    double maxArea = 0;

    for (size_t i = 0; i < contours.size(); i++)
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

Mat FingersDetector::threshImage(const Mat &frame)
{
    Mat gray, blur, thresh;
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, Size(41, 41), 0);
    threshold(blur, thresh, 80, 255, THRESH_BINARY);

    Mat element = getStructuringElement(MARKER_CROSS, Size(15, 15));
    morphologyEx(thresh, thresh, MORPH_CLOSE, element);

    imshow("thresh", thresh);
    return thresh;
}