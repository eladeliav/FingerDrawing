//
// Created by elad on 23/08/2019.
//

#include "../include/FingerCounter.hpp"

FingerCounter::FingerCounter()
{
    color_blue = Scalar(255, 0, 0);
    color_green = Scalar(0, 255, 0);
    color_red = Scalar(0, 0, 255);
    color_black = Scalar(0, 0, 0);
    color_white = Scalar(255, 255, 255);
    color_yellow = Scalar(0, 255, 255);
    color_purple = Scalar(255, 0, 255);
}

double FingerCounter::findPointsDistance(Point a, Point b)
{
    Point difference = a - b;
    return sqrt(difference.ddot(difference));
}

vector<Point> FingerCounter::compactOnNeighborhoodMedian(vector<Point> points, double max_neighbor_distance)
{
    vector<Point> median_points;

    if (points.size() == 0)
        return median_points;

    if (max_neighbor_distance <= 0)
        return median_points;

    // we start with the first point
    Point reference = points[0];
    Point median = points[0];

    for (int i = 1; i < points.size(); i++)
    {
        if (findPointsDistance(reference, points[i]) > max_neighbor_distance)
        {

            // the point is not in range, we save the median
            median_points.push_back(median);

            // we swap the reference
            reference = points[i];
            median = points[i];
        } else
            median = (points[i] + median) / 2;
    }

    // last median
    median_points.push_back(median);

    return median_points;
}

double FingerCounter::findAngle(Point a, Point b, Point c)
{
    double ab = findPointsDistance(a, b);
    double bc = findPointsDistance(b, c);
    double ac = findPointsDistance(a, c);
    return acos((ab * ab + bc * bc - ac * ac) / (2 * ab * bc)) * 180 / CV_PI;
}

bool
FingerCounter::isFinger(Point a, Point b, Point c, double limit_angle_inf, double limit_angle_sup, Point palm_center,
                        double min_distance_from_palm)
{
    double angle = findAngle(a, b, c);
    if (angle > limit_angle_sup || angle < limit_angle_inf)
        return false;

    // the finger point sohould not be under the two far points
    int delta_y_1 = b.y - a.y;
    int delta_y_2 = b.y - c.y;
    if (delta_y_1 > 0 && delta_y_2 > 0)
        return false;

    // the two far points should not be both under the center of the hand
    int delta_y_3 = palm_center.y - a.y;
    int delta_y_4 = palm_center.y - c.y;
    if (delta_y_3 < 0 && delta_y_4 < 0)
        return false;

    double distance_from_palm = findPointsDistance(b, palm_center);
    if (distance_from_palm < min_distance_from_palm)
        return false;

    // this should be the case when no fingers are up
    double distance_from_palm_far_1 = findPointsDistance(a, palm_center);
    double distance_from_palm_far_2 = findPointsDistance(c, palm_center);
    return !(distance_from_palm_far_1 < min_distance_from_palm / 4 ||
             distance_from_palm_far_2 < min_distance_from_palm / 4);

}

vector<Point> FingerCounter::findClosestOnX(vector<Point> points, Point pivot)
{
    vector<Point> to_return(2);

    if (points.size() == 0)
        return to_return;

    double distance_x_1 = DBL_MAX;
    double distance_1 = DBL_MAX;
    double distance_x_2 = DBL_MAX;
    double distance_2 = DBL_MAX;
    int index_found = 0;

    for (int i = 0; i < points.size(); i++)
    {
        double distance_x = findPointsDistanceOnX(pivot, points[i]);
        double distance = findPointsDistance(pivot, points[i]);

        if (distance_x < distance_x_1 && distance_x != 0 && distance <= distance_1)
        {
            distance_x_1 = distance_x;
            distance_1 = distance;
            index_found = i;
        }
    }

    to_return[0] = points[index_found];

    for (int i = 0; i < points.size(); i++)
    {
        double distance_x = findPointsDistanceOnX(pivot, points[i]);
        double distance = findPointsDistance(pivot, points[i]);

        if (distance_x < distance_x_2 && distance_x != 0 && distance <= distance_2 && distance_x != distance_x_1)
        {
            distance_x_2 = distance_x;
            distance_2 = distance;
            index_found = i;
        }
    }

    to_return[1] = points[index_found];

    return to_return;
}

double FingerCounter::findPointsDistanceOnX(Point a, Point b)
{
    double to_return = 0.0;

    if (a.x > b.x)
        to_return = a.x - b.x;
    else
        to_return = b.x - a.x;

    return to_return;
}

void FingerCounter::drawVectorPoints(Mat image, vector<Point> points, Scalar color, bool with_numbers)
{
    for (int i = 0; i < points.size(); i++)
    {
        circle(image, points[i], 5, color, 2, 8);
        if (with_numbers)
            putText(image, std::to_string(i), points[i], FONT_HERSHEY_PLAIN, 3, color);
    }
}

