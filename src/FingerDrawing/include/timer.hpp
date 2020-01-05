//
// Created by elade on 1/5/2020.
//

#ifndef FINGERDRAWING_TIMER_HPP
#define FINGERDRAWING_TIMER_HPP
#include <iostream>
#include <thread>
#include <chrono>

class Timer {
    bool clear = false;

    public:
        void setTimeout(auto function, int delay);
        void setInterval(auto function, int interval);
        void stop()
        {
            this->clear = true;
        }

};

void Timer::setTimeout(auto function, int delay) {
    this->clear = false;
    std::thread t([=]() {
        if(this->clear) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(this->clear) return;
        function();
    });
    t.detach();
}

void Timer::setInterval(auto function, int interval) {
    this->clear = false;
    std::thread t([=]() {
        while(true) {
            if(this->clear) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(this->clear) return;
            function();
        }
    });
    t.detach();
}
#endif //FINGERDRAWING_TIMER_HPP
