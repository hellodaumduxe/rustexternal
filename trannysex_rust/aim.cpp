#include "stdafx.h"

c_aim_ctx aim::aim_ctx;

/*bool aim::calculate_predicted_pos(const vec3& vecTargetPosition, const vec3& vecTargetVelocity, const vec3& vecSourcePosition, const float flBulletSpeed, vec3* pPositionOut, float* pTimeOut) {

	float flVelocityX = vecTargetVelocity.x;
	float flVelocityY = vecTargetVelocity.y;
	float flVelocityZ = vecTargetVelocity.z;

	float flTargetSpeed = math::sqrt((flVelocityX * flVelocityX) + (flVelocityY * flVelocityY) + (flVelocityZ * flVelocityZ));

	if (0.0078125f > flTargetSpeed) {
		*pPositionOut = vecTargetPosition;

		float flDeltaX = vecTargetPosition.x - vecSourcePosition.x;
		float flDeltaY = vecTargetPosition.y - vecSourcePosition.y;
		float flDeltaZ = vecTargetPosition.z - vecSourcePosition.z;

		float flDistance = math::sqrt((flDeltaX * flDeltaX) + (flDeltaY * flDeltaY) + (flDeltaZ * flDeltaZ));

		*pTimeOut = flDistance / flBulletSpeed;

		return true;
	}

	float flA = (flVelocityX * flVelocityX) + (flVelocityY * flVelocityY) + (flVelocityZ * flVelocityZ) - (flBulletSpeed * flBulletSpeed);

	if (0.f == flA) {
		return false;
	}

	float flStartX = vecTargetPosition.x;
	float flStartY = vecTargetPosition.y;
	float flStartZ = vecTargetPosition.z;

	float flSourceX = vecSourcePosition.x;
	float flSourceY = vecSourcePosition.y;
	float flSourceZ = vecSourcePosition.z;

	float flB = 2.f * (flVelocityX * (flStartX - flSourceX) + flVelocityY * (flStartY - flSourceY) + flVelocityZ * (flStartZ - flSourceZ));
	float flC = (flStartX * flStartX) + (flStartY * flSourceY) + (flStartZ * flStartZ) + (flSourceX * flSourceX) + (flSourceY * flSourceY) + (flSourceZ * flSourceZ) - (2.f * flSourceX * flStartX) - (2.f * flSourceY * flStartY) - (2.f * flSourceZ * flStartZ);
	float flDiscriminant = (flB * flB) - (4.f * flA * flC);

	if (flDiscriminant < 0.f) {
		return false;
	}

	float flDiscriminantSqrt = math::sqrt(flDiscriminant);

	float flTravelTime1 = (-flB + flDiscriminantSqrt) / (2.f * flA);
	float flTravelTime2 = (-flB - flDiscriminantSqrt) / (2.f * flA);

	float flTravelTime = min(flTravelTime1, flTravelTime2);
	if (flTravelTime < 0.f) {
		flTravelTime = max(flTravelTime1, flTravelTime2);
		if (flTravelTime < 0.f) {
			return false;
		}
	}

	float flDirectionX = vecTargetVelocity.x / flTargetSpeed;
	float flDirectionY = vecTargetVelocity.y / flTargetSpeed;
	float flDirectionZ = vecTargetVelocity.z / flTargetSpeed;

	*pTimeOut = flTravelTime;

	flTravelTime *= flTravelTime;
	*pPositionOut = vec3(flStartX + (flDirectionX * flTravelTime), flStartY + (flDirectionY * flTravelTime), flStartZ + (flDirectionZ * flTravelTime));
	return true;
}*/


