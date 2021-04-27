#pragma once

template<typename type>
class vector
{
	size_t index;
	type* list;
public:

	inline void push_back(const type o) { this->list[this->index++] = o; }
	inline size_t size() { return this->index; }
	inline void clear() { this->index = 0; }
	inline type& operator[](size_t id) { return this->list[id]; }

	inline void allocate(const size_t size) { this->index = 0; this->list = reinterpret_cast<type*>(memory::heap_alloc(size * sizeof(type))); }
	vector() {}
};