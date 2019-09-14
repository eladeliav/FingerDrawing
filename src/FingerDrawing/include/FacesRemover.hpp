//
// Created by elad on 23/08/2019.
//

#ifndef FINGERDRAWING_FACESREMOVER_HPP
#define FINGERDRAWING_FACESREMOVER_HPP


#include <opencv2/opencv.hpp>
#include <vector>

using std::vector;
using namespace cv;

#define FACE_CLASSIFIER_FILE_PATH "haarcascade_frontalface_alt.xml"

namespace FacesRemover
{
    void removeFaces(Mat &frame, Mat &output);
};


#endif //FINGERDRAWING_FACESREMOVER_HPP