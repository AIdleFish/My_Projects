#include "MessageBuilder.h"

static const std::string TAG_RECV = "\x5b\xe6\x8e\xa5\xe6\x94\xb6\x5d\x28";
static const std::string TAG_SEND = "\x29\x5b\xe5\x8f\x91\xe9\x80\x81\x5d\x28";

std::string build_message(int sender_uid, int receiver_uid, const std::string& content) {
	return TAG_RECV + std::to_string(sender_uid) + TAG_SEND
		+ std::to_string(receiver_uid) + ")\"" + content + "\"";
}

bool parse_message(const std::string& raw, ParsedMessage& out) {
	if (raw.compare(0, TAG_RECV.size(), TAG_RECV) != 0) return false;

	size_t p1 = raw.find(')', TAG_RECV.size());
	if (p1 == std::string::npos) return false;

	std::string sender = raw.substr(TAG_RECV.size(), p1 - TAG_RECV.size());

	if (raw.compare(p1, TAG_SEND.size(), TAG_SEND) != 0) return false;

	size_t p3 = raw.find(')', p1 + TAG_SEND.size());
	if (p3 == std::string::npos) return false;

	std::string receiver = raw.substr(p1 + TAG_SEND.size(), p3 - (p1 + TAG_SEND.size()));

	if (p3 + 1 >= raw.size() || raw[p3 + 1] != '"') return false;
	size_t p4 = raw.find('"', p3 + 2);
	if (p4 == std::string::npos || p4 + 1 != raw.size()) return false;

	std::string content = raw.substr(p3 + 2, p4 - p3 - 2);

	try {
		out.sender_uid = std::stoi(sender);
		out.receiver_uid = std::stoi(receiver);
	}
	catch (...) {
		return false;
	}
	out.content = content;
	out.valid = true;
	return true;
}
