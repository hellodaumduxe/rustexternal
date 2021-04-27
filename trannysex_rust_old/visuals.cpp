#include "stdafx.h"

vector<vec3> skeleton_points;
inline void draw_skeleton(c_entity* entity, d3d_color color)
{
	skeleton_points.clear();
	vec3 head = math::get_bone_pos(entity->head_address);
	vec3 spine1 = math::get_bone_pos(entity->spine1_address);
	vec3 spine2 = math::get_bone_pos(entity->spine2_address);
	vec3 spine3 = math::get_bone_pos(entity->spine3_address);
	vec3 spine4 = math::get_bone_pos(entity->spine4_address);
	vec3 l_clavicle = math::get_bone_pos(entity->l_clavicle_address);
	vec3 l_upperarm = math::get_bone_pos(entity->l_upperarm_address);
	vec3 l_forearm = math::get_bone_pos(entity->l_forearm_address);
	vec3 l_hand = math::get_bone_pos(entity->l_hand_address);
	vec3 r_clavicle = math::get_bone_pos(entity->r_clavicle_address);
	vec3 r_upperarm = math::get_bone_pos(entity->r_upperarm_address);
	vec3 r_forearm = math::get_bone_pos(entity->r_forearm_address);
	vec3 r_hand = math::get_bone_pos(entity->r_hand_address);
	vec3 l_hip = math::get_bone_pos(entity->l_hip_address);
	vec3 l_knee = math::get_bone_pos(entity->l_knee_address);
	vec3 l_foot = math::get_bone_pos(entity->l_foot_address);
	vec3 r_hip = math::get_bone_pos(entity->r_hip_address);
	vec3 r_knee = math::get_bone_pos(entity->r_knee_address);
	vec3 r_foot = math::get_bone_pos(entity->r_foot_address);

	skeleton_points.push_back(l_foot);
	skeleton_points.push_back(l_knee);
	skeleton_points.push_back(l_hip);
	skeleton_points.push_back(spine1);
	skeleton_points.push_back(r_hip);
	skeleton_points.push_back(r_knee);
	skeleton_points.push_back(r_foot);
	skeleton_points.push_back(r_knee);
	skeleton_points.push_back(r_hip);
	skeleton_points.push_back(spine1);
	skeleton_points.push_back(spine2);
	skeleton_points.push_back(spine3);
	skeleton_points.push_back(spine4);
	skeleton_points.push_back(head);
	skeleton_points.push_back(spine4);
	skeleton_points.push_back(l_clavicle);
	skeleton_points.push_back(l_upperarm);
	skeleton_points.push_back(l_forearm);
	skeleton_points.push_back(l_hand);
	skeleton_points.push_back(l_forearm);
	skeleton_points.push_back(l_upperarm);
	skeleton_points.push_back(l_clavicle);
	skeleton_points.push_back(spine4);
	skeleton_points.push_back(r_clavicle);
	skeleton_points.push_back(r_upperarm);
	skeleton_points.push_back(r_forearm);
	skeleton_points.push_back(r_hand);
	drawing::draw_3d_line_from_array(skeleton_points, color);
};

inline void draw_player_esp(vec3 head, vec3 origin, float distance, c_entity* player)
{
	//main box
	float height = origin.y - head.y;
	height *= 1.4f;

	float width = height;
	width *= 0.6f;

	int w = static_cast<int>(width);
	int h = static_cast<int>(height);

	int x = static_cast<int>(head.x - (width / 2.0f));
	int y = static_cast<int>(head.y - (height * 0.15f));

	bool should_draw_misc = (distance < 275.0f);
	bool should_draw_skeleton = (distance < 100.0f);

	//skeleton
	if (should_draw_skeleton)
		draw_skeleton(player, c_white);

	//name
	if (should_draw_misc)
		fonts::pixel_font.draw_wide_string(player->name_buffer, x, y - 12, w, flag_centered | flag_outline, c_white);

	drawing::draw_outlined_hollow_rectangle(x, y, w, h, c_green, c_black);

	//hp bar
	float hp_bar_w = width + 1.0f;
	float percentage = min(1.0f, (player->health / player->max_health));

	d3d_color green = static_cast<d3d_color>(percentage * 255.0f);
	d3d_color red = 255 - green;

	drawing::draw_rectangle(x - 1, y + h + 6, w + 3, 4, c_black);
	drawing::draw_rectangle(x, y + h + 7, static_cast<int>(percentage * hp_bar_w), 2, d3d_color_argb(255, red, green, 0));

	//hp number
	fonts::pixel_font.draw_string(player->health_buffer, x, y + h + 10, w, flag_centered | flag_outline, c_white);
}

