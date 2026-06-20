#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Protocol.h"

bool recv_all(SOCKET sock, char* buf, int len) {
	int received = 0;
	while (received < len) {
		int n = recv(sock, buf + received, len - received, 0);
		if (n <= 0) return false;
		received += n;
	}
	return true;
}

bool send_all(SOCKET sock, const char* buf, int len) {
	int sent = 0;
	while (sent < len) {
		int n = send(sock, buf + sent, len - sent, 0);
		if (n <= 0) return false;
		sent += n;
	}
	return true;
}

bool recv_packet(SOCKET sock, std::string& out) {
	char header[HEADER_SIZE] = { 0 };
	if (!recv_all(sock, header, HEADER_SIZE)) return false;

	uint32_t body_len = 0;
	memcpy(&body_len, header, HEADER_SIZE);
	body_len = ntohl(body_len);

	if (body_len == 0 || body_len > 65536) return false;

	std::vector<char> body(body_len);
	if (!recv_all(sock, body.data(), static_cast<int>(body_len))) return false;

	out.assign(body.data(), body_len);
	return true;
}

bool send_packet(SOCKET sock, const std::string& data) {
	uint32_t len = htonl(static_cast<uint32_t>(data.size()));
	char header[HEADER_SIZE];
	memcpy(header, &len, HEADER_SIZE);

	if (!send_all(sock, header, HEADER_SIZE)) return false;
	if (data.empty()) return true;
	return send_all(sock, data.c_str(), static_cast<int>(data.size()));
}
