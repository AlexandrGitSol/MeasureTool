#include "Session.h"

Session::Session() : path_to_img("/home/alexandr/Images/2023_Obsidian_logo.svg.png"){

    img = cv::imread("/home/alexandr/Images/2023_Obsidian_logo.svg.png");
    if (std::min(img.cols, img.rows) < 1000) cv::resize(img, img, cv::Size(img.cols * 2, img.rows * 2));
    orig_img = img.clone();
}

Session::Session(QString path_) : path_to_img(path_){
    img = cv::imread(path_.toStdString());
    if (std::min(img.cols, img.rows) < 1000) cv::resize(img, img, cv::Size(img.cols * 2, img.rows * 2));
    orig_img = img.clone();

}

bool Session::loadImage(QString path_){
    if (cv::imread(path_.toStdString()).empty()) return false;
    lines.clear();
    path_to_img = path_;
    calibration.realLength = 0;
    img = cv::imread(path_.toStdString());
    if (std::min(img.cols, img.rows) < 1000) cv::resize(img, img, cv::Size(img.cols * 2, img.rows * 2));
    orig_img = img.clone();
    //if (img.empty()) return false;
    return true;
}

void Session::saveImage(QString path_, QString name_){
    selectedline = -1;
    renderImage();
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
    if (std::min(orig_img.cols, orig_img.rows) < 1000) cv::resize(orig_img, orig_img, cv::Size(orig_img.cols * 2, orig_img.rows * 2));
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

/*
void Session::renderImage(){
    img = orig_img.clone();

    if (calibration.realLength != 0){
        line(img, calibration.p1, calibration.p2, cv::Scalar(200,50,0), 2);
        cv::putText(img, std::__cxx11::to_string(calibration.realLength), cv::Point((calibration.p2 + calibration.p1).x/2, (calibration.p2 + calibration.p1).y/2), cv::FONT_HERSHEY_DUPLEX, 0.4, cv::Scalar(200,50,0));
    }
    for (int i = 0; i < lines.size(); ++i){
        lines[i].realLength = calibration.realLength / sqrt(pow(calibration.p2.x - calibration.p1.x, 2) + pow(calibration.p2.y - calibration.p1.y, 2)) * sqrt(pow(lines[i].p2.x - lines[i].p1.x, 2) + pow(lines[i].p2.y - lines[i].p1.y, 2));
        line(img, lines[i].p1, lines[i].p2, cv::Scalar(0,200,0), 2);
        cv::putText(img, std::__cxx11::to_string(lines[i].realLength), (lines[i].p2 + lines[i].p1)/2, cv::FONT_HERSHEY_DUPLEX, 0.4, cv::Scalar(0,200,0));
    }
}
*/

void Session::renderImage() {
    img = orig_img.clone();

    // Форматирование с двумя знаками после запятой
    auto format = [](double val) -> std::string {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", val);
        return std::string(buf);
    };

    // Функция для вычисления позиции текста (смещение от центра линии)
    auto textPos = [](const cv::Point& p1, const cv::Point& p2) -> cv::Point {
        cv::Point center = (p1 + p2) / 2;
        cv::Point dir = p2 - p1;
        // Перпендикулярное смещение
        cv::Point perp(-dir.y, dir.x);
        double len = std::sqrt(perp.x * perp.x + perp.y * perp.y);
        if (len > 1e-5) {
            perp.x = static_cast<int>(perp.x / len * 15);
            perp.y = static_cast<int>(perp.y / len * 15);
        }
        // Если линия почти горизонтальна, смещаем вверх; иначе вбок
        if (std::abs(dir.x) > std::abs(dir.y))
            return cv::Point(center.x, center.y - 15);
        else
            return cv::Point(center.x + 15, center.y);
    };

    // Калибровка
    if (calibration.realLength != 0) {
        cv::line(img, calibration.p1, calibration.p2, cv::Scalar(200, 50, 0), 2 * std::min(img.rows, img.cols)/500);
        cv::Point pos = textPos(calibration.p1, calibration.p2);
        std::string text = format(calibration.realLength) + " cm";
        cv::putText(img, text, pos, cv::FONT_HERSHEY_DUPLEX, 0.5 * std::min(img.rows, img.cols)/800, cv::Scalar(200, 50, 0), 2 * std::min(img.rows, img.cols)/1400);
    }

    // Масштаб
    double scale = 0.0;
    if (calibration.realLength != 0) {
        double calibPix = std::hypot(calibration.p2.x - calibration.p1.x,
                                     calibration.p2.y - calibration.p1.y);
        scale = calibration.realLength / calibPix;
    }

    // Измерения
    for (int i = 0; i < lines.size(); ++i) {
        auto& line = lines[i];
        double pixLen = std::hypot(line.p2.x - line.p1.x, line.p2.y - line.p1.y);
        line.realLength = pixLen * scale;

        if (i != selectedline){
            cv::line(img, line.p1, line.p2, cv::Scalar(0, 200, 0), 2 * std::min(img.rows, img.cols)/500);
            cv::Point pos = textPos(line.p1, line.p2);
            std::string text = format(line.realLength) + " cm";
            cv::putText(img, text, pos, cv::FONT_HERSHEY_DUPLEX, 0.5 * std::min(img.rows, img.cols)/800, cv::Scalar(0, 200, 0), 2 * std::min(img.rows, img.cols)/1400);
        }
        else{
            cv::line(img, line.p1, line.p2, cv::Scalar(255, 255, 255), 2 * std::min(img.rows, img.cols)/500);
            cv::Point pos = textPos(line.p1, line.p2);
            std::string text = format(line.realLength) + " cm";
            cv::putText(img, text, pos, cv::FONT_HERSHEY_DUPLEX, 0.5 * std::min(img.rows, img.cols)/800, cv::Scalar(255, 255, 255), 2 * std::min(img.rows, img.cols)/1400);
        }
    }
}

void Session::delLine(int ind){
    lines.erase(lines.begin() + ind);
    renderImage();
}

std::vector<QString> Session::getLines(){
    std::vector<QString> ans;
    QString addline = "";
    for (int i = 0; i < lines.size(); ++i){
        addline = "Line";
        addline += QString::fromStdString(std::__cxx11::to_string(i)) + ": ";
        addline += QString::fromStdString(std::__cxx11::to_string(lines[i].realLength)) + " cm";
        ans.push_back(addline);
    }
    return ans;
}

void Session::selectedLine(int ind){
    selectedline = ind;
}

cv::Mat Session::getMat(){
    return img;
}