inline void draw_npc_esp(vec3 head, vec3 origin, c_npc* npc)
{
	//main box
	float height = origin.y - head.y;
	height *= 1.4f;

	float width = height;
	width *= 0.6f;

	int w = static_cast<int>(width);
	int h = static_cast<int>(height);

	int x = static_cast<int>(head.x - (width / 2.0f));
	int y = static_cast<int>(head.y - (height * 0.15f));

	//hp bar
	float hp_bar_w = width + 1.0f;
	float percentage = min(1.0f, (npc->health / npc->max_health));

	d3d_color green = static_cast<d3d_color>(percentage * 255.0f);
	d3d_color red = 255 - green;

	drawing::draw_rectangle(x - 1, y + h + 6, w + 3, 4, c_black);
	drawing::draw_rectangle(x, y + h + 7, static_cast<int>(percentage * hp_bar_w), 2, d3d_color_argb(255, red, green, 0));

	//hp number
	fonts::pixel_font.draw_string(npc->health_buffer, x + 1, y + h + 10, w, flag_centered | flag_outline, c_white);
}

D3DXMATRIX visuals::view_matrix;
void visuals::on_render()
{
	memory::read_buffer(entities::camera.view_matrix_address, &visuals::view_matrix, sizeof(visuals::view_matrix));
	if (entities::is_updating == true)
		return;

	entities::sleepers.lock();
	for (auto entry = entities::sleepers.list->next_entry; entry != nullptr; entry = entry->next_entry)
	{
		vec3 location_world;
		if (visuals::world_to_screen(entry->_item.location, &location_world))
		{
			fonts::pixel_font.draw_string(s("sleeper"), static_cast<int>(location_world.x), static_cast<int>(location_world.y), 0, flag_outline | flag_centered, c_yellow);
			fonts::pixel_font.draw_wide_string(entry->_item.name_buffer, static_cast<int>(location_world.x), static_cast<int>(location_world.y + 8), 0, flag_outline | flag_centered, c_yellow);
		}
	}
	entities::sleepers.unlock();

	entities::npcs.lock();
	for (auto entry = entities::npcs.list->next_entry; entry != nullptr; entry = entry->next_entry)
	{
		vec3 head = math::get_bone_pos(entry->_item.head_address);
		vec3 origin = memory::read<vec3>(entry->_item.position_address);

		vec3 head_world, origin_world;
		if (
			!visuals::world_to_screen(head, &head_world) ||
			!visuals::world_to_screen(origin, &origin_world)
			)
			continue;

		draw_npc_esp(head_world, origin_world, &entry->_item);
	}
	entities::npcs.unlock();

	entities::players.lock();
	for (auto entry = entities::players.list->next_entry; entry != nullptr; entry = entry->next_entry)
	{
		vec3 head = math::get_bone_pos(entry->_item.head_address);
		vec3 origin = memory::read<vec3>(entry->_item.player_model + player_model::position);
		float distance = entities::local_player.position.distance(origin);

		vec3 head_world, origin_world;
		if (
			!visuals::world_to_screen(head, &head_world) ||
			!visuals::world_to_screen(origin, &origin_world)
			)
			continue;

		draw_player_esp(head_world, origin_world, distance, &entry->_item);
	}
	entities::players.unlock();

	drawing::draw_outlined_crosshair(static_cast<int>(overlay::screen_center.x), static_cast<int>(overlay::screen_center.y), 4, c_pink, c_black);
}