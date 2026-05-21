#ifndef LINE_H
#define LINE_H

#include <opencv2/opencv.hpp>

struct Line{
    cv::Point p1;
    cv::Point p2;
    double realLength;
};

#endif // LINE_H