void aim::find_target()
{
	aim::aim_ctx.found = false;
	if (entities::is_updating)
		return;

	c_bullet bullet = aim::get_bullet();
	bullet.update(aim::aim_ctx.item_velocity_scale);

	aim::aim_ctx.camera = memory::read<vec3>(entities::camera.visual_state + visual_state::camera_position);

	float best_fov = FLT_MAX;
	for (auto entry = entities::players.list->next_entry; entry != nullptr; entry = entry->next_entry)
	{
		if (memory::read<int>(entry->_item.base_entity + base_player::life_state) != life_states::alive) continue;
		vec3 head = util::get_bone_position(entry->_item.head_address);

		vec3 velocity = memory::read<vec3>(entry->_item.player_model + player_model::velocity);

		float distance = aim::aim_ctx.camera.distance(head);
		float bullet_time = aim::calc_bullet_time(distance, bullet);

		//aim::comp_for_movement(aim::aim_ctx.camera, head, velocity, bullet.velocity, bullet_time);
		aim::comp_for_drop(aim::aim_ctx.camera, head, bullet_time, bullet);

		vec3 head_angles = util::calc_angle(&aim::aim_ctx.camera, &head).normalize();
		float fov = util::calc_fov(entities::local_player.input.get_view(), &head_angles);
		if ((fov <= best_fov) && (fov <= static_cast<float>(settings::o.aim.fov)))
		{
			aim::aim_ctx.found = true;
			aim::aim_ctx.position = head;
			//aim::aim_ctx.position = vecAimPosition;
			//aim::aim_ctx.velocity = vecVelocity;
			best_fov = fov;
		}
	}
}

c_bullet aim::get_bullet()
{
	switch (macro::hash(aim::aim_ctx.item_ammo_type))
	{
	case ammo_rifle_hash:
		return c_bullet(375.0f, 0.6f, 1.0f);
	case ammo_rifle_hv_hash:
		return c_bullet(450.0f, 0.6f, 1.0f);
	case ammo_rifle_incendiary_hash:
		return c_bullet(225.0f, 0.6f, 1.0f);
	case ammo_rifle_explosive_hash:
		return c_bullet(225.0f, 0.6f, 1.2f);

	case ammo_pistol_hash:
		return c_bullet(300.0f, 0.7f, 1.0f);
	case ammo_pistol_hv_hash:
		return c_bullet(400.0f, 0.7f, 1.0f);
	case ammo_pistol_fire_hash:
		return c_bullet(225.0f, 0.7f, 1.0f);

	case arrow_wooden_hash:
		return c_bullet(50.0f, 0.0f, 0.7f);
	case arrow_hv_hash:
		return c_bullet(80.0f, 0.0f, 0.5f);
	case arrow_bone_hash:
		return c_bullet(45.0f, 0.0f, 0.7f);
	case arrow_fire_hash:
		return c_bullet(40.0f, 0.0f, 1.0f);

	case ammo_nailgun_nails_hash:
		return c_bullet(50.0f, 0.0f, 0.7f);

	case ammo_shotgun_hash:
		return c_bullet(50.0f, 0.0f, 0.0f);
	case ammo_shotgun_fire_hash:
		return c_bullet(93.2f, 1.0f, 1.0f);
	case ammo_handmade_shell_hash:
		return c_bullet(98.4f, 1.0f, 1.0f);
	case ammo_shotgun_slug_hash:
		return c_bullet(225.0f, 1.0f, 1.0f);
	}
	return c_bullet(9999.0f, 0.0f, 1.0f);
}

float aim::calc_bullet_time(const float d, const c_bullet& b)
{
	float time = 0.0f;
	float step = (5 / d);
	for (float i = step; i <= d; i += step)
	{
		float curr_speed = b.velocity;
		if (b.drag > 0.01f)
			curr_speed -= (b.drag * time);

		time += (step / curr_speed);
	}
	return time;
}

