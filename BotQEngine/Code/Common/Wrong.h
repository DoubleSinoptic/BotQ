#pragma once

class WrongType 
{
public:
	WrongType& operator=(const WrongType&) = delete;
	WrongType(const WrongType&) = delete;
	WrongType() = delete;
};