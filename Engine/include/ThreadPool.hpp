#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>
#include <future>
#include <type_traits>

class ThreadPool {
	private:
		std::vector<std::thread> threads;
		std::mutex mutex;
		std::condition_variable con_var;
		std::queue<std::function<void()>> tasks;
		void work();
		bool stop;

	public:
		ThreadPool(std::size_t num_threads = std::thread::hardware_concurrency());
		~ThreadPool();

		template <typename F, typename... Args>
		auto enqueueTask(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

		ThreadPool(ThreadPool&) = delete;
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
};

#include "ThreadPool.inl"

#endif