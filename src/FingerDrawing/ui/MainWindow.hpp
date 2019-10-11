#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QKeyEvent>
#include <string>
#include <iostream>
#include <thread>
#include "include/DrawingCam.hpp"

using std::string;

#define DEF_IP "172.16.1.127"
#define DEFAULT_PORT 1234

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void mainLoop();
private slots:
    void on_sample_btn_clicked();

    void on_reset_sample_btn_clicked();

    void on_calibrate_bg_btn_clicked();

    void on_reset_canvas_btn_clicked();

    void on_flip_btn_clicked();

    void on_show_debug_btn_clicked();

    void on_disconnect_btn_clicked();

    void on_connect_btn_clicked();

private:
    Ui::MainWindow *ui;
    DrawingCam* cam;
    bool shouldFlip = true;
    bool showDebug = false;
    void keyPressEvent(QKeyEvent* event) override;
};

#endif // MAINWINDOW_H
