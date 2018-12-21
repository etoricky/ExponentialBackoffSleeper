#include "Sleeper.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

class Thread {
private:    
    std::thread thread;
protected:
    std::atomic_bool done{false};
public:
    virtual ~Thread() {
        stop();
    }
    virtual void run() {}
    virtual void start() {
        stop();
        done = false;
        thread = std::thread(&Thread::run, this);
    }
    virtual void stop() {
        done = true;
        if (thread.joinable()) {
            thread.join();
        }
    }
};

class MyThread : public Thread {
private:
    Sleeper sleeper{200, 60000};
public:
    virtual void run() override {
        while (!done) {
            using namespace std::chrono;
            milliseconds ms = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
            );
            std::cout << ms.count() << std::endl;
            //std::this_thread::sleep_for(std::chrono::milliseconds(200));
            sleeper.sleep(200);
        }
    }
    virtual void start() {
        sleeper.reset();
        Thread::start();
    }
    virtual void stop() {
        sleeper.stop();
        Thread::stop();
    }
};

void main() {
    MyThread thread;
    std::cout << "start 1" << std::endl;
    thread.start();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    //std::cout << "start 2" << std::endl;
    //thread.start();
    //std::this_thread::sleep_for(std::chrono::seconds(2));
    //thread.stop();
    std::cout << "bye" << std::endl;
}