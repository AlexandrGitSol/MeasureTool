#ifndef SESSION_H
#define SESSION_H

#include <QString>
#include <vector>
#include <opencv2/opencv.hpp>
#include <QPoint>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <sstream>
#include "../libs/json.hpp"
#include "Line.h"

class Session{
    cv::Mat img;
    cv::Mat orig_img;
    QString path_to_img;
    std::vector<Line> lines;
    Line calibration;
    int selectedline = -1;

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
    void delLine(int ind);
    void selectedLine(int ind);
    std::vector<QString> getLines();
    cv::Mat getMat();
};


#endif // SESSION_H
