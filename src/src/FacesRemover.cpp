//
// Created by elad on 23/08/2019.
//

#include "../include/FacesRemover.hpp"

void FacesRemover::removeFaces(Mat &frame, Mat &output)
{
    CascadeClassifier cascadeClassifier;

    if (!cascadeClassifier.load(FACE_CLASSIFIER_FILE_PATH))
        throw std::runtime_error("Couldn't load face classifier");
    vector<Rect> faces;
    Mat frameGray;

    cvtColor(frame, frameGray, COLOR_BGR2GRAY);
    equalizeHist(frameGray, frameGray);

    cascadeClassifier.detectMultiScale(frameGray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(120, 120));

    for (size_t i = 0; i < faces.size(); i++)
    {
        rectangle(
                output,
                Point(faces[i].x, faces[i].y),
                Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height),
                Scalar(0, 0, 0),
                -1
        );
    }
}