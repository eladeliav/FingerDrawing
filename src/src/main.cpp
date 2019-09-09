//
// Created by elad on 23/08/2019.
//
#include <iostream>
#include "../include/DrawingCam.hpp"

int main(int argc, char **argv)
{

    std::cout << "Enter inPort: " << std::endl;
    int inPort, outPort;
    std::cin >> inPort;
    std::cout << "Enter outport: " << std::endl;
    std::cin >> outPort;
    ConnectionDetails details("127.0.0.1", inPort, outPort, SOMAXCONN, 4);
    Communicator com(details);
    DrawingCam cam = DrawingCam();
    cam.start();
    return 0;
}
