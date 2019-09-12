#include "MainWindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mainLoop()
{
    Mat current;
    DrawingCam cam(0, DEF_IP, DEFAULT_PORT);
    for (char user_input = cv::waitKey(10); user_input != 27; user_input = cv::waitKey(10))
    {
        current = cam.getNextFrame(user_input);
        cvtColor(current, current, COLOR_BGR2RGB);
        this->ui->img_label->setPixmap(QPixmap::fromImage(QImage(current.data, current.cols, current.rows, current.step, QImage::Format_RGB888)));
    }
}
