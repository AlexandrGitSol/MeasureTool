#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QDebug>
#include <QToolBar>
#include <QAction>
#include <QDockWidget>
#include <QListWidget>
#include "Session.h"
#include "GraphicsView.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    QGraphicsScene *scene;

    Q_OBJECT

public:
    Session session;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateView();
    void updateListWidget();

private slots:
    void onOpenFile();
    void onSaveFile();
    void onLoadSession();
    void onSaveSession();
    void onPan();
    void onCalibration();
    void onLine();
    void onCalibrationDone(double ppm);
    void onDeleteLine();

private:
    Ui::MainWindow *ui;
    GraphicsView *graphicsView;
    double currentPixelsPerCm;
    void setupUI();
    void connectSignals();
};
#endif // MAINWINDOW_H
