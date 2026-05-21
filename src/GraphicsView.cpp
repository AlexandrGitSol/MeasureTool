#include "GraphicsView.h"
#include "mainwindow.h"

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , currentMode(Pan)
    , isDrawing(false)
    , tempLine(nullptr)
    , tempPolygon(nullptr)
    , pixelsPerCm(0.0)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setRenderHint(QPainter::Antialiasing);
}

void GraphicsView::setMode(Mode mode)
{
    currentMode = mode;
    isDrawing = false;
    // В режиме Pan включаем перетаскивание рукой
    if (mode == Pan) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        setCursor(Qt::OpenHandCursor);
    } else {
        setDragMode(QGraphicsView::NoDrag);
        setCursor(Qt::CrossCursor);
    }
    // Очищаем временные фигуры
    if (tempLine) { delete tempLine; tempLine = nullptr; }
    if (tempPolygon) { delete tempPolygon; tempPolygon = nullptr; }
    polygonPoints.clear();
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    if (currentMode == Pan) {
        QGraphicsView::mousePressEvent(event);
        return;
    }
    if (currentMode == Calibration) {
        if (!isDrawing) {
            startPoint = scenePos;
            isDrawing = true;
        }
    }
    else if (currentMode == Line) {
        if (!isDrawing) {
            startPoint = scenePos;
            isDrawing = true;
        }
    }
    else if (currentMode == Polygon) {
        polygonPoints.append(scenePos);
        // Рисуем временную ломаную
        if (tempPolygon) delete tempPolygon;
        QPainterPath path;
        if (!polygonPoints.isEmpty()) {
            path.moveTo(polygonPoints.first());
            for (int i = 1; i < polygonPoints.size(); ++i)
                path.lineTo(polygonPoints[i]);
        }
        tempPolygon = scene()->addPath(path, QPen(Qt::blue, 2, Qt::DashLine));
    }
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    if (currentMode == Pan) {
        QGraphicsView::mouseMoveEvent(event);
        return;
    }
    if (currentMode == Calibration && isDrawing) {
        if (tempLine) delete tempLine;
        tempLine = scene()->addLine(QLineF(startPoint, scenePos), QPen(Qt::red, 2, Qt::DashLine));
    }
    else if (currentMode == Line && isDrawing) {
        if (tempLine) delete tempLine;
        tempLine = scene()->addLine(QLineF(startPoint, scenePos), QPen(Qt::red, 2, Qt::DashLine));
    }
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    if (currentMode == Pan) {
        QGraphicsView::mouseReleaseEvent(event);
        return;
    }
    if (currentMode == Calibration && isDrawing) {
        isDrawing = false;
        if (tempLine) delete tempLine;
        tempLine = nullptr;
        // Запрашиваем длину эталона
        bool ok;
        double realLength = QInputDialog::getDouble(this, "Калибровка",
                                                    "Введите реальную длину отрезка (в см):", 1.0, 0.0, 10000.0, 2, &ok);
        if (ok && realLength > 0) {
            static_cast<MainWindow*>(this->parent()->parent())->session.addCalibration(startPoint, scenePos, realLength);
            static_cast<MainWindow*>(this->parent()->parent())->session.renderImage();
            static_cast<MainWindow*>(this->parent()->parent())->updateView();
            //double pixelLength = QLineF(startPoint, scenePos).length();
            //pixelsPerCm = pixelLength / realLength;

        }
    }
    else if (currentMode == Line && isDrawing) {
        isDrawing = false;
        if (tempLine) delete tempLine;
        tempLine = nullptr;
        double pixelLength = QLineF(startPoint, scenePos).length();
        double realLength = (pixelsPerCm > 0) ? pixelLength / pixelsPerCm : 0.0;

        static_cast<MainWindow*>(this->parent()->parent())->session.addLine(startPoint, scenePos, realLength);
        static_cast<MainWindow*>(this->parent()->parent())->session.renderImage();
        static_cast<MainWindow*>(this->parent()->parent())->updateView();
    }
    // Для полигона завершение по двойному клику – см. mouseDoubleClickEvent
}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (currentMode == Polygon && polygonPoints.size() >= 3) {
        // Замыкаем полигон
        QPainterPath path;
        path.moveTo(polygonPoints.first());
        for (int i = 1; i < polygonPoints.size(); ++i)
            path.lineTo(polygonPoints[i]);
        path.closeSubpath();
        // Вычисляем площадь
        double areaPixels = 0.0;
        int n = polygonPoints.size();
        for (int i = 0; i < n; ++i) {
            const QPointF &p1 = polygonPoints[i];
            const QPointF &p2 = polygonPoints[(i+1)%n];
            areaPixels += p1.x() * p2.y() - p2.x() * p1.y();
        }
        areaPixels = std::abs(areaPixels) / 2.0;
        double realArea = (pixelsPerCm > 0) ? areaPixels / (pixelsPerCm * pixelsPerCm) : 0.0;
        // Рисуем полигон
        QGraphicsPolygonItem *polyItem = scene()->addPolygon(QPolygonF(polygonPoints), QPen(Qt::blue, 2), QBrush(Qt::cyan, Qt::DiagCrossPattern));
        measurementItems.append(polyItem);
        QString text = QString("%1 кв.см").arg(realArea, 0, 'f', 2);
        QGraphicsTextItem *textItem = scene()->addText(text);
        // Центр масс
        QPointF center(0,0);
        for (const auto &p : polygonPoints) center += p;
        center /= polygonPoints.size();
        textItem->setPos(center);
        measurementItems.append(textItem);
        emit measurementCreated("Polygon", realArea, polygonPoints);
        // Сброс полигона
        if (tempPolygon) delete tempPolygon;
        tempPolygon = nullptr;
        polygonPoints.clear();
        setMode(currentMode); // сброс временной графики
    }
    else {
        QGraphicsView::mouseDoubleClickEvent(event);
    }
}

void GraphicsView::clearAllMeasurements()
{
    for (QGraphicsItem *item : measurementItems) {
        scene()->removeItem(item);
        delete item;
    }
    measurementItems.clear();
}

void GraphicsView::undoLastMeasurement()
{
    if (!measurementItems.isEmpty()) {
        delete measurementItems.last();
        measurementItems.pop_back();
    }
}
