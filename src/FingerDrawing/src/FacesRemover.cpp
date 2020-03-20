//
// Created by elad on 23/08/2019.
//

#include "../include/FacesRemover.hpp"

void FacesRemover::removeFaces(Mat &frame, Mat &output)
{
    // loads cascade classifier (Big file that helps detect faces)
    CascadeClassifier cascadeClassifier;

    if (!cascadeClassifier.load(FACE_CLASSIFIER_FILE_PATH))
        throw std::runtime_error("Couldn't load face classifier");
    vector<Rect> faces;
    Mat frameGray;

    // change to grayscale for more accurate results
    cvtColor(frame, frameGray, COLOR_BGR2GRAY);
    equalizeHist(frameGray, frameGray);

    // detect faces
    cascadeClassifier.detectMultiScale(frameGray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(120, 120));
    // if found more than 1 face, warn user to remove face from roi
    if(faces.size() > 0)
    {
        putText(output, "Faces Detected in ROI!", Point(output.cols / 2 - 50, output.rows / 2), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255, 255));
    }

}