#ifndef LINE_H
#define LINE_H

#include <opencv2/opencv.hpp>

struct Line{
    cv::Point p1;
    cv::Point p2;
    double realLength = 0;
};

#endif // LINE_H
