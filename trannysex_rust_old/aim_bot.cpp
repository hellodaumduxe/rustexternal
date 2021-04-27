#include "stdafx.h"

c_player_input aim_bot::input;
vec3 aim_bot::camera_position;

#define aim_bot_fov 12.0f

c_aimbot_target aim_bot::aimbot_target;
void aim_bot::find_target()
{
	aim_bot::aimbot_target.found = false;

	if (entities::is_updating == true)
		return;

	float best_fov = FLT_MAX;

	entities::players.lock();
	for (auto entry = entities::players.list->next_entry; entry != nullptr; entry = entry->next_entry)
	{
		vec3 head = math::get_bone_pos(entry->_item.head_address);
		vec3 head_angles = math::calc_angle(camera_position, head).normalize();

		float fov = math::calc_fov(*aim_bot::input.get_view(), head_angles);
		if (
			(fov <= best_fov) &&
			(fov <= aim_bot_fov)
			)
		{
			aim_bot::aimbot_target.found = true;
			aim_bot::aimbot_target.head = head;
			aim_bot::aimbot_target.velocity = memory::read<vec3>(entry->_item.player_model + player_model::velocity);
			best_fov = fov;
		}
	}
	entities::players.unlock();

	/*entities::npcs.lock();
	for (auto entry = entities::npcs.list->next_entry; entry != nullptr; entry = entry->next_entry)
	{
		vec3 head = math::get_bone_pos(entry->_item.head_address);
		vec3 head_angles = math::calc_angle(camera_position, head).normalize();

		float fov = math::calc_fov(*aim_bot::input.get_view(), head_angles);
		if (
			(fov <= best_fov) &&
			(fov < aim_bot_fov)
			)
		{
			aim_bot::aimbot_target.found = true;
			aim_bot::aimbot_target.head = head;
			aim_bot::aimbot_target.velocity = memory::read<vec3>(entry->_item.player_model + player_model::velocity);
		}
	}
	entities::npcs.unlock();*/
}

#define ammo_rifle_hash 0x82f85c36
#define ammo_rifle_explosive_hash 0x1dec9de8
#define ammo_rifle_hv_hash 0x79847e62
#define ammo_rifle_incendiary_hash 0xecc8d8b3
#define ammo_pistol_hash 0x1efed5a8
#define ammo_pistol_hv_hash 0x20fade2
#define ammo_pistol_fire_hash 0x1226f1ea
#define ammo_handmade_shell_hash 0xd6afb7d6
#define ammo_shotgun_hash 0x4bda55dc
#define ammo_shotgun_fire_hash 0x98262f4b
#define ammo_shotgun_slug_hash 0x6f0eb7ad
#define arrow_wooden_hash 0xe1f1eb8
#define arrow_hv_hash 0x333e8b71
#define arrow_fire_hash 0x840ff412
#define arrow_bone_hash 0x42dca5c8
#define ammo_nailgun_nails_hash 0x2f063ffa
float aim_bot::get_bullet_speed(int name_hash)
{
	float bullet_speed = 9999.0f;
	switch (name_hash)
	{
	case ammo_rifle_hash:
		bullet_speed = 375.0f;
		break;
	case ammo_rifle_explosive_hash:
		bullet_speed = 196.0f;
		break;
	case ammo_rifle_hv_hash:
		bullet_speed = 450.0f;
		break;
	case ammo_rifle_incendiary_hash:
		bullet_speed = 211.0f;
		break;
	case ammo_pistol_hash:
		bullet_speed = 300.0f;
		break;
	case ammo_pistol_hv_hash:
		bullet_speed = 400.0f;
		break;
	case ammo_pistol_fire_hash:
		bullet_speed = 225.0f;
		break;
	case ammo_handmade_shell_hash:
		bullet_speed = 107.042f;
		break;
	case ammo_shotgun_hash:
		bullet_speed = 180.1635f;
		break;
	case ammo_shotgun_fire_hash:
		bullet_speed = 105.9409f;
		break;
	case ammo_shotgun_slug_hash:
		bullet_speed = 225.0f;
		break;
	case arrow_wooden_hash:
		bullet_speed = 50.0f;
		break;
	case arrow_hv_hash:
		bullet_speed = 80.0f;
		break;
	case arrow_fire_hash:
		bullet_speed = 40.0f;
		break;
	case arrow_bone_hash:
		bullet_speed = 45.0f;
		break;
	case ammo_nailgun_nails_hash:
		bullet_speed = 59.7f;
		break;
	}

	return bullet_speed;
}

