#include <iostream>
#include <future>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

 
template <class T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lk(_mtx);
    _cv.wait(lk, [this] { return !_queue.empty(); } );

    T msg = std::move(_queue.back());
    _queue.pop_back();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lk(_mtx);
    _queue.emplace_front(std::move(msg));
    if (_queue.size() > 1)
        _queue.pop_back();
    _cv.notify_one();
}


/* Implementation of class "TrafficLight" */
TrafficLight::TrafficLight() {
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if ( _queue.receive() == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // light changes random 4-6s duration
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(4,6);
    double cycleDuration = dis(gen) * 1000;
    
    // init stop watch
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    lastUpdate = std::chrono::system_clock::now();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();

        if (timeSinceLastUpdate >= cycleDuration)
        {
            _currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;
            
            //auto ftr = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, _queue, std::move(_currentPhase) );
            //ftr.wait();
            _queue.send(std::move(_currentPhase));
            lastUpdate = std::chrono::system_clock::now();
        }
    }
}
