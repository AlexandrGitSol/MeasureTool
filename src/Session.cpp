#include "Session.h"

Session::Session() : path_to_img("/home/alexandr/Images/2023_Obsidian_logo.svg.png"){

    img = cv::imread("/home/alexandr/Images/2023_Obsidian_logo.svg.png");
    orig_img = img.clone();
}

Session::Session(QString path_) : path_to_img(path_){
    img = cv::imread(path_.toStdString());
    orig_img = img.clone();

}

bool Session::loadImage(QString path_){
    lines.clear();
    calibration.realLength = 0;
    img = cv::imread(path_.toStdString());
    orig_img = img.clone();
    if (img.empty()) return false;
    return true;
}

void Session::saveImage(QString path_, QString name_){
    cv::imwrite(path_.toStdString() + "/" + name_.toStdString() + ".jpg", img);
}

bool Session::loadSession(QString path_){
    nlohmann::json jo;
    std::ifstream file(path_.toStdString());
    if (!file.is_open()) return false;
    file >> jo;
    file.close();
    path_to_img = QString::fromStdString(jo["path_to_image"]);
    calibration.p1 = cv::Point(jo["calibration"]["p1"]["x"], jo["calibration"]["p1"]["y"]);
    calibration.p2 = cv::Point(jo["calibration"]["p2"]["x"], jo["calibration"]["p2"]["y"]);
    calibration.realLength = jo["calibration"]["realLength"];

    lines.clear();
    for (const auto &l : jo["lines"]){
        Line addline;
        addline.p1 = cv::Point(l["p1"]["x"], l["p1"]["y"]);
        addline.p2 = cv::Point(l["p2"]["x"], l["p2"]["y"]);
        lines.push_back(addline);
    }
    orig_img = cv::imread(path_to_img.toStdString());
    renderImage();
    return true;
}

void Session::saveSession(QString path_, QString name_){
    nlohmann::json jo;
    jo["path_to_image"] = path_to_img.toStdString();
    jo["calibration"]["p1"]["x"] = calibration.p1.x;
    jo["calibration"]["p1"]["y"] = calibration.p1.y;
    jo["calibration"]["p2"]["x"] = calibration.p2.x;
    jo["calibration"]["p2"]["y"] = calibration.p2.y;
    jo["calibration"]["realLength"] = calibration.realLength;
    jo["lines"] = nlohmann::json::array();

    for (const auto &l : lines){
        nlohmann::json line;
        line["p1"]["x"] = l.p1.x;
        line["p1"]["y"] = l.p1.y;
        line["p2"]["x"] = l.p2.x;
        line["p2"]["y"] = l.p2.y;
        jo["lines"].push_back(line);
    }

    std::ofstream file(path_.toStdString() + "/" + name_.toStdString() + ".json");
    file << jo.dump(4);
    file.close();

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

    if (calibration.realLength != 0){
        line(img, calibration.p1, calibration.p2, cv::Scalar(200,50,0), 2);
        cv::putText(img, std::__cxx11::to_string(calibration.realLength), (calibration.p2 + calibration.p1)/2, cv::FONT_HERSHEY_DUPLEX, 0.4, cv::Scalar(200,50,0));
    }
    for (int i = 0; i < lines.size(); ++i){
        lines[i].realLength = calibration.realLength / sqrt(pow(calibration.p2.x - calibration.p1.x, 2) + pow(calibration.p2.y - calibration.p1.y, 2)) * sqrt(pow(lines[i].p2.x - lines[i].p1.x, 2) + pow(lines[i].p2.y - lines[i].p1.y, 2));
        line(img, lines[i].p1, lines[i].p2, cv::Scalar(0,200,0), 2);
        cv::putText(img, std::__cxx11::to_string(lines[i].realLength), (lines[i].p2 + lines[i].p1)/2, cv::FONT_HERSHEY_DUPLEX, 0.4, cv::Scalar(0,200,0));
    }
}

cv::Mat Session::getMat(){
    return img;
}
