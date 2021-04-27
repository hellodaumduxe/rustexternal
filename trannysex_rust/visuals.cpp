#include "stdafx.h"

vector<vec3> skeleton_points;
void visuals::draw_skeleton(c_entity* entity, d3d_color color)
{
	skeleton_points.clear();
	vec3 head = util::get_bone_position(entity->head_address);
	vec3 spine1 = util::get_bone_position(entity->spine1_address);
	vec3 spine2 = util::get_bone_position(entity->spine2_address);
	vec3 spine4 = util::get_bone_position(entity->spine4_address);
	vec3 l_clavicle = util::get_bone_position(entity->l_clavicle_address);
	vec3 l_upperarm = util::get_bone_position(entity->l_upperarm_address);
	vec3 l_forearm = util::get_bone_position(entity->l_forearm_address);
	vec3 l_hand = util::get_bone_position(entity->l_hand_address);
	vec3 r_clavicle = util::get_bone_position(entity->r_clavicle_address);
	vec3 r_upperarm = util::get_bone_position(entity->r_upperarm_address);
	vec3 r_forearm = util::get_bone_position(entity->r_forearm_address);
	vec3 r_hand = util::get_bone_position(entity->r_hand_address);
	vec3 l_hip = util::get_bone_position(entity->l_hip_address);
	vec3 l_knee = util::get_bone_position(entity->l_knee_address);
	vec3 l_foot = util::get_bone_position(entity->l_foot_address);
	vec3 r_hip = util::get_bone_position(entity->r_hip_address);
	vec3 r_knee = util::get_bone_position(entity->r_knee_address);
	vec3 r_foot = util::get_bone_position(entity->r_foot_address);

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
	drawing::draw_3d_line_from_array(&skeleton_points, color);
}

void visuals::draw_radar()
{
	/*float x = 220;
	float y = 120;
	float radius = 80.0f;
	float range = (static_cast<float>(settings::o.visuals.radar_distance) * 40.0f);
	drawing::draw_filled_circle(x, y, 50.0f, radius, d3d_color(0, 0, 0, 160));

	float view_y = entities::local_player.input.get_view()->y;
	auto draw_shit = [=](float d, float a, d3d_color color)
	{
		float angle = a;
		if (angle < 0.0f || angle > 360.0f)
			return;

		angle -= (angle > 360.0f) ? 360.0f : 0.0f;
		angle += (angle < 0.0f) ? 360.0f : 0.0f;
		angle = 360.0f - (angle - view_y);
		angle -= (angle > 360.0f) ? 360.0f : 0.0f;
		angle += (angle < 0.0f) ? 360.0f : 0.0f;
		if (angle < 0.0f || angle > 360.0f)
			return;

		float angle_rad = deg_to_rad(angle);
		math::clamp(angle, 0.0f, 360.0f);
		float c = rad_to_deg(math::cos(angle_rad));
		float s = rad_to_deg(math::sin(angle_rad));

		float circle_x = x + (d * s);
		float circle_y = y +(d * c);
		drawing::draw_filled_circle(circle_x, circle_y, 12.0f, 2.3f, color);
		drawing::draw_circle(circle_x, circle_y, 12.0f, 2.3f, c_black);
	};

	entities::players.lock();
	for (auto entry = entities::players.list->next_entry; entry != nullptr; entry = entry->next_entry)
	{
		c_entity* player = &entry->_item;
		vec3 position = memory::read<vec3>(player->player_model + player_model::position);

		float distance = (position.distance_2d(entities::local_player.position) / range);
		if (distance >= ((radius - 2.0f) * m_pi_rad))
			continue;

		float angle = util::calc_angle(&entities::local_player.position, &position).normalize().y + 180.0f;
		angle -= (angle > 360.0f) ? 360.0f : 0.0f;
		angle += (angle < 0.0f) ? 360.0f : 0.0f;
		draw_shit(distance, angle, c_blue);
	}
	entities::players.unlock();

	drawing::draw_circle(x, y, 50.0f, radius, c_pink);
	drawing::draw_triangle(x, y, 40, c_pink);*/
}

