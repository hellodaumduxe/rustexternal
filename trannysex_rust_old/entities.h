#pragma once

class c_camera
{
public:
	uintptr_t view_matrix_address;
	uintptr_t visual_state_address;
};

class c_local_player
{
public:
	vec3 position;
	uintptr_t player_input;
	uintptr_t base_player;

	char active_item_name[24];
	char active_item_ammo_type[24];
	float active_item_bullet_velocity;
};

class c_npc
{
public:
	uintptr_t player_model;

	float health;
	float max_health;
	char health_buffer[24];

	uintptr_t head_address;
	uintptr_t position_address;
};

class c_entity
{
public:
	uintptr_t base_entity;
	uintptr_t player_model;

	float health;
	float max_health;
	char health_buffer[64];

	wchar_t name_buffer[64];
	uintptr_t head_address;
	uintptr_t spine1_address;
	uintptr_t spine2_address;
	uintptr_t spine3_address;
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
};

class c_sleeper
{
public:
	uintptr_t base_entity;

	vec3 location;
	wchar_t name_buffer[64];
};

namespace entities
{
	extern c_camera camera;
	extern c_local_player local_player;
	extern c_linked_list<c_npc> npcs;
	extern c_linked_list<c_entity> players;
	extern c_linked_list<c_sleeper> sleepers;
	extern bool is_updating;

	void handle_camera(uintptr_t game_object);
	void handle_entity(uintptr_t game_object, uintptr_t name);

	unsigned long __stdcall object_loop(void* param);
	void start();
}