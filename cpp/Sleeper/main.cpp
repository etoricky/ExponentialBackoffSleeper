#include "Sleeper.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <mutex>

std::chrono::milliseconds ms() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

int main_sleep() {
	std::mutex mutex;
	Sleeper sleeper{ 100, 6000 };
	auto start = ms();
	for (int i = 0; i < 10; ++i) {
		if (i == 8) {
			std::thread thread([&]() {
				std::this_thread::sleep_for(std::chrono::seconds(2));
				sleeper.stop(); // 100, 200, 400, 800, 1600, 3200, 6000, 6000, 4000
				{
					std::lock_guard<std::mutex> lock(mutex);
					std::cout << "sleeper stopped after 2s" << '\n';
				}
			});
			thread.detach();
		}
		long next = sleeper.next();
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << "sleeping for " << next << "ms" << '\n';
		}
		sleeper.sleep(); // 100, 200, 400, 800, 1600, 3200, 6000, 6000, 6000, 6000

		auto t = ms() - start;
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << t.count() << "ms elapsed after iteration" << i << '\n';
		}
	}
	std::cout << "main exit" << '\n';
	std::cin.get();
	return 0;
}


#include "Thread.h"
class MyThread : public Thread {
private:
	Sleeper sleeper{ 200, 60000 };
public:
	virtual void run() override {
		while (!done) {
			using namespace std::chrono;
			milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			std::cout << ms.count() << std::endl;
			sleeper.sleep(200); // or sleeper.sleep();
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

int main_thread() {
	MyThread thread;
	std::cout << "start 1" << std::endl;
	thread.start();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "start 2" << std::endl;
	thread.start();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	thread.stop();
	std::cout << "bye" << std::endl;
	std::cin.get();
	return 0;
}

int main() {
	main_sleep();
	main_thread();
	return 0;
}