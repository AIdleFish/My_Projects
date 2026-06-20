#pragma once
#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include "Common.h"

bool recv_all(SOCKET sock, char* buf, int len);
bool send_all(SOCKET sock, const char* buf, int len);

bool recv_packet(SOCKET sock, std::string& out);
bool send_packet(SOCKET sock, const std::string& data);

#endif
