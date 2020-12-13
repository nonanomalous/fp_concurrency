#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

enum TrafficLightPhase
{
    red = 0,
    green = 1,
};

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    MessageQueue<T>() {};
    ~MessageQueue<T>() {};
    MessageQueue<T>(const MessageQueue<T> &) {};
    MessageQueue<T>(const MessageQueue<T> &&) {};
    MessageQueue<T> &operator=(const MessageQueue<T> &) {return *this;}
    MessageQueue<T> &operator=(const MessageQueue<T> &&) {return *this;}


    T receive();
    void send(T &&);

private:
    std::mutex _mtx;
    std::condition_variable _cv;
    std::deque<T> _queue;
};


class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _queue;
    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.

    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif