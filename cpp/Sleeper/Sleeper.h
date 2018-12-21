#pragma once
#include <chrono>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <algorithm>

class Sleeper {

private:

	const long initialMs = 100;
	const long maxMs = 0;
	long iteration = 0;
	mutable std::condition_variable cv;
	mutable std::mutex mutex;
	std::atomic_bool done{ false };

public:

	Sleeper(long maxMs=0) : maxMs(maxMs) {

	}

	~Sleeper() {
		stop();
	}

	long next(long ms) const {
		if (maxMs == 0) {
			return ms;
		}
		else {
			long durationMs = static_cast<long>(ms * powl(2, iteration));
			return (std::min)(durationMs, maxMs);
		}
	}

	void sleep(long ms) {
		long durationMs = next(ms);
		if (maxMs != 0) {
			++iteration;
		}
		const auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(durationMs);
		while (!done) {
			std::unique_lock<std::mutex> lock(mutex);
			std::cv_status status = cv.wait_until(lock, end);
			if (status == std::cv_status::timeout || done) {
				break;
			}
		}
	}

	void reset() {
		iteration = 0;
		done = false;
	}

	void stop() {
		std::unique_lock<std::mutex> lock(mutex);
		done = true;
		cv.notify_one();
	}

};