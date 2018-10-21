#pragma once

/*
Не поддерживает big-endian платформы...
комплить с осторожностью
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
	Стоит заметить что дання функция захватывает лишние данные если её контейнер
	больше чем содержимое.
	то-есть если sizeof(void*) == 8 а sizeof(T) == 4 то
	захваться лишние данные что лежат за value... в сетеке или на куче.
	Однако на резултат возварта это не повлияет так-как запросят этот-же тип и данные
	обрежутся. Эта функция быстрей чем та что сверху
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

