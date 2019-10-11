#include "MainWindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cam = new DrawingCam(0, DEF_IP, DEFAULT_PORT);
    std::thread mainLoopThread(&MainWindow::mainLoop, this);
    mainLoopThread.detach();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete cam;
}

void MainWindow::mainLoop()
{
    Mat current;
    current = this->cam->getNextFrame(this->shouldFlip, showDebug);
    this->ui->img_label->setPixmap(QPixmap::fromImage(QImage(current.data, current.cols, current.rows, current.step, QImage::Format_RGB888)));
    this->ui->img_label->setScaledContents( true );
    this->ui->img_label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    for(;;)
    {
        current = this->cam->getNextFrame(this->shouldFlip, showDebug);
        if(current.empty())
            break;
        cvtColor(current, current, COLOR_BGR2RGB);
        this->ui->img_label->setPixmap(QPixmap::fromImage(QImage(current.data, current.cols, current.rows, current.step, QImage::Format_RGB888)));
    }
    UniSocket::cleanup();
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

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_C)
        this->on_calibrate_bg_btn_clicked();
    else if(event->key() == Qt::Key_R)
        this->on_reset_canvas_btn_clicked();
    else if(event->key() == Qt::Key_S)
        this->on_sample_btn_clicked();
    else if(event->key() == Qt::Key_X)
        this->on_reset_sample_btn_clicked();
    else if(event->key() == Qt::Key_Escape)
        QCoreApplication::exit(0);

}


void MainWindow::on_show_debug_btn_clicked()
{
    this->showDebug = !this->showDebug;
}


void MainWindow::on_connect_btn_clicked()
{
    string ip = this->ui->ip_input->text().toStdString();
    int port = this->ui->port_input->text().toInt();
    if(port == 0)
    {
        std::cout << "Invalid Port" << std::endl;
        return;
    }
    if(this->cam->tryConnect(ip, port))
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
