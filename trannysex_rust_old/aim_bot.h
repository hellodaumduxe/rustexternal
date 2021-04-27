#pragma once

class c_aimbot_target
{
public:
	bool found;
	vec3 head;
	vec3 velocity;
};

namespace aim_bot
{
	extern c_player_input input;
	extern vec3 camera_position;

	extern c_aimbot_target aimbot_target;
	void find_target();
	float get_bullet_speed(int name_hash);

	unsigned long __stdcall aim_bot_loop(void* param);

	void on_render();
	void start();
};