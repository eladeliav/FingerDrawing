#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QKeyEvent>
#include <string>
#include <iostream>
#include <thread>
#include <QtWidgets/QLabel>
#include <QButtonGroup>
#include <QTimer>
#include <QThread>
#include <fstream>
#include "include/DrawingCam.hpp"

using std::string;

#define DEF_IP "172.16.1.127" // default values
#define DEFAULT_PORT 1234
#define CONFIG_FILE_PATH "./config.cfg"

// set namespace to Ui
namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    // constructor for ui window
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow() override;

private slots:

    // slots for all buttons
    void on_sample_btn_clicked();

    void on_reset_sample_btn_clicked();

    void on_calibrate_bg_btn_clicked();

    void on_reset_canvas_btn_clicked();

    void on_flip_btn_clicked();

    void on_show_debug_btn_clicked();

    void on_disconnect_btn_clicked();

    void on_connect_btn_clicked();

    void red();

    void blue();

    void green();

    void eraser();

    void mainLoop();

    void on_size_slider_valueChanged(int value);

    void updateConfigFile();

private:
    Ui::MainWindow *ui; // ui object pointer
    DrawingCam *cam; // camera object pointer
    QTimer *updateTimer; // timer for invoking methods pointer
    Mat frame; // current frame
    QImage qt_image; // current qImage
    bool shouldFlip = true; // shouldflip boolean
    bool showDebug = false; // showdebug boolean
    bool done = false; // done
    std::string ip = DEF_IP;
    int port = DEFAULT_PORT;

    void keyPressEvent(QKeyEvent *event) override;
};

#endif // MAINWINDOW_H