void aim::comp_for_drop(vec3 c, vec3& p, const float t, const c_bullet& b)
{
	const float flDeltaTime = 1.0 / 1000.0f;
	const float flGravityBase = -(9.81f * 0.5f);
	const float flGravityModifier = b.gravity_mod;
	const float flGravity = flGravityBase * flGravityModifier;
	const float flGravityStep = flGravity * flDeltaTime;
	const float flDrag = 0.6f;
	const float flDragStep = flDrag * flDeltaTime;

	const float flDeltaX = p.x - c.x;
	const float flDeltaY = p.y - c.y;
	const float flDeltaZ = p.z - c.z;

	const float flDistance = math::sqrt((flDeltaX * flDeltaX) + (flDeltaY * flDeltaY) + (flDeltaZ * flDeltaZ));
	const float flDistance2D = math::sqrt((flDeltaX * flDeltaX) + (flDeltaZ * flDeltaZ));

	const float flAimDirX = flDeltaX / flDistance;
	const float flAimDirY = flDeltaY / flDistance;
	const float flAimDirZ = flDeltaZ / flDistance;

	float flCurrentX = c.x;
	float flCurrentY = c.y;
	float flCurrentZ = c.z;

	float flVelocityX = flAimDirX * b.velocity;
	float flVelocityY = flAimDirY * b.velocity;
	float flVelocityZ = flAimDirZ * b.velocity;

	float flElapsedTime = 0.f;
	float flElapsedDistance2D = 0.f;

	while (flElapsedDistance2D < flDistance2D) {
		flVelocityY += flGravityStep;

		flVelocityX -= flVelocityX * flDragStep;
		flVelocityY -= flVelocityY * flDragStep;
		flVelocityZ -= flVelocityZ * flDragStep;

		const float flMoveX = flVelocityX * flDeltaTime;
		const float flMoveY = flVelocityY * flDeltaTime;
		const float flMoveZ = flVelocityZ * flDeltaTime;

		const float flMoveDistance2D = math::sqrt((flMoveX * flMoveX) + (flMoveZ * flMoveZ));

		flCurrentX += flMoveX;
		flCurrentY += flMoveY;
		flCurrentZ += flMoveZ;

		flElapsedDistance2D += flMoveDistance2D;
		flElapsedTime += flDeltaTime;
	}
	p.y += (p.y - flCurrentY);
}
	/*const float delta_time = (5 / t);
	const float gravity = (9.81f * 0.5f) * b.gravity_mod;
	const float gravity_step = gravity * delta_time;
	const float drag_step = b.drag * delta_time;

	vec3 delta = (p - c);
	float distance = delta.length();
	float distance_2d = delta.length_2d();
	vec3 velocity = (delta / distance) * b.velocity;

	float elapsed_distance = 0.0f;
	vec3 current = c;
	do
	{
		velocity.y += gravity_step;
		velocity -= (velocity * drag_step);

		vec3 move = velocity * delta_time;
		current += move;

		elapsed_distance += move.length_2d();
	} while (elapsed_distance < distance_2d);
	p.y += (p.y - current.y);*/

	/*float gravity = (9.81f * 0.5f) * m;
	float total_drop = gravity * math::square(t);
	p.y += total_drop;*/

