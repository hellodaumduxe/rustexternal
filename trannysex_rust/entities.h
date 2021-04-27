#pragma once

class c_camera
{
public:
	uintptr_t view_matrix;
	uintptr_t visual_state;

	c_camera() {}
};

class c_local_player
{
public:
	uintptr_t player_input;
	uintptr_t base_player;
	uintptr_t player_model;

	vec3 position;
	c_player_input input;

	c_local_player() {}
};


class c_entity
{
public:
	uintptr_t base_entity;
	uintptr_t player_model;
	wchar_t name_buffer[64];

	float health_percentage;
	char health_buffer[64];

	uintptr_t head_address;
	uintptr_t spine1_address;
	uintptr_t spine2_address;
	uintptr_t spine4_address;
	uintptr_t l_clavicle_address;
	uintptr_t l_upperarm_address;
	uintptr_t l_forearm_address;
	uintptr_t l_hand_address;
	uintptr_t r_clavicle_address;
	uintptr_t r_upperarm_address;
	uintptr_t r_forearm_address;
	uintptr_t r_hand_address;
	uintptr_t l_hip_address;
	uintptr_t l_knee_address;
	uintptr_t l_foot_address;
	uintptr_t r_hip_address;
	uintptr_t r_knee_address;
	uintptr_t r_foot_address;

	c_entity() {}
};

class c_npc
{
public:
	uintptr_t player_model;

	float health_percentage;
	char health_buffer[64];

	c_npc() {}
};

class c_sleeper
{
public:
	uintptr_t base_entity;

	vec3 location;
	wchar_t name_buffer[64];

	c_sleeper() {}
};

class c_tc
{
public:
	vec3 b1;
	vec3 b2;
	vec3 b3;
	vec3 b4;
	vec3 t1;
	vec3 t2;
	vec3 t3;
	vec3 t4;

	c_tc() {}
};

#define validate_entry(l, e) \
for (auto entry = ##l##.list->next_entry; entry != nullptr; entry = entry->next_entry) \
	if (entry->_item.##e## == e) return;

namespace entities
{
	extern c_camera camera;
	extern c_local_player local_player;
	extern c_linked_list<c_entity> players;
	extern c_linked_list<c_sleeper> sleepers;
	extern c_linked_list<c_npc> npcs;
	extern c_linked_list<c_tc> tcs;
	extern bool is_updating;

	void handle_entity(uintptr_t object, uintptr_t name);

	extern bool loaded;
	void load();
	void unload();

	unsigned long __stdcall loop(void* param);
	void update_local_data();

	void on_render();
	void run_loop();
	void init();
};