#include"OnLine.h"
#include <wincrypt.h>
#include <iomanip>
#include <sstream>
#include <iostream>

OnLine::OnLine(DBHelper& db) : db_(db) {}

std::string md5_hash(const std::string& input) {
	HCRYPTPROV prov = 0;
	HCRYPTHASH hash = 0;
	if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		return input;
	}
	if (!CryptCreateHash(prov, CALG_MD5, 0, 0, &hash)) {
		CryptReleaseContext(prov, 0);
		return input;
	}
	if (!CryptHashData(hash, reinterpret_cast<const BYTE*>(input.c_str()),
		static_cast<DWORD>(input.size()), 0)) {
		CryptDestroyHash(hash);
		CryptReleaseContext(prov, 0);
		return input;
	}
	BYTE digest[16];
	DWORD len = 16;
	if (!CryptGetHashParam(hash, HP_HASHVAL, digest, &len, 0)) {
		CryptDestroyHash(hash);
		CryptReleaseContext(prov, 0);
		return input;
	}
	CryptDestroyHash(hash);
	CryptReleaseContext(prov, 0);

	std::ostringstream oss;
	for (DWORD i = 0; i < len; ++i) {
		oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
	}
	return oss.str();
}


bool OnLine::Register(const std::string& username, const std::string& password) {
	std::string fwd = md5_hash(password);
	std::string sql = "SELECT id FROM user WHERE username='" + username + "'AND password ='" + fwd + "'";
	MYSQL_RES* res = db_.QuerySQL(sql.c_str());
	if (mysql_num_rows(res) > 0) return true;
	return false;
}

std::string OnLine::get_uid(const std::string& username) {
	if (!db_.isConnected()) {
		std::cout << "数据库未连接" << std::endl;
		return "";
	}
	std::string sql = "SELECT uid FROM user WHERE username =" + db_.escape(username);
	MYSQL_RES* res = db_.QuerySQL(sql.c_str());
	MYSQL_ROW row = mysql_fetch_row(res);
	return row[0];
}