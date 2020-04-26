//
// Created by elad on 23/08/2019.
//
#include <iostream>
#include <string>
#include "ui/MainWindow.hpp"
#include <QApplication>

using std::string;

#define DEF_IP "172.16.1.127" // default values
#define DEFAULT_PORT 1234

int main(int argc, char **argv)
{
    // opens ui window
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
