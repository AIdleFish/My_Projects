#pragma once
#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "Common.h"
#include<functional>

class ThreadPool {
private:
	bool thread_running; // 线程池是否正在运行
	std::vector<std::thread> workers; // 工作线程
	std::queue<std::function<void()>> tasks; // 任务队列
	std::mutex queue_mutex; // 保护任务队列的互斥锁
	std::condition_variable condition; // 条件变量用于通知工作线程有新任务
public:
	ThreadPool(int threads);
	~ThreadPool();
	void enqueue(std::function<void()> task); // 添加任务到线程池
	void stop(); // 停止线程池，等待所有线程完成
private:
	void worker(); // 工作线程函数
};

#endif // !THREADPOOL_H


