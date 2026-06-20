#pragma once
#ifndef MESSAGE_BUILDER_H
#define MESSAGE_BUILDER_H

#include <string>

struct ParsedMessage {
	int sender_uid = 0;
	int receiver_uid = 0;
	std::string content;
	bool valid = false;
};

std::string build_message(int sender_uid, int receiver_uid, const std::string& content);
bool parse_message(const std::string& raw, ParsedMessage& out);

#endif
