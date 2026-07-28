#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <utility>

template <typename T>
class ThreadSafeQueue {
	private:
		std::mutex m;
		std::queue<T> q;

	public:
		ThreadSafeQueue() = default;
		size_t size() const {
			std::unique_lock<std::mutex> l(m);
			return q.size();
		}

		void push(T value) {
			std::unique_lock<std::mutex> l(m);
			q.push(std::move(value));
		}

		bool try_pop(T& front_val) {
			std::unique_lock<std::mutex> l(m);

			if (q.empty())
				return false;

			front_val = std::move(q.front());
			q.pop();

			return true;
		}
};

#endif
