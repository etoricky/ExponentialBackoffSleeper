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
	virtual void start() {
		done = true;
		if (thread.joinable()) {
			thread.join();
		}
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