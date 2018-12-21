#pragma once
#include <thread>
#include <atomic>

class Thread {
private:
	std::thread thread;
protected:
	std::atomic_bool done{ false };
public:
	virtual ~Thread() {
		stop();
	}
	virtual void run() {}
	void start() {
		stop();
		done = false;
		thread = std::thread(&Thread::run, this);
	}
	void stop() {
		done = true;
		if (thread.joinable()) {
			thread.join();
		}
	}
};