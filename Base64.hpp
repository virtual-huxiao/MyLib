#pragma once

#include <string>
#include <cassert>
#include <exception>

#include <bitset>

namespace ecllipse{
	class Base64_exception;

	class Base64;
}

class ecllipse::Base64_exception :std::exception {
	const char* msg_;
public:
	Base64_exception(const char* msg)
		:msg_{ msg } {
	}
	virtual char const* what() const override {
		return msg_;
	}
};

class ecllipse::Base64{
	constexpr static char Alphabet[] =	//base64字母表
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	constexpr static char Reverse_Alphabet[] = //通过ascii码拿到base64字母对应的值
	{	//-1为无效的内容对应,0-63为base64的64个对应字母
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
		-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
		-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
	};

public:
	//解码字符串,返回解析后的内容
	static inline std::string Decode(const std::string& str){
		return ecllipse::Base64::Decode(str.c_str(), str.size());
	}
	static std::string Decode(const char* s,size_t count){
		if ((count % 4) != 0) { 
			throw ecllipse::Base64_exception("invalid string size!"); 
		}
		std::string ret{};
		for (size_t i = 0; i < count; i+=4) {
			ret += char((Reverse_Alphabet[s[i]]<<2)|(Reverse_Alphabet[s[i+1]]>>4));	//第一个字符由第一个base64的6位和第二个base64的后2位组合
			ret += char(((Reverse_Alphabet[s[i+1]]&0b1111)<<4)|(Reverse_Alphabet[s[i+2]])>>2);//第二个字符由第二个base64的后4位和第三个base64的前4位组合
			ret += char(((Reverse_Alphabet[s[i+2]]&0b11)<<6)|Reverse_Alphabet[s[i+3]]);	//第三个字符串由第三个base64的后两位和第四个base64的6位组合
		}
		return ret;
	}

	//编码字符串,返回编码后的内容
	static inline std::string Encode(const std::string& str){
		return ecllipse::Base64::Encode(str.c_str(),str.size());
	}
	static inline std::string Encode(const char* s,size_t count){
		return "";
	}
};

