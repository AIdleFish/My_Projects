#include "LogManager.h"

// 获取当前时间的字符串表示，格式为 "YYYY-MM-DD HH:MM:SS"
static std::string current_time_str() {
	auto now = std::chrono::system_clock::now();// 获取当前系统时间点
	auto t = std::chrono::system_clock::to_time_t(now);// 将时间点转换为 time_t 类型
	std::tm tm_buf{}; // 定义一个 tm 结构体变量，用于存储时间信息
	localtime_s(&tm_buf, &t);// 将 time_t 类型的时间转换为 tm 结构体，使用 localtime_s 函数以确保线程安全
	std::ostringstream oss;// 定义一个字符串输出流对象，用于格式化时间字符串
	oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");// 使用 std::put_time 格式化 tm 结构体为 "YYYY-MM-DD HH:MM:SS" 的字符串格式，并将其写入字符串输出流
	return oss.str();// 返回格式化后的时间字符串
}

// 添加日志记录，参数包括原始消息内容、发送者 UID、接收者 UID 和消息内容
void LogManager::add(const std::string& raw, const std::string& sender,
	const std::string& receiver, const std::string& content) {
	std::lock_guard<std::mutex> lock(mtx_);
	LogEntry entry;// 创建一个 LogEntry 结构体变量，用于存储日志记录的信息
	entry.time = current_time_str();// 获取当前时间的字符串表示，并将其赋值给日志记录的 time 字段
	entry.raw_message = raw;
	entry.sender_uid = sender;
	entry.receiver_uid = receiver;
	entry.content = content;
	logs_.push_back(std::move(entry));// 将日志记录添加到日志列表中，move函数可以直接将 entry 的内容移动到 logs_ 中，避免不必要的复制
}

// 打印所有日志记录，输出格式为 "[时间] 发送者UID -> 接收者UID : 消息内容"
void LogManager::print_all() const {
	std::lock_guard<std::mutex> lock(mtx_);// 锁上logs_，确保在打印日志时不会有其他线程修改日志列表
	if (logs_.empty()) {
		std::cout << "暂无日志记录" << std::endl;
		return;
	}
	std::cout << "========== 消息日志 (共 " << logs_.size() << " 条) ==========" << std::endl;
	for (size_t i = 0; i < logs_.size(); ++i) {// 用size_t可以兼容不同平台，避免类型不匹配的问题
		const auto& e = logs_[i];
		std::cout << "[" << (i + 1) << "] " << e.time << std::endl;
		std::cout << "    原始消息: " << e.raw_message << std::endl;
		std::cout << "    发送方UID: " << e.sender_uid
			<< "  接收方UID: " << e.receiver_uid << std::endl;
		std::cout << "    内容: " << e.content << std::endl;
	}
	std::cout << "============================================" << std::endl;
}

// 清除所有日志记录
void LogManager::clear() {
	std::lock_guard<std::mutex> lock(mtx_);
	logs_.clear();
}

// 获取日志记录的数量
size_t LogManager::count() const {
	std::lock_guard<std::mutex> lock(mtx_);
	return logs_.size();
}
