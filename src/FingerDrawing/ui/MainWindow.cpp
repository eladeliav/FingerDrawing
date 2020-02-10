#include "MainWindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cam = new DrawingCam(0, DEF_IP, DEFAULT_PORT);
    this->debugWindows = new DebugWindows(new QWidget, new QWidget);

    std::thread mainLoopThread(&MainWindow::mainLoop, this);
    mainLoopThread.detach();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete cam;
    delete debugWindows;
}

void MainWindow::mainLoop()
{
    Mat current, foreground, skinMask;
    Mat debugFrames[2];
    current = this->cam->getNextFrame(this->shouldFlip, debugFrames);
    this->ui->img_label->setScaledContents( true );
    this->ui->img_label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    this->debugWindows->foregroundLabel->setScaledContents( true );
    this->debugWindows->foregroundLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    this->debugWindows->skinLabel->setScaledContents( true );
    this->debugWindows->skinLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    for(;;)
    {
        current = this->cam->getNextFrame(this->shouldFlip, debugFrames);
        foreground = debugFrames[0];
        skinMask = debugFrames[1];

        if(current.empty())
            break;

        cvtColor(current, current,COLOR_BGR2RGB);
        cvtColor(foreground, foreground,COLOR_BGR2RGB);
        cvtColor(skinMask, skinMask, COLOR_BGR2RGB);

        this->ui->img_label->setPixmap(QPixmap::fromImage(QImage(current.data, current.cols, current.rows, current.step, QImage::Format_RGB888)));
        this->debugWindows->foregroundLabel->setPixmap(QPixmap::fromImage(QImage(foreground.data, foreground.cols, foreground.rows, foreground.step, QImage::Format_RGB888)));
        this->debugWindows->skinLabel->setPixmap(QPixmap::fromImage(QImage(skinMask.data, skinMask.cols, skinMask.rows, skinMask.step, QImage::Format_RGB888)));
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
    else if(event->key() == Qt::Key_1)
        this->on_ared_radio_clicked();
    else if(event->key() == Qt::Key_2)
        this->on_bblue_radio_clicked();
    else if(event->key() == Qt::Key_3)
        this->on_cgreen_radio_clicked();
    else if(event->key() == Qt::Key_4)
        this->on_deraser_radio_clicked();
    else if(event->key() == Qt::Key_Escape)
        QCoreApplication::exit(0);
    else if(event->key() == Qt::Key_T)
        this->cam->toggleMode();

}


void MainWindow::on_show_debug_btn_clicked()
{
    this->showDebug = !this->showDebug;
    this->debugWindows->foregroundWindow->setVisible(!this->debugWindows->foregroundWindow->isVisible());
    this->debugWindows->skinWindow->setVisible(!this->debugWindows->skinWindow->isVisible());
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

void MainWindow::setRadio(int index)
{
    const auto checkList = this->ui->color_box->findChildren<QRadioButton*>();
    checkList[index]->setChecked(true);
}

void MainWindow::on_ared_radio_clicked()
{
    setRadio(0);
    this->cam->setColor(RED);
}

void MainWindow::on_bblue_radio_clicked()
{
    setRadio(1);
    this->cam->setColor(GREEN);
}

void MainWindow::on_cgreen_radio_clicked()
{
    setRadio(2);
    this->cam->setColor(BLUE);
}

void MainWindow::on_deraser_radio_clicked()
{
    setRadio(3);
    this->cam->setColor(ERASER);
}
