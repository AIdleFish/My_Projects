#include "ThreadPool.h"
#include "Common.h"

ThreadPool::ThreadPool(int threads) : thread_running(true) {
	for (int i = 0; i < threads; ++i) { // 创建10个线程
		workers.emplace_back(&ThreadPool::worker, this);
	}
	LOG_INFO("线程池初始化完成, 线程数有: " << threads);
}

ThreadPool::~ThreadPool() {
	stop();
}

void ThreadPool::enqueue(std::function<void()> task) {
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		tasks.push(std::move(task));// 添加一个任务到任务队列
	}
	condition.notify_one();// 通知一个线程完成
}

void ThreadPool::stop() {
	if (!thread_running) return;
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		thread_running = false;
	}
	condition.notify_all();// 通知所有线程完成剩下所有任务
	for (std::thread& worker : workers) {
		if (worker.joinable()) {
			worker.join();
		}
	}
	LOG_INFO("线程池已停止");
}

void ThreadPool::worker() {
	while (true) {// 一直等待任务队列添加任务
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			condition.wait(lock, [this] { return !tasks.empty() || !thread_running; });
			if (!thread_running && tasks.empty()) {
				return;
			}
			task = std::move(tasks.front());
			tasks.pop();
		}
		task();// 执行任务
	}
}