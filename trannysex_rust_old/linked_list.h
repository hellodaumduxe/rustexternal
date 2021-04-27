#pragma once

//credits: hades for being a template god
template<typename T>
class c_linked_list_entry
{
public:
	c_linked_list_entry* previous_entry;
	c_linked_list_entry* next_entry;
	T _item;
};

template<typename T>
class c_linked_list
{
public:
	c_linked_list_entry<T>* list;
	bool in_use;

	T* create_item()
	{
		c_linked_list_entry<T>* entry = this->list;
		while (entry->next_entry != nullptr)
			entry = entry->next_entry;

		void* allocation = HeapAlloc(memory::process_heap, 0, sizeof(c_linked_list_entry<T>));
		if (allocation == nullptr)
			return nullptr;

		c_linked_list_entry<T>* new_entry = reinterpret_cast<c_linked_list_entry<T>*>(allocation);
		new_entry->previous_entry = entry;
		new_entry->next_entry = nullptr;

		entry->next_entry = new_entry;
		return &new_entry->_item;
	}

	void remove_entry(c_linked_list_entry<T>* entry)
	{
		c_linked_list_entry<T>* previous_entry = entry->previous_entry;
		c_linked_list_entry<T>* next_entry = entry->next_entry;

		if (previous_entry)
			previous_entry->next_entry = next_entry;

		if (next_entry)
			next_entry->previous_entry = previous_entry;

		HeapFree(memory::process_heap, 0, entry);
	}

	void lock() { this->in_use = true; }
	void unlock() { this->in_use = false; }
	bool is_locked() { return this->in_use; }

	void copy(c_linked_list<T>* to_copy)
	{
		this->clear();
		for (auto entry = to_copy->list->next_entry; entry != nullptr; entry = entry->next_entry)
		{
			T* item = this->create_item();
			memcpy(item, &entry->_item, sizeof(T));
		}
	}

	void clear()
	{
		c_linked_list_entry<T>* entry = this->list->next_entry;
		if (entry == nullptr)
			return; //already empty

		this->list->next_entry = nullptr;
		while (true)
		{
			c_linked_list_entry<T>* next_entry = entry->next_entry;
			HeapFree(memory::process_heap, 0, entry);
			entry = next_entry;

			if (entry == nullptr)
				break;
		}
	}

	void free_list()
	{
		this->clear();
		HeapFree(memory::process_heap, 0, this->list);
	}

	void allocate_list()
	{
		void* allocation = HeapAlloc(memory::process_heap, 0, sizeof(c_linked_list_entry<T>));
		this->list = reinterpret_cast<c_linked_list_entry<T>*>(allocation);
		this->list->next_entry = nullptr;
		this->list->previous_entry = nullptr;

		this->in_use = false;
	}

	c_linked_list() {};
};