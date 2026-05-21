#include "mainwindow.h"
#include "./ui_mainwindow.h"

QImage cvMatToQImage(const cv::Mat& mat) {
    if (mat.empty()) {
        return QImage();
    }

    // Преобразование для 8-битных изображений
    if (mat.type() == CV_8UC1) {                        // Черно-белое изображение
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }
    else if (mat.type() == CV_8UC3) {                  // Цветное изображение (BGR)
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    }
    else if (mat.type() == CV_8UC4) {                  // Цветное изображение (BGRA)
        cv::Mat rgba;
        cv::cvtColor(mat, rgba, cv::COLOR_BGRA2RGBA);
        return QImage(rgba.data, rgba.cols, rgba.rows, rgba.step, QImage::Format_RGBA8888).copy();
    }

    return QImage();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(nullptr)
    , currentPixelsPerCm(0.0)
{
    ui->setupUi(this);
    // Получаем указатель на продвинутый GraphicsView
    graphicsView = ui->graphicsView;
    setupUI();
    connectSignals();


    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    this->updateView();

    connect(ui->action, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(ui->action_4, &QAction::triggered, this, &MainWindow::onSaveFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateView(){
    QImage imgout = cvMatToQImage(session.getMat()); // 3. Конвертировать в QImage
    QPixmap pix = QPixmap::fromImage(imgout);        // 4. Конвертировать в QPixmap
    scene->clear();
    QGraphicsPixmapItem* item = scene->addPixmap(pix); // 5. Добавить на сцену (сразу возвращает указатель)
    scene->setSceneRect(pix.rect());
}

void MainWindow::onOpenFile(){
    QDialog dialog(this);
    dialog.setWindowTitle("Открыть");

    QVBoxLayout layout(&dialog);

    QLabel label("Введите путь к фото:");
    QLineEdit edit;
    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    layout.addWidget(&label);
    layout.addWidget(&edit);
    layout.addWidget(&buttons);

    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted){
        session.loadImage(edit.text());
    }
    this->updateView();

}

void MainWindow::onSaveFile(){
    QDialog dialog(this);
    dialog.setWindowTitle("Сохранить");

    QVBoxLayout layout(&dialog);

    QLabel label1("Введите путь к папке:");
    QLineEdit edit1;
    QLabel label2("Введите имя файла:");
    QLineEdit edit2;
    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    layout.addWidget(&label1);
    layout.addWidget(&edit1);
    layout.addWidget(&label2);
    layout.addWidget(&edit2);
    layout.addWidget(&buttons);

    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted){
        session.saveImage(edit1.text(), edit2.text());
    }
    this->updateView();
}

void MainWindow::setupUI()
{
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onPan);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onCalibration);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::onLine);
}

void MainWindow::connectSignals()
{
    connect(graphicsView, &GraphicsView::calibrationDone, this, &MainWindow::onCalibrationDone);
    connect(graphicsView, &GraphicsView::measurementCreated, this, &MainWindow::onMeasurementCreated);
}

void MainWindow::onPan() { graphicsView->setMode(GraphicsView::Pan); }
void MainWindow::onCalibration() { graphicsView->setMode(GraphicsView::Calibration); }
void MainWindow::onLine() { graphicsView->setMode(GraphicsView::Line); }

void MainWindow::onCalibrationDone(double ppm)
{
    currentPixelsPerCm = ppm;
    statusBar()->showMessage(QString("Калибровка: %1 пикс/см").arg(ppm));
}

void MainWindow::onMeasurementCreated(const QString& type, double value, const QVector<QPointF>& points)
{
    // Добавляем запись в список справа
    QListWidget *list = findChild<QListWidget*>("measurementsList");
    if (!list) {
        // если нет, создаём
        list = new QListWidget(this);
        QDockWidget *dock = new QDockWidget("Измерения", this);
        dock->setWidget(list);
        addDockWidget(Qt::RightDockWidgetArea, dock);
        list->setObjectName("measurementsList");
    }
    QString itemText = QString("%1: %2").arg(type).arg(value);
    list->addItem(itemText);
}
