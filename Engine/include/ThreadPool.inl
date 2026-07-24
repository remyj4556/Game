
// f, args... are *forwarding references*, can bind to anything (lvalue, rvalue, references, const/non-const)
template <typename F, typename... Args>
inline auto ThreadPool::enqueueTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
	// std::forward preserves exact type of arguments passed in
	// func is a callable wrapper for f with args bound to it, i.e., func() is f(args...)
	// thus, func() takes no arguments as is required for our tasks queue
	auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

	// construct a packaged_task to wrap func, and assign a shared_ptr to point at it
	auto encapsulated_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

	// get the future associated with the packaged_task
	std::future<decltype(f(args...))> future_return = encapsulated_ptr->get_future();

	// push "function"/task to queue
	{
		std::unique_lock<std::mutex> lock(mutex);
		// (*encapsulated_ptr)() gets us the callable std::function object from the packaged_task
		auto task_func = [encapsulated_ptr]() { (*encapsulated_ptr)(); };
		tasks.emplace(task_func);
	}

	// wake up a thread
	con_var.notify_one();

	return future_return;
}