inline void draw_player_esp(c_entity* player)
{
	if ((player->player_model == 0) || (player->base_entity == 0))
		return;

	vec3 head_world = util::get_bone_position(player->head_address);
	vec3 origin_world = memory::read<vec3>(player->player_model + player_model::position);
	float distance = entities::local_player.position.distance(origin_world);

	vec3 head, origin;
	if (!drawing::world_to_screen(head_world, &head) || !drawing::world_to_screen(origin_world, &origin))
		return;

	float h = (origin.y - head.y) * 1.4f;
	float w = h * 0.6f;
	float x = head.x - (w / 2.0f);
	float y = head.y - (h * 0.15f);

	if (settings::o.visuals.players.skeleton && (distance < 175.0f))
		visuals::draw_skeleton(player, c_white);
	drawing::draw_esp_box(x, y, w, h, c_green, c_black);

	if (settings::o.visuals.players.health && (distance < 350.0f))
	{
		uint8_t green = static_cast<uint8_t>(player->health_percentage * 255.0f);
		uint8_t red = (255 - green);
		drawing::draw_rectangle(x - 1, y + h + 6, w + 3, 4, c_black);
		drawing::draw_rectangle(x, y + h + 7, (player->health_percentage * (w + 1.0f)), 2, d3d_color(red, green, 0, 255));
		holy_font::draw_string(holy_font::pixel_font, player->health_buffer, x, y + h + 10, w, flag_centered | flag_outline, c_white);
	}

	if (settings::o.visuals.players.name)
		holy_font::draw_wide_string(holy_font::arial_small, player->name_buffer, x, y - 12, w, flag_centered | flag_outline, c_white);
}

inline void draw_sleeper(c_sleeper* sleeper)
{
	vec3 origin;
	if (!drawing::world_to_screen(sleeper->location, &origin))
		return;

	holy_font::draw_string(holy_font::pixel_font, s("sleeper"), origin.x, origin.y, 0, flag_centered | flag_outline, c_yellow);
	holy_font::draw_wide_string(holy_font::arial_small, sleeper->name_buffer, origin.x, origin.y + 10, 0, flag_centered | flag_outline, c_white);
}

inline void draw_npc(c_npc* npc)
{
	vec3 origin_world = memory::read<vec3>(npc->player_model + player_model::position);
	vec3 head_world = origin_world + vec3{ 0.0f, 1.0f, 0.0f }; //used to align hp bar, doesnt need to be accurate

	vec3 origin, head;
	if (!drawing::world_to_screen(origin_world, &origin) || !drawing::world_to_screen(head_world, &head))
		return;

	float h = (origin.y - head.y) * 1.4f;
	float w = h * 0.6f;
	float x = head.x - (w / 2.0f);
	float y = head.y - (h * 0.15f);

	uint8_t green = static_cast<uint8_t>(npc->health_percentage * 255.0f);
	uint8_t red = (255 - green);
	drawing::draw_rectangle(x - 1, y + h + 6, w + 3, 4, c_black);
	drawing::draw_rectangle(x, y + h + 7, (npc->health_percentage * (w + 1.0f)), 2, d3d_color(red, green, 0, 255));
	holy_font::draw_string(holy_font::pixel_font, npc->health_buffer, x, y + h + 10, w, flag_centered | flag_outline, c_white);
}

vector<vec3> tc_points;
inline void draw_tc(c_tc* tc)
{
	tc_points.clear();
	tc_points.push_back(tc->t1);
	tc_points.push_back(tc->t2);
	tc_points.push_back(tc->b2);
	tc_points.push_back(tc->b1);
	tc_points.push_back(tc->t1);
	tc_points.push_back(tc->t3);
	tc_points.push_back(tc->b3);
	tc_points.push_back(tc->b1);
	tc_points.push_back(tc->b3);
	tc_points.push_back(tc->b4);
	tc_points.push_back(tc->t4);
	tc_points.push_back(tc->t3);
	tc_points.push_back(tc->t4);
	tc_points.push_back(tc->t2);
	tc_points.push_back(tc->b2);
	tc_points.push_back(tc->b4);
	drawing::draw_3d_line_from_array(&tc_points, c_pink);
}

void visuals::on_render()
{
	if (entities::is_updating)
		return;

	if (settings::o.visuals.npcs)
		for (auto entry = entities::npcs.list->next_entry; entry != nullptr; entry = entry->next_entry)
			draw_npc(&entry->_item);


	if (settings::o.visuals.sleepers)
		for (auto entry = entities::sleepers.list->next_entry; entry != nullptr; entry = entry->next_entry)
			draw_sleeper(&entry->_item);

	if (settings::o.visuals.players.enabled)
		for (auto entry = entities::players.list->next_entry; entry != nullptr; entry = entry->next_entry)
			draw_player_esp(&entry->_item);

	if (settings::o.visuals.tc)
		for (auto entry = entities::tcs.list->next_entry; entry != nullptr; entry = entry->next_entry)
			draw_tc(&entry->_item);

	if (settings::o.visuals.crosshair)
		drawing::draw_gamer_crosshair(overlay::screen_center.x, overlay::screen_center.y);

	if (settings::o.visuals.radar)
		visuals::draw_radar();
}

void visuals::draw_watermark()
{
	holy_font::draw_string(holy_font::pixel_font, s("trannysex"), 10, 11, 0, flag_outline, c_white);
	holy_font::draw_string(holy_font::pixel_font, s("rust"), 28, 11, 0, flag_outline, c_pink);
}

void visuals::init()
{
	skeleton_points.allocate(27);
	tc_points.allocate(17);
}