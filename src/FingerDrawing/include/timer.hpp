//
// Created by elade on 1/5/2020.
//

#ifndef FINGERDRAWING_TIMER_HPP
#define FINGERDRAWING_TIMER_HPP

#include <iostream>
#include <thread>
#include <chrono>

// class that imitates setInterval from JS
template<class T>
class Timer
{
    // template so that the given functions can only be member functions of a given class
    typedef void(T::*FunctionPointer)();

    bool clear = false; // stop loops

public:
    // repeats a function in a given interval
    void setInterval(void(*function)(T *), int interval, T *th)
    {
        this->clear = false;
        std::thread t([=]()
                      {
                          while (true)
                          {
                              if (this->clear) return;
                              std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                              if (this->clear) return;
                              if (th)
                                  function(th);
                              else
                                  function(nullptr);
                          }
                      });
        t.detach();
    }

    // stops repeating
    void stop()
    {
        this->clear = true;
    }

};

#endif //FINGERDRAWING_TIMER_HPP
