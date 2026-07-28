#include "../include/ThreadPool.hpp"
#include <thread>
#include <functional>
#include <mutex>
#include <future>
#include <utility>
#include <memory>

ThreadPool::ThreadPool(std::size_t num_threads) : stop(false) {
	// fill threads vector with threads doing work()
	for (auto i{ 0 }; i < num_threads; ++i) {
		threads.emplace_back(&ThreadPool::work, this); // "this" needed to invoke work() on particular instance
	}
}

ThreadPool::~ThreadPool() {
	{
		std::unique_lock<std::mutex> lock(mutex);
		stop = true;
	}
	
	// wake up and join all threads
	con_var.notify_all();
	for (auto& thread : threads) {
		thread.join();
	}
}

// all threads loop here indefinitely, until either:
// 1. pool running AND no work to be done -> put to "sleep" 
// or 
// 2. pool stopped AND no work to be done -> quits
void ThreadPool::work() {
	while (true) {
		std::function<void()> current_task;

		{
			std::unique_lock<std::mutex> lock(mutex);

			// 1. sleep when pool running and no work to be done
			// predicate equivalent to:
			// while (!stop && tasks.empty())
			//	    con_var.wait(lock)
			con_var.wait(lock, [this]() { return stop || !tasks.empty(); });

			// 2. quit when pool stopped and no work to be done
			if (stop && tasks.empty()) {
				break;
			}

			// no tasks, continue
			if (tasks.empty()) {
				continue;
			}

			current_task = tasks.front(); 
			tasks.pop();
		}

		// execute current task
		current_task();
	}
}
