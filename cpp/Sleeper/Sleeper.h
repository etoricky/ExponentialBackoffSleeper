#pragma once
#include <chrono>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <algorithm>

class Sleeper {

private:

	const long initialMs = 100;
	const long maxMs = 60000; // 60s
	long iteration = 0;
	mutable std::condition_variable cv;
	mutable std::mutex mutex;
	std::atomic_bool done{ false };

public:

	Sleeper(long initialMs, long maxMs) : initialMs(initialMs), maxMs(maxMs) {

	}

	~Sleeper() {
		stop();
	}

	long next() const {
		long durationMs = initialMs * pow(2, iteration);
		return (std::min)(durationMs, maxMs);
	}

	void sleep() {
		long durationMs = next();
		++iteration;
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
	}

	void stop() {
		std::unique_lock<std::mutex> lock(mutex);
		done = true;
		cv.notify_one();
	}

};