#pragma once

template<typename type>
class vector
{
	size_t index;
	type list[500];
public:

	__forceinline size_t size()
	{
		return this->index;
	}

	__forceinline void push_back(type obj)
	{
		this->list[this->index] = obj;
		this->index++;
	}

	__forceinline void clear()
	{
		this->index = 0;
	}

	__forceinline type operator[](int id)
	{
		return this->list[id];
	}
};