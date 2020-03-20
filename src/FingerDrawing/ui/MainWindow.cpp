#include "MainWindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cam = new DrawingCam(0, DEF_IP, DEFAULT_PORT);
    frame = this->cam->getNextFrame(this->shouldFlip);
    this->setFixedSize(frame.cols * 1.5, frame.rows + 20);
    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(mainLoop()));
    updateTimer->start(20);
//    std::thread mainLoopThread(&MainWindow::mainLoop, this);
//    mainLoopThread.detach();
}

MainWindow::~MainWindow()
{
    delete cam;
    delete updateTimer;
    UniSocket::cleanup();
    delete ui;
}

void MainWindow::mainLoop()
{
    if(done)
        return;

    if (!this->cam->connected())
        this->on_disconnect_btn_clicked();

    frame = this->cam->getNextFrame(this->shouldFlip);

    if (frame.empty())
        return;

    cvtColor(frame, frame, COLOR_BGR2RGB);

    if (done)
        return;

    qt_image = QImage((const unsigned char*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

    this->ui->img_label->setPixmap(QPixmap::fromImage(qt_image));
}

void MainWindow::on_sample_btn_clicked()
{
    this->cam->sampleSkinColor();
    this->ui->sample_btn->setEnabled(false);
    this->ui->reset_sample_btn->setEnabled(true);
}

void MainWindow::on_reset_sample_btn_clicked()
{
    this->cam->resetSkinColor();
    this->ui->sample_btn->setEnabled(true);
    this->ui->reset_sample_btn->setEnabled(false);
}

void MainWindow::on_calibrate_bg_btn_clicked()
{
    this->cam->calibrateBackground();
}

void MainWindow::on_reset_canvas_btn_clicked()
{
    this->cam->resetCanvas();
}


void MainWindow::on_flip_btn_clicked()
{
    this->shouldFlip = !this->shouldFlip;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_C)
        this->on_calibrate_bg_btn_clicked();
    else if (event->key() == Qt::Key_R)
        this->on_reset_canvas_btn_clicked();
    else if (event->key() == Qt::Key_S)
        this->on_sample_btn_clicked();
    else if (event->key() == Qt::Key_X)
        this->on_reset_sample_btn_clicked();
    else if (event->key() == Qt::Key_1)
        this->red();
    else if (event->key() == Qt::Key_2)
        this->blue();
    else if (event->key() == Qt::Key_3)
        this->green();
    else if (event->key() == Qt::Key_4)
        this->eraser();
    else if (event->key() == Qt::Key_Escape)
    {
        this->done = true;
        QCoreApplication::exit(0);
    } else if (event->key() == Qt::Key_T)
        this->cam->toggleMode();
    else if(event->key() == Qt::Key_Equal)
        this->cam->incSize();
    else if(event->key() == Qt::Key_Minus)
        this->cam->decSize();

}


void MainWindow::on_show_debug_btn_clicked()
{
    this->showDebug = !this->showDebug;
}


void MainWindow::on_connect_btn_clicked()
{
    string ip = this->ui->ip_input->text().toStdString();
    int port = this->ui->port_input->text().toInt();
    if (port == 0)
    {
        std::cout << "Invalid Port" << std::endl;
        return;
    }
    if (this->cam->tryConnect(ip, port))
    {
        this->ui->connect_btn->setEnabled(false);
        this->ui->disconnect_btn->setEnabled(true);
    }
}

void MainWindow::on_disconnect_btn_clicked()
{
    this->cam->disconnect();
    this->ui->connect_btn->setEnabled(true);
    this->ui->disconnect_btn->setEnabled(false);
}

void MainWindow::red()
{
    this->cam->setColor(RED);
}

void MainWindow::blue()
{
    this->cam->setColor(GREEN);
}

void MainWindow::green()
{
    this->cam->setColor(BLUE);
}

void MainWindow::eraser()
{
    this->cam->setColor(ERASER);
}

void MainWindow::on_size_slider_valueChanged(int value)
{
    this->cam->setSize(value);
}
