#pragma once

#include "Ref.h"
#include <cstdint>
#include "Defines.h"

#include <string.h>
#include <stdarg.h>

typedef unsigned short Char;

class SGE_EXPORT String
{
public:
	String();
	//-------------------------------------------------------
	String(const char* beg, const char* end);
	//-------------------------------------------------------
	String(const char* str);
	//-------------------------------------------------------
	String(size_t charscount, uint32_t ch);
	//-------------------------------------------------------
	String(const char* str, size_t charscount);
	//-------------------------------------------------------
	String(const String&) = default;
	//-------------------------------------------------------
	String& operator=(const String&) = default;
	//-------------------------------------------------------
	String& operator=(const char* str)
	{
		Set(str, strlen(str));
		return *this;
	}
	//-------------------------------------------------------	
	template<typename... Arguments>
	static String Format(const char* format, const Arguments&... args) 
	{
		int len = snprintf(nullptr, 0, format, args...);
		String r;
		r._ptr = new char[len + 1];
		snprintf(r._ptr.GetPtr(), len + 1, format, args...);
		r._length = len;
		return r;
	}
	//-------------------------------------------------------	
	template<typename... Arguments>
	void Scan(const char* format, Arguments&... args)
	{	
		sscanf(_ptr.GetPtr(), format, (&args)...);
	}
	//-------------------------------------------------------	
	void Append(const String& str);
	//-------------------------------------------------------
	void Append(const char* str);
	//-------------------------------------------------------
	void Append(uint32_t c);
	//-------------------------------------------------------
	void CopyTo(char* dest) const;
	//-------------------------------------------------------
	void Set(const char* str, size_t len);
	//-------------------------------------------------------
	void Set(Ref<char[]> str, size_t len);
	//-------------------------------------------------------
	int Compare(const String& str) const;
	//-------------------------------------------------------
	int Compare(const char* str) const;
	//-------------------------------------------------------
	bool Equals(const String& str) const;
	//-------------------------------------------------------
	bool Equals(const char* str) const;
	//-------------------------------------------------------
	const char* c_str() const;
	//-------------------------------------------------------
	const char* operator*()  const
	{
		return _ptr.GetPtr();
	}
	//-------------------------------------------------------
	int IndexOf(uint32_t ch) const;
	//-------------------------------------------------------
	int IndexOf(const String& str) const;
	//-------------------------------------------------------
	int IndexOf(const char* str) const;
	//-------------------------------------------------------
	int LastIndexOf(uint32_t ch) const;
	//-------------------------------------------------------
	bool Contains(const String& str) const;
	//-------------------------------------------------------
	bool Contains(const char* str) const;
	//-------------------------------------------------------
	String TrimLeft(uint32_t ch) const;
	//-------------------------------------------------------
	String TrimRight(uint32_t ch) const;
	//-------------------------------------------------------
	String Trim(uint32_t ch) const;
	//-------------------------------------------------------
	String Remove(int offset, int length) const;
	//-------------------------------------------------------
	String Replace(const String& exempl, const String& to) const;
	//-------------------------------------------------------
	String Replace(const char* exempl, const char* to) const;
	//-------------------------------------------------------
	size_t Length() const { return _length; }
	//-------------------------------------------------------
	void Swap(int idl, int idr);
	//-------------------------------------------------------
	bool IsEmpty() const { return _length == 0; }
	//-------------------------------------------------------
	void Clear()
	{
		_length = 0;
		_ptr = new char[1];
		_ptr[0] = '\0';
	}
	//------------------------------------------------------
	char& operator [](size_t index)
	{
		return _ptr[index];
	}
	//-------------------------------------------------------
	const char& operator [](size_t index) const
	{
		return _ptr[index];
	}
	//-------------------------------------------------------
	static uint32_t IterateUtf8(const char* &oktet);
	//-------------------------------------------------------
	//STL adapters
	FORCEINLINE char* begin()
	{
		return _ptr.GetPtr();
	}
	//------------------------------------------------------
	FORCEINLINE char* end()
	{
		return _ptr.GetPtr() + _length;
	}
	//------------------------------------------------------
	FORCEINLINE const char* cbegin() const
	{
		return _ptr.GetPtr();
	}
	//------------------------------------------------------
	FORCEINLINE const char* cend() const
	{
		return _ptr.GetPtr() + _length;
	}
	//------------------------------------------------------
	FORCEINLINE const char* operator*()
	{
		return _ptr.GetPtr();
	}
private:
	Ref<char[]> _ptr;
	size_t _length;
};

inline String& operator+=(String& lhs, uint32_t c)
{
	lhs.Append(c);
	return lhs;
}

inline String& operator+=(String& lhs, const char* rhs)
{
	lhs.Append(rhs);
	return lhs;
}

inline String& operator+=(String& lhs, const String& rhs)
{
	lhs.Append(rhs);
	return lhs;
}

inline String operator+(const String& lhs, const char* rhs)
{
	String str = lhs;
	str.Append(rhs);
	return str;
}

inline String operator+(const char* lhs, const String& rhs)
{
	String str = lhs;
	str.Append(rhs);
	return str;
}


inline String operator+(const String& lhs, const String& rhs)
{
	String str = lhs;
	str.Append(rhs);
	return str;
}

inline bool operator==(const String& lhs, const String& rhs)
{
	return lhs.Compare(rhs) == 0;
}

inline bool operator!=(const String& lhs, const String& rhs)
{
	return lhs.Compare(rhs) != 0;
}

inline bool operator< (const String& lhs, const String& rhs)
{
	return lhs.Compare(rhs) <  0;
}

inline bool operator> (const String& lhs, const String& rhs)
{
	return lhs.Compare(rhs) >  0;
}

inline bool operator<=(const String& lhs, const String& rhs)
{
	return lhs.Compare(rhs) <= 0;
}

inline bool operator>=(const String& lhs, const String& rhs)
{
	return lhs.Compare(rhs) >= 0;
}

inline bool operator==(const String& lhs, const char* rhs)
{
	return lhs.Compare(rhs) == 0;
}

inline bool operator!=(const String& lhs, const char* rhs)
{
	return lhs.Compare(rhs) != 0;
}

inline bool operator< (const String& lhs, const char* rhs)
{
	return lhs.Compare(rhs) <  0;
}

inline bool operator> (const String& lhs, const char* rhs)
{
	return lhs.Compare(rhs) >  0;
}

inline bool operator<=(const String& lhs, const char* rhs)
{
	return lhs.Compare(rhs) <= 0;
}

inline bool operator>=(const String& lhs, const char* rhs)
{
	return lhs.Compare(rhs) >= 0;
}
