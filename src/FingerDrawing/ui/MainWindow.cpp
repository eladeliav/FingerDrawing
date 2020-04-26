#include "MainWindow.hpp"
#include "ui_mainwindow.h"

// constructor
MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    // sets up ui
    ui->setupUi(this);
    // sets up drawing cam object
    cam = new DrawingCam(0, DEF_IP, DEFAULT_PORT);
    frame = this->cam->getNextFrame(this->shouldFlip); // gets first frame for size setup
    this->setFixedSize(frame.cols * 1.5, frame.rows + 20); // set size of frame
    updateTimer = new QTimer(this); // timer for repeating functions
    // do mainloop every 20 milliseconds
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(mainLoop()));
    updateTimer->start(20);
//    std::thread mainLoopThread(&MainWindow::mainLoop, this);
//    mainLoopThread.detach();
    updateConfigFile();
}

// frees memory and disconnects winsock library (if on windows)
MainWindow::~MainWindow()
{
    delete cam;
    delete updateTimer;
    UniSocket::cleanup();
    delete ui;
}

void MainWindow::mainLoop()
{
    // if done stop
    if (done)
        return;

    // if not connected simulate disconnect click so that the ui button updates
    if (!this->cam->connected())
        this->on_disconnect_btn_clicked();

    // gets frame
    frame = this->cam->getNextFrame(this->shouldFlip);

    if (frame.empty())
        return;

    // converts to rgb from bgr
    cvtColor(frame, frame, COLOR_BGR2RGB);

    // check done again
    if (done)
        return;

    // conver to QImage format
    qt_image = QImage((const unsigned char *) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

    // display frame
    this->ui->img_label->setPixmap(QPixmap::fromImage(qt_image));
}

// button to sample skin color
void MainWindow::on_sample_btn_clicked()
{
    this->cam->sampleSkinColor();
    this->ui->sample_btn->setEnabled(false);
    this->ui->reset_sample_btn->setEnabled(true);
}

// button to reset sampled skin color
void MainWindow::on_reset_sample_btn_clicked()
{
    this->cam->resetSkinColor();
    this->ui->sample_btn->setEnabled(true);
    this->ui->reset_sample_btn->setEnabled(false);
}

// calibrates background button
void MainWindow::on_calibrate_bg_btn_clicked()
{
    this->cam->calibrateBackground();
}

// resets canvas button
void MainWindow::on_reset_canvas_btn_clicked()
{
    this->cam->resetCanvas();
}

// flips frame
void MainWindow::on_flip_btn_clicked()
{
    this->shouldFlip = !this->shouldFlip;
}

// keyboard shortcuts for all buttons
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
    else if (event->key() == Qt::Key_Equal)
        this->cam->incSize();
    else if (event->key() == Qt::Key_Minus)
        this->cam->decSize();

}


// deprecated show debug window
void MainWindow::on_show_debug_btn_clicked()
{
    this->showDebug = !this->showDebug;
}

// attempt to connect button
void MainWindow::on_connect_btn_clicked()
{
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

// disconnect button
void MainWindow::on_disconnect_btn_clicked()
{
    this->cam->disconnect();
    this->ui->connect_btn->setEnabled(true);
    this->ui->disconnect_btn->setEnabled(false);
}

// buttons to set color
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

// size slider
void MainWindow::on_size_slider_valueChanged(int value)
{
    this->cam->setSize(value);
}

void MainWindow::updateConfigFile()
{
    std::ifstream config_file(CONFIG_FILE_PATH);
    if(!config_file.is_open())
    {
        std::stringstream s;
        s << "ip=" << DEF_IP << std::endl << "port=" << DEFAULT_PORT;
        std::ofstream new_config_file(CONFIG_FILE_PATH);
        new_config_file << s.str();
        new_config_file.close();
        config_file = std::ifstream(CONFIG_FILE_PATH);
    }
    std::string line;
    while(std::getline(config_file, line))
    {
        std::istringstream is_line(line);
        std::string key;
        if(std::getline(is_line, key, '='))
        {
            std::string value;
            if(std::getline(is_line, value))
            {
                try
                {
                    if(key == "ip")
                        this->ip = value;
                    else if(key == "port")
                        this->port = std::stoi(value);
                }catch(std::invalid_argument& e)
                {
                    std::cout << "Invalid Port or IP" << std::endl;
                    exit(1);
                }
            }
        }
    }
    config_file.close();
    std::cout << "IP: " << this->ip << ", Port: " << this->port << std::endl;
}