void aim::update_item_information()
{
	aim::aim_ctx.updated = false;
	uintptr_t inventory = memory::read<uintptr_t>(entities::local_player.base_player + base_player::player_inventory);
	pointer_validity_check(inventory);
	uint32_t active_item_id = memory::read<uint32_t>(entities::local_player.base_player + base_player::active_item);

	uintptr_t belt = memory::read<uintptr_t>(inventory + 0x28);
	pointer_validity_check(belt);
	uintptr_t item_list = memory::read<uintptr_t>(belt + 0x38);
	pointer_validity_check(item_list);
	uintptr_t items = memory::read<uintptr_t>(item_list + 0x10);
	pointer_validity_check(items);
	for (int i = 0; i <= 6; i++)
	{
		uintptr_t item = memory::read<uintptr_t>(items + 0x20 + (i * 8));
		if (!macro::is_valid_address(item)) continue;

		uint32_t item_id = memory::read<uint32_t>(item + item::id);
		if (active_item_id != item_id) continue;

		uintptr_t item_definition = memory::read<uintptr_t>(item + item::definition);
		uintptr_t base_projectile = memory::read<uintptr_t>(item + item::projectile);
		if (!macro::is_valid_address(base_projectile) || !macro::is_valid_address(item_definition)) continue;

		auto update_projectile_name = [=](uintptr_t item_definition, uintptr_t base_projectile)
		{
			memset(aim::aim_ctx.item_name, 0, sizeof(aim::aim_ctx.item_name));
			memset(aim::aim_ctx.item_ammo_type, 0, sizeof(aim::aim_ctx.item_ammo_type));

			uintptr_t primary_magazine = memory::read<uintptr_t>(base_projectile + base_projectile::primary_magazine);
			pointer_validity_check(primary_magazine);
			uintptr_t ammo_definition = memory::read<uintptr_t>(primary_magazine + magazine::ammo_definition);
			pointer_validity_check(ammo_definition);

			uintptr_t name_pointer = memory::read<uintptr_t>(item_definition + item_definition::name);
			pointer_validity_check(name_pointer);
			uintptr_t ammo_name_pointer = memory::read<uintptr_t>(ammo_definition + item_definition::name);
			pointer_validity_check(ammo_name_pointer);

			wchar_t wide_item_name[32];
			wchar_t wide_ammo_name[32];
			memory::read_buffer(name_pointer + 0x14, &wide_item_name, sizeof(wide_item_name));
			memory::read_buffer(ammo_name_pointer + 0x14, &wide_ammo_name, sizeof(wide_ammo_name));
			WideCharToMultiByte(CP_UTF8, 0, wide_item_name, sizeof(wide_item_name), aim::aim_ctx.item_name, 32, 0, 0);
			WideCharToMultiByte(CP_UTF8, 0, wide_ammo_name, sizeof(wide_item_name), aim::aim_ctx.item_ammo_type, 32, 0, 0);
			aim::aim_ctx.updated = true;
		};

		update_projectile_name(item_definition, base_projectile);
		aim::aim_ctx.item_velocity_scale = memory::read<float>(base_projectile + base_projectile::velocity_scale);

		if (settings::o.misc.no_spread)
		{
			memory::write<float>(base_projectile + base_projectile::aim_sway, 0.0f);
			memory::write<float>(base_projectile + base_projectile::aim_cone, 0.0f);
			memory::write<float>(base_projectile + base_projectile::hip_aim_cone, 0.0f);
			memory::write<float>(base_projectile + base_projectile::stance_penalty_scale, 0.0f);
		}

		uintptr_t recoil = memory::read<uintptr_t>(base_projectile + base_projectile::recoil);
		if (settings::o.misc.no_recoil && macro::is_valid_address(recoil))
		{
			memory::write<float>(recoil + recoil_properties::recoil_yaw_min, 0.0f);
			memory::write<float>(recoil + recoil_properties::recoil_yaw_max, 0.0f);
			memory::write<float>(recoil + recoil_properties::recoil_pitch_min, 0.0f);
			memory::write<float>(recoil + recoil_properties::recoil_pitch_max, 0.0f);
		}
	}
}

