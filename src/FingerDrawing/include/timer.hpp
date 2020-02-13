//
// Created by elade on 1/5/2020.
//

#ifndef FINGERDRAWING_TIMER_HPP
#define FINGERDRAWING_TIMER_HPP
#include <iostream>
#include <thread>
#include <chrono>


template<class T>
class Timer {
    typedef void(T::*FunctionPointer)();
    bool clear = false;

    public:
        void setTimeout(FunctionPointer function, int delay, T* th)
        {
            this->clear = false;
            std::thread t([=]() {
                if(this->clear) return;
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                if(this->clear) return;
                (th->*function)();
            });
            t.detach();
        }
        void setInterval(void(*function)(T*) , int interval, T* th)
        {
            this->clear = false;
            std::thread t([=]() {
                while(true) {
                    if(this->clear) return;
                    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                    if(this->clear) return;
                    if(th)
                        function(th);
                    else
                        function(nullptr);
                }
            });
            t.detach();
        }
        void stop()
        {
            this->clear = true;
        }

};

#endif //FINGERDRAWING_TIMER_HPP
