#include "../include/MainWindow.hpp"
#include "../ui_mainwindow.h"

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

void MainWindow::on_pushButton_clicked()
{
    string ip = DEF_IP;
    int port = DEFAULT_PORT;
    string answer;
    userInput:
    std::cout << "Use default settings?[y]es/[n]o" << std::endl;
    std::cin >> answer;
    if(answer == "n")
    {
        std::cout << "ip: " << std::endl;
        std::cin >> ip;
        std::cout << "port: " << std::endl;
        std::cin >> port;
    }
    else if(answer != "y")
    {
        std::cout << "Invalid input..." << std::endl;
        goto userInput;
    }
    DrawingCam cam(0, ip, port);
    cam.start();
}