unsigned long __stdcall aim_bot::aim_bot_loop(void* param)
{
	while (true)
	{
		memory::read_buffer(entities::local_player.player_input, &aim_bot::input, sizeof(aim_bot::input));
		aim_bot::camera_position = memory::read<vec3>(entities::camera.visual_state_address + visual_state::camera_position);
		aim_bot::input.get_view()->normalize();

		aim_bot::find_target();

		if (GetAsyncKeyState(VK_XBUTTON2) && aim_bot::aimbot_target.found)
		{
			vec3 target_head = aim_bot::aimbot_target.head;
			vec3 new_velocity = aim_bot::aimbot_target.velocity;

			float distance = camera_position.distance(target_head);
			int name_hash = macro::hash(entities::local_player.active_item_ammo_type);

			float gravity = 5.20f;
			float bullet_speed = aim_bot::get_bullet_speed(name_hash);
			bullet_speed *= entities::local_player.active_item_bullet_velocity;

			switch (name_hash)
			{
			case arrow_wooden_hash:
			case arrow_hv_hash:
			case arrow_fire_hash:
			case arrow_bone_hash:
			{
				gravity = 3.64f;
				if (distance > 30.0f)
					new_velocity *= 0.85f;
				else
					new_velocity *= 0.72f;

				if ((entities::local_player.active_item_bullet_velocity == 1.0f) && (distance > 50.0f))
					new_velocity *= 1.21f;
			}
			break;

			case ammo_rifle_hash:
			case ammo_rifle_explosive_hash:
			case ammo_rifle_hv_hash:
			case ammo_rifle_incendiary_hash:
				new_velocity *= 0.78f;
				break;

			case ammo_nailgun_nails_hash:
				new_velocity *= 0.95f;
				break;

			default:
				new_velocity *= 0.87f;
				break;
			}

			float bullet_time = (distance / bullet_speed);
			new_velocity *= bullet_time;
			target_head += new_velocity;
			target_head.y += (gravity * (bullet_time * bullet_time));

			vec3 aim_angles = math::calc_angle(camera_position, target_head).normalize();
			aim_angles -= *aim_bot::input.get_recoil();
			if (
				(!math::is_nan(aim_angles.x) && !math::is_nan(aim_angles.y) && !math::is_nan(aim_angles.z)) &&
				(math::calc_fov(*aim_bot::input.get_view(), aim_angles) <= aim_bot_fov)
				)
			{
				aim_angles.normalize();
				aim_angles.clamp();
				memory::write<vec3>(entities::local_player.player_input + player_input::view_angles, aim_angles);
			}
		}

		Sleep(25);
	}

	return 0;
}

void aim_bot::on_render()
{
	if (aim_bot::aimbot_target.found)
	{
		vec3 aimbot_location_world;
		if (visuals::world_to_screen(aim_bot::aimbot_target.head, &aimbot_location_world))
			drawing::draw_line(static_cast<int>(overlay::screen_center.x), overlay::height, static_cast<int>(aimbot_location_world.x), static_cast<int>(aimbot_location_world.y), c_pink);
	}

	drawing::draw_circle(static_cast<int>(overlay::screen_center.x), static_cast<int>(overlay::screen_center.y), (aim_bot_fov * 8.0f), c_pink);
}

void aim_bot::start()
{
	HANDLE thread = CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(aim_bot::aim_bot_loop), nullptr, 0, 0);
	SetThreadPriority(thread, THREAD_PRIORITY_BELOW_NORMAL);
}