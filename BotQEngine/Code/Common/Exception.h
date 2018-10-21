#pragma once

#include <stdexcept>

class Exception : public std::exception
{
	char msgbuff[1024];
public:
	template<typename... Fmt>
	Exception(const char* fmt, const Fmt&... args) 
	{
		snprintf(msgbuff, 1024, fmt, args...);
	}

	const char* what() const noexcept override 
	{
		return msgbuff;
	}
};