void aim::on_input_loop()
{
	entities::update_local_data();
	if (!settings::o.aim.enabled)
		return;

	aim::update_item_information();
	if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000)
	{
		aim::find_target();
		if (!aim::aim_ctx.found || !aim::aim_ctx.updated)
			return;

		vec3 angle = util::calc_angle(&aim::aim_ctx.camera, &aim::aim_ctx.position);
		angle.normalize();
		angle.clamp();
		if (angle.is_nan())
			return;

		if (settings::o.aim.smoothing.enabled)
		{
			vec3 delta_aim = angle - *entities::local_player.input.get_view();
			delta_aim.normalize();
			delta_aim.clamp();

			float smoothing_amount = 1.0f + (static_cast<float>(settings::o.aim.smoothing.amount) / 10.0f);
			float x_scale = (static_cast<float>(settings::o.aim.smoothing.x) / 8.0f) + 2.0f;
			float y_scale = (static_cast<float>(settings::o.aim.smoothing.y) / 8.0f) + 2.0f;
			delta_aim += { delta_aim.y / y_scale, delta_aim.x / x_scale, 0.0f };
			delta_aim /= smoothing_amount;
			delta_aim.normalize();
			delta_aim.clamp();

			angle = *entities::local_player.input.get_view() + delta_aim;
			angle.normalize();
			angle.clamp();
		}

		if (!angle.is_nan())
			memory::write<vec3>(entities::local_player.player_input + player_input::view_angles, angle);
	}

	/*
	bool aah;
	vec3 view;
	vec3 cam;

	cam = memory::read<vec3>(entities::camera.visual_state + visual_state::camera_position);
	uintptr_t input_state = memory::read<uintptr_t>(entities::local_player.player_input + 0x20);
	uintptr_t message = memory::read<uintptr_t>(input_state + 0x10);
	if (memory::read<int>(message + 0x10) & 1024)
	{
		vec3 best_head;
		float best_fov = FLT_MAX;

		for (auto entry = entities::players.list->next_entry; entry != nullptr; entry = entry->next_entry)
		{
			vec3 head = util::get_bone_position(entry->_item.head_address);
			vec3 head_angles = util::calc_angle(&cam, &head).normalize();

			float fov = util::calc_fov(&view, &head_angles);
			if (fov <= best_fov)
			{
				best_head = head_angles;
				best_fov = fov;
			}
		}

		if ((!math::is_nan(best_head.x) && !math::is_nan(best_head.y) && !math::is_nan(best_head.z)))
		{
			best_head.normalize();
			best_head.clamp();

			vec3 aaah = (view - best_head).normalize();
			memory::write<vec3>(entities::local_player.player_input + player_input::view_angles, best_head);
			memory::write<vec3>(entities::local_player.player_input + player_input::head_angles, aaah);
			memory::write<float>(entities::local_player.player_input + 0x78, -20.0f);
			aah = true;
		}
	}
	else
	{
		if (aah)
		{
			memory::write<vec3>(entities::local_player.player_input + player_input::view_angles, view);
			memory::write<vec3>(entities::local_player.player_input + player_input::head_angles, { 0.0f, 0.0f, 0.0f });
			memory::write<float>(entities::local_player.player_input + 0x78, 50.0f);
		}

		aah = false;
		view = memory::read<vec3>(entities::local_player.player_input + player_input::view_angles).normalize();
	}*/
}
void aim::on_render()
{
	vec3 out;
	if (drawing::world_to_screen(aim::aim_ctx.position, &out))
		drawing::draw_crosshair(out.x, out.y, 4, c_red);

	if (drawing::world_to_screen(aim::aim_ctx.camera, &out))
		drawing::draw_crosshair(out.x, out.y, 4, c_red);
}

/*unsigned long __stdcall loop(void* param)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);

	do
	{
		uintptr_t created_projectiles = memory::read<uintptr_t>(proj + 0x338);
		uintptr_t projectile_list = memory::read<uintptr_t>(created_projectiles + 0x10);

		for (int i = 0; i < memory::read<int>(created_projectiles + 0x18); i++)
		{
			uintptr_t projectile = memory::read<uintptr_t>(projectile_list + 0x20 + (i * 8));

			vec3 velocity = memory::read<vec3>(projectile + 0x18);
			float drag = memory::read<float>(projectile + 0x24);
			float gravity_modifier = memory::read<float>(projectile + 0x28);
			float thickness = memory::read<float>(projectile + 0x2c);

			float v = (velocity.length() / aim::aim_ctx.item_velocity_scale);
			print("%s: velocity%d - drag%d - gravity_modifier%d - thickness%d \n", aim::aim_ctx.item_ammo_type, int(v * 10), int(drag * 10), int(gravity_modifier * 10), int(thickness * 10));
		}

	} while (true);
	return 0;
}*/

void aim::init()
{
	//proj = 0;
	//macro::create_thread(loop);
}