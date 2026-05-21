#include "Session.h"

Session::Session(){

    img = cv::imread("/home/alexandr/Images/2023_Obsidian_logo.svg.png");
    orig_img = img.clone();
}

Session::Session(QString path_){

    img = cv::imread(path_.toStdString());
    orig_img = img.clone();

}

bool Session::loadImage(QString path_){
    img = cv::imread(path_.toStdString());
    orig_img = img.clone();
    if (img.empty()) return false;
    return true;
}

void Session::saveImage(QString path_, QString name_){
    cv::imwrite(path_.toStdString() + "/" + name_.toStdString() + ".jpg", img);
}

bool Session::loadSession(QString path_){

}

void Session::saveSession(QString path_, QString name_){

}

void Session::addLine(QPointF p1q, QPointF p2q, double rl){
    cv::Point p1(p1q.x(), p1q.y());
    cv::Point p2(p2q.x(), p2q.y());
    Line l;
    l.p1 = p1;
    l.p2 = p2;
    l.realLength = rl;
    lines.push_back(l);
}

void Session::addCalibration(QPointF p1q, QPointF p2q, double rl){
    cv::Point p1(p1q.x(), p1q.y());
    cv::Point p2(p2q.x(), p2q.y());
    Line l;
    l.p1 = p1;
    l.p2 = p2;
    l.realLength = rl;
    calibration = l;
}

void Session::renderImage(){
    img = orig_img.clone();

    line(img, calibration.p1, calibration.p2, cv::Scalar(200,50,0), 2);
    cv::putText(img, std::__cxx11::to_string(calibration.realLength), (calibration.p2 + calibration.p1)/2, cv::FONT_HERSHEY_DUPLEX, 0.4, cv::Scalar(200,50,0));

    for (int i = 0; i < lines.size(); ++i){
        lines[i].realLength = calibration.realLength / sqrt(pow(calibration.p2.x - calibration.p1.x, 2) + pow(calibration.p2.y - calibration.p1.y, 2)) * sqrt(pow(lines[i].p2.x - lines[i].p1.x, 2) + pow(lines[i].p2.y - lines[i].p1.y, 2));
        line(img, lines[i].p1, lines[i].p2, cv::Scalar(0,200,0), 2);
        cv::putText(img, std::__cxx11::to_string(lines[i].realLength), (lines[i].p2 + lines[i].p1)/2, cv::FONT_HERSHEY_DUPLEX, 0.4, cv::Scalar(0,200,0));
    }
}

cv::Mat Session::getMat(){
    return img;
}
