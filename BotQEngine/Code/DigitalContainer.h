#pragma once

/*
�� ������������ big-endian ���������...
�������� � �������������
*/
class DigitalContainer
{
public:
	typedef  void* DataStorage;

	inline DigitalContainer() :
		mData(0)
	{}

	inline DigitalContainer(DataStorage aData) :
		mData(aData)
	{}

	inline bool IsNull() const
	{
		return  mData == 0;
	}

	template<typename T>
	inline void Set(const T& value)
	{
		static_assert(sizeof(T) <= sizeof(DataStorage), "error put type to digital container");
		mData = 0;
		*reinterpret_cast<T*>(&mData) = value;	
	}


	/*
	����� �������� ��� ����� ������� ����������� ������ ������ ���� � ���������
	������ ��� ����������.
	��-���� ���� sizeof(void*) == 8 � sizeof(T) == 4 ��
	��������� ������ ������ ��� ����� �� value... � ������ ��� �� ����.
	������ �� �������� �������� ��� �� �������� ���-��� �������� ����-�� ��� � ������
	���������. ��� ������� ������� ��� �� ��� ������
	template<typename T>
	inline void Set(const T& value)
	{
		static_assert(sizeof(T) <= sizeof(DataStorage), "error put type to digital container");
		mData = *reinterpret_cast<DataStorage const*>(&value);
	}*/

	template<typename  T>
	inline T Get() const
	{
		static_assert(sizeof(T) <= sizeof(DataStorage), "error put type to digital container");
		return *reinterpret_cast<T const*>(&mData);

	}
private:
	DataStorage mData;
};