vector<Point> FingerCounter::findFingers(Mat mask, Mat frame)
{
    Mat contours_image(320, 240, CV_8UC3, Scalar(0, 0, 0));
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    int maxIndex = -1;
    contours = getContours(mask, hierarchy, maxIndex);

    if (maxIndex < 0)
        return vector<Point>();

    vector<Point> hull_points;
    vector<int> hull_ints;

    convexHull(Mat(contours[maxIndex]), hull_points, true);
    convexHull(Mat(contours[maxIndex]), hull_ints, false);

    vector<Vec4i> defects;
    if (hull_ints.size() > 3)
        convexityDefects(Mat(contours[maxIndex]), hull_ints, defects);
    else
        return vector<Point>();

    Rect bounding_rectangle = boundingRect(Mat(hull_points));

    Point center_bounding_rect(
            (bounding_rectangle.tl().x + bounding_rectangle.br().x) / 2,
            (bounding_rectangle.tl().y + bounding_rectangle.br().y) / 2
    );

    vector<Point> start_points;
    vector<Point> far_points;

    for (int i = 0; i < defects.size(); i++)
    {
        start_points.push_back(contours[maxIndex][defects[i].val[0]]);

        if (findPointsDistance(contours[maxIndex][defects[i].val[2]], center_bounding_rect) <
            bounding_rectangle.height * BOUNDING_RECT_FINGER_SIZE_SCALING)
            far_points.push_back(contours[maxIndex][defects[i].val[2]]);
    }

    vector<Point> filtered_start_points = compactOnNeighborhoodMedian(start_points, bounding_rectangle.height *
                                                                                    BOUNDING_RECT_NEIGHBOR_DISTANCE_SCALING);
    vector<Point> filtered_far_points = compactOnNeighborhoodMedian(far_points, bounding_rectangle.height *
                                                                                BOUNDING_RECT_NEIGHBOR_DISTANCE_SCALING);

    vector<Point> filtered_finger_points;
    if (filtered_far_points.size() > 1)
    {
        vector<Point> finger_points;

        for (int i = 0; i < filtered_start_points.size(); i++)
        {
            vector<Point> closest_points = findClosestOnX(filtered_far_points, filtered_start_points[i]);

            if (isFinger(closest_points[0], filtered_start_points[i], closest_points[1], LIMIT_ANGLE_INF,
                         LIMIT_ANGLE_SUP, center_bounding_rect,
                         bounding_rectangle.height * BOUNDING_RECT_FINGER_SIZE_SCALING))
                finger_points.push_back(filtered_start_points[i]);
        }

        if (!finger_points.empty())
        {
            while (finger_points.size() > 5)
                finger_points.pop_back();

            for (int i = 0; i < finger_points.size() - 1; i++)
                if (findPointsDistanceOnX(finger_points[i], finger_points[i + 1]) >
                    bounding_rectangle.height * FINGERS_TOO_CLOSE_THRESH)
                    filtered_finger_points.push_back(finger_points[i]);

            if (finger_points.size() > 2)
            {
                if (findPointsDistanceOnX(finger_points[0], finger_points[finger_points.size() - 1]) >
                    bounding_rectangle.height * FINGERS_TOO_CLOSE_THRESH)
                    filtered_finger_points.push_back(finger_points[finger_points.size() - 1]);
            } else
                filtered_finger_points.push_back(finger_points[finger_points.size() - 1]);
        }
    }
    // we draw what found on the returned image
    drawContours(contours_image, contours, maxIndex, color_green, 2, 8, hierarchy);
    polylines(contours_image, hull_points, true, color_blue);
    rectangle(contours_image, bounding_rectangle.tl(), bounding_rectangle.br(), color_red, 2, 8, 0);
    circle(contours_image, center_bounding_rect, 5, color_purple, 2, 8);
    drawVectorPoints(contours_image, filtered_start_points, color_blue, true);
    drawVectorPoints(contours_image, filtered_far_points, color_red, true);
    drawVectorPoints(contours_image, filtered_finger_points, color_yellow, false);
    putText(contours_image, std::to_string(filtered_finger_points.size()), center_bounding_rect, FONT_HERSHEY_PLAIN, 3,
            color_purple);

    // and on the starting frame
    drawContours(frame, contours, maxIndex, color_green, 2, 8, hierarchy);
    circle(frame, center_bounding_rect, 5, color_purple, 2, 8);
    drawVectorPoints(frame, filtered_finger_points, color_yellow, false);
    putText(frame, std::to_string(filtered_finger_points.size()), center_bounding_rect, FONT_HERSHEY_PLAIN, 3,
            color_purple);

    return filtered_finger_points;
}

vector<vector<Point>> FingerCounter::getContours(Mat mask, vector<Vec4i> &heirarchy, int &maxIndex)
{
    if (mask.empty() || mask.channels() != 1)
    {
        maxIndex = -1;
        return vector<vector<Point>>();
    }

    vector<vector<Point>> contours;
    findContours(mask, contours, heirarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

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
