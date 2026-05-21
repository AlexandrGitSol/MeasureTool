#ifndef SESSION_H
#define SESSION_H

#include <QString>
#include <vector>
#include <opencv2/opencv.hpp>
#include <QPoint>
#include "Line.h"

class Session{
    cv::Mat img;
    cv::Mat orig_img;
    std::vector<Line> lines;
    Line calibration;

public:
    Session();
    Session(QString path_);
    bool loadSession(QString path_);
    bool loadImage(QString path_);
    void saveImage(QString path_, QString name_);
    void saveSession(QString path_, QString name_);
    void addLine(QPointF p1q, QPointF p2q, double rl);
    void addCalibration(QPointF p1q, QPointF p2q, double rl);
    void renderImage();
    cv::Mat getMat();
};


#endif // SESSION_H
