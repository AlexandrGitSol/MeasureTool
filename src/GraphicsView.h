#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QVector>
#include <QPointF>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QPen>
#include <QInputDialog>


class MainWindow;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    enum Mode { Pan, Calibration, Line, Polygon };
    explicit GraphicsView(QWidget *parent = nullptr);
    void setMode(Mode mode);
    void clearAllMeasurements();
    void undoLastMeasurement();

signals:
    void calibrationDone(double pixelsPerCm);
    void measurementCreated(const QString& type, double value, const QVector<QPointF>& points);
    void lineMeasured(double lengthCm);         // когда измерили линию

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    Mode currentMode;
    bool isDrawing;
    QPointF startPoint;
    QVector<QPointF> polygonPoints;
    QGraphicsLineItem *tempLine;
    QGraphicsPathItem *tempPolygon;
    QVector<QGraphicsItem*> measurementItems; // для undo/clear
    double pixelsPerCm; // установленная калибровка
};

#endif // GRAPHICSVIEW_H
