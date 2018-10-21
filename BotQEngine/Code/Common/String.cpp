#include "String.h"
#include "DynamicArray.h"
#include "Utf8.h"

#include <stdexcept>
#include <stdarg.h>
#include <stdio.h>

String::String()
{
	_length = 0;
}

//-------------------------------------------------------

String::String(const char* beg, const char* end)
	: String(beg, end - beg)
{

}

//-------------------------------------------------------

String::String(const char * str)
{
	Set(str, strlen(str));
}

//-------------------------------------------------------

String::String(size_t charscount, uint32_t ch)
{
	size_t onchrlen = utf8::char_length_in_utf8(ch);
	_length = charscount * onchrlen;
	_ptr = new char[_length + 1];
	char* p = _ptr.GetPtr();
	p[_length] = 0;
	size_t sz = 0;
	while (_length != sz)
	{
		sz += utf8::write_utf8_char((uint8_t* &)p, ch);
		p += onchrlen;
	}
}

//-------------------------------------------------------

String::String(const char * str, size_t charscount)
{
	Set(str, charscount);
}

//-------------------------------------------------------	

void String::Append(const String & str)
{
	Append(*str);
}

//-------------------------------------------------------

void String::Append(const char * str)
{
	size_t lens = strlen(str);
	size_t rezlens = lens + _length;
	Ref<char[]> ptrnew = new char[rezlens + 1];
	char* ptr = ptrnew.GetPtr();
	ptr[rezlens] = 0;
	CopyTo(ptr);
	ptr += _length;
	for (int i = 0; i < lens; i++)
		ptr[i] = str[i];
	_ptr = ptrnew;
	_length = rezlens;
}


//-------------------------------------------------------


void String::Append(uint32_t c)
{
	char utf8buffer[5];
	utf8buffer[utf8::write_utf8_char((uint8_t*)utf8buffer, c)] = '\0';
	Append(utf8buffer);
}

//-------------------------------------------------------

void String::CopyTo(char * dest) const
{
	for (int i = 0; i < _length; i++)
		dest[i] = _ptr[i];
}

//-------------------------------------------------------

void String::Set(const char * str, size_t len)
{
	_length = len;
	_ptr = new char[_length + 1];
	char* p = _ptr.GetPtr();
	p[_length] = 0;
	for (int i = 0; i < _length; i++)
		p[i] = str[i];
}

//-------------------------------------------------------

void String::Set(Ref<char[]> str, size_t len)
{
	_ptr = str;
	_length = len;
}

//-------------------------------------------------------

int String::Compare(const String & str) const
{
	return strcmp(_ptr.GetPtr() ? _ptr.GetPtr() : "", str._ptr.GetPtr() ? str._ptr.GetPtr() : "");
}

//-------------------------------------------------------

int String::Compare(const char * str) const
{
	return strcmp(_ptr.GetPtr() ? _ptr.GetPtr() : "", str);
}

//-------------------------------------------------------

bool String::Equals(const String & str) const
{
	return Compare(str) == 0;
}

//-------------------------------------------------------

bool String::Equals(const char * str) const
{
	return Compare(str) == 0;
}

//-------------------------------------------------------

const char * String::c_str() const
{
	return _ptr.GetPtr();
}

//-------------------------------------------------------

int String::IndexOf(uint32_t ch) const
{
	const char* ptr = _ptr.GetPtr();
	for (int i = 0; i < _length; i++)
		if (ptr[i] == ch)
			return i;
	return -1;
}

int String::IndexOf(const String & str) const
{
	return IndexOf(*str);
}

int String::IndexOf(const char * str) const
{
	size_t st = strlen(str);
	const char* ptr = _ptr.GetPtr();
	auto cheker = [&](int chid) -> bool
	{
		for (int i = 0; i < st; i++)
		{
			if ((chid + i) == _length)
				return false;
			if (str[i] != ptr[chid + i])
				break;
			if (i == (st - 1))
				return true;
		}
		return false;
	};
	for (int i = 0; i < _length; i++)
		if (cheker(i))
			return i;
	return -1;
}

//-------------------------------------------------------

int String::LastIndexOf(uint32_t ch) const
{
	const char* ptr = _ptr.GetPtr();
	for (int i = (_length - 1); i > -1; i--)
		if (ptr[i] == ch)
			return i;
	return -1;
}

//-------------------------------------------------------

bool String::Contains(const String & str) const
{
	return Contains(*str);
}

//-------------------------------------------------------

bool String::Contains(const char* str) const
{
	size_t st = strlen(str);
	const char* ptr = _ptr.GetPtr();
	auto cheker = [&](int chid) -> bool
	{
		for (int i = 0; i < st; i++)
		{
			if ((chid + i) == _length)
				return false;
			if (str[i] != ptr[chid + i])
				break;
			if (i == (st - 1))
				return true;
		}
		return false;
	};
	for (int i = 0; i < _length; i++)
		if (cheker(i))
			return true;
	return false;
}

//-------------------------------------------------------

String String::TrimLeft(uint32_t ch)  const
{
	String rz = *this;
	size_t rmsz = 0;
	for (int i = 0; i < _length; i++)
	{
		if (_ptr[i] == ch)
			rmsz++;
		else
			break;
	}
	memmove(rz._ptr.GetPtr(), _ptr.GetPtr() + rmsz, _length - rmsz);
	rz._ptr.GetPtr()[_length - rmsz] = '\0';
	rz._length = _length - rmsz;
	return rz;
}

//-------------------------------------------------------

String String::TrimRight(uint32_t ch) const
{
	String rz = *this;
	size_t rmsz = 0;
	for (int i = (_length - 1); i > -1; i--)
	{
		if (_ptr[i] == ch)
			rmsz++;
		else
			break;
	}
	rz[_length - rmsz] = 0;
	rz._length = _length - rmsz;
	return rz;
}
//-------------------------------------------------------

String String::Trim(uint32_t ch) const
{
	return (*this).TrimRight(ch).TrimLeft(ch);
}

//-------------------------------------------------------

String String::Remove(int offset, int length) const
{
	DynamicArray<char> rezult;
	rezult.EnsureCapacity(_length);
	char* ptr = rezult.GetData();
	size_t sublen = 0;
	for (int i = 0; i < _length; i++)
		if ((i < offset) || (i >= (offset + length)))
			rezult.Add(_ptr[i]);
		else
			sublen++;
	rezult.Add('\0');
	String rz;
	rz.Set(rezult.GetShared(), _length - sublen);
	return rz;
}

//-------------------------------------------------------

String String::Replace(const String & exempl, const String & to) const
{
	return Replace(*exempl, *to);
}

//-------------------------------------------------------

String String::Replace(const char* exempl, const char* to) const
{
	size_t exlen = strlen(exempl);
	DynamicArray<char> rezult;
	rezult.EnsureCapacity(100);
	return "";
}

//-------------------------------------------------------

void String::Swap(int idl, int idr)
{
	char ch = _ptr[idl];
	_ptr[idl] = _ptr[idr];
	_ptr[idr] = ch;
}

//-------------------------------------------------------

uint32_t String::IterateUtf8(const char* &oktet)
{
	return utf8::utf8_char_iterator((const uint8_t* &)oktet);
}