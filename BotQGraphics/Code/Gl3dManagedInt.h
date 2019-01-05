#pragma once

template<typename T, T init>
class ManagedInt
{
	T m_value;
public:
	constexpr ManagedInt(T val) :
		m_value(val)
	{}

	constexpr ManagedInt() :
		m_value(init)
	{}

	constexpr ManagedInt(const ManagedInt& value) :
		m_value(value.m_value)
	{}

	constexpr operator T() const
	{
		return m_value;
	}

	constexpr ManagedInt& operator=(T value)
	{
		m_value = value;
		return *this;
	}

	constexpr ManagedInt& operator=(const ManagedInt& value)
	{
		m_value = value.m_value;
		return *this;
	}

	constexpr ManagedInt& operator+=(const ManagedInt& value)
	{
		m_value += value.m_value;
		return *this;
	}

	constexpr ManagedInt& operator+=(T value)
	{
		m_value += value;
		return *this;
	}

	constexpr ManagedInt& operator/=(const ManagedInt& value)
	{
		m_value /= value.m_value;
		return *this;
	}

	constexpr ManagedInt& operator/=(T value)
	{
		m_value /= value;
		return *this;
	}

	constexpr ManagedInt& operator*=(const ManagedInt& value)
	{
		m_value *= value.m_value;
		return *this;
	}

	constexpr ManagedInt& operator*=(T value)
	{
		m_value *= value;
		return *this;
	}

	constexpr ManagedInt& operator-=(const ManagedInt& value)
	{
		m_value -= value.m_value;
		return *this;
	}

	constexpr ManagedInt& operator-=(T value)
	{
		m_value -= value;
		return *this;
	}

	constexpr ManagedInt& operator+()
	{
		return *this;
	}

	constexpr ManagedInt& operator-()
	{
		m_value = -m_value;;
		return *this;
	}
};

template<typename T, T init>
constexpr bool operator ==(T x, const ManagedInt<T, init>& v)
{
	return T(v) == x;
}

template<typename T, T init>
constexpr bool operator ==(const ManagedInt<T, init>& v, T x)
{
	return T(v) == x;
}

template<typename T, T init, typename X, X Xinit>
constexpr bool operator ==(const ManagedInt<T, init>& v, const ManagedInt<X, Xinit>&  x)
{
	return T(v) == X(x);
}

template<typename T, T init>
constexpr bool operator !=(T x, const ManagedInt<T, init>& v)
{
	return T(v) != x;
}

template<typename T, T init>
constexpr bool operator !=(const ManagedInt<T, init>& v, T x)
{
	return T(v) != x;
}

template<typename T, T init, typename X, X Xinit>
constexpr bool operator !=(const ManagedInt<T, init>& v, const ManagedInt<X, Xinit>&  x)
{
	return T(v) != X(x);
}