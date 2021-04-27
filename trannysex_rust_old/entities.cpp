#include "..\trannysex_rust\entities.h"
#include "stdafx.h"
#include "entities.h"

c_camera entities::camera;
c_local_player entities::local_player;

c_linked_list<c_npc> entities::npcs;
c_linked_list<c_npc> tmp_npc_list;

c_linked_list<c_entity> entities::players;
c_linked_list<c_entity> tmp_player_list;

c_linked_list<c_sleeper> entities::sleepers;
c_linked_list<c_sleeper> tmp_sleeper_list;

bool entities::is_updating = false;
void entities::handle_camera(uintptr_t game_object)
{
	uintptr_t class_pointer = memory::read<uintptr_t>(game_object + 0x30);
	uintptr_t entity_pointer = memory::read<uintptr_t>(class_pointer + 0x18);
	entities::camera.view_matrix_address = (entity_pointer + 0x2e4);

	uintptr_t transform_address = memory::read<uintptr_t>(class_pointer + 0x8);
	entities::camera.visual_state_address = memory::read<uintptr_t>(transform_address + 0x38);
}

void update_active_item_info(uintptr_t inventory, uint32_t active_item_id)
{
	wchar_t w_item_name[24];
	wchar_t w_ammo_name[24];

	strcpy(entities::local_player.active_item_name, s(""));
	strcpy(entities::local_player.active_item_ammo_type, s("none"));
	entities::local_player.active_item_bullet_velocity = 1.0f;

	uintptr_t belt = memory::read<uintptr_t>(inventory + 0x28);
	uintptr_t item_list = memory::read<uintptr_t>(belt + 0x38);
	uintptr_t items = memory::read<uintptr_t>(item_list + 0x10);
	for (int i = 0; i <= 6; i++)
	{
		uintptr_t item = memory::read<uintptr_t>(items + 0x20 + (0x8 * i));
		uint32_t item_id = memory::read<uint32_t>(item + item::id);
		if (active_item_id != item_id)
			continue;

		uintptr_t item_definition = memory::read<uintptr_t>(item + item::definition);
		uintptr_t name_pointer = memory::read<uintptr_t>(item_definition + item_definition::name);

		uintptr_t base_projectile = memory::read<uintptr_t>(item + item::projectile);
		if (base_projectile == 0x0)
			continue;

		uintptr_t recoil = memory::read<uintptr_t>(base_projectile + base_projectile::recoil);
		if (recoil != 0x0)
		{
			memory::write<float>(recoil + recoil_properties::recoil_yaw_min, 0.0f);
			memory::write<float>(recoil + recoil_properties::recoil_yaw_max, 0.0f);
			memory::write<float>(recoil + recoil_properties::recoil_pitch_min, 0.0f);
			memory::write<float>(recoil + recoil_properties::recoil_pitch_max, 0.0f);
		}

		uintptr_t primary_magazine = memory::read<uintptr_t>(base_projectile + base_projectile::primary_magazine);
		uintptr_t ammo_definition = memory::read<uintptr_t>(primary_magazine + magazine::ammo_definition);
		uintptr_t ammo_name_pointer = memory::read<uintptr_t>(ammo_definition + item_definition::name);

		memory::read_buffer(name_pointer + 0x14, &w_item_name, sizeof(w_item_name));
		memory::read_buffer(ammo_name_pointer + 0x14, &w_ammo_name, sizeof(w_ammo_name));

		entities::local_player.active_item_bullet_velocity = memory::read<float>(base_projectile + base_projectile::velocity_scale);
		WideCharToMultiByte(CP_UTF8, 0, w_item_name, sizeof(w_item_name), entities::local_player.active_item_name, 24, 0, 0);
		WideCharToMultiByte(CP_UTF8, 0, w_ammo_name, sizeof(w_item_name), entities::local_player.active_item_ammo_type, 24, 0, 0);
	}
}

void entities::handle_entity(uintptr_t game_object, uintptr_t name)
{
	char entity_name_buffer[64];

	uintptr_t class_pointer = memory::read<uintptr_t>(game_object + 0x30);
	uintptr_t info_pointer = memory::read<uintptr_t>(class_pointer + 0x18);
	uintptr_t base_entity = memory::read<uintptr_t>(info_pointer + 0x28);
	uintptr_t player_model = memory::read<uintptr_t>(base_entity + base_player::player_model);
	if ((base_entity == 0x0) || (info_pointer == 0x0) || (class_pointer == 0x0) || (player_model == 0x0))
		return;


	uintptr_t flags = memory::read<uintptr_t>(base_entity + base_player::player_flags);
	if (flags & 16)
	{
		c_sleeper* sleeper = tmp_sleeper_list.create_item();
		sleeper->base_entity = base_entity;

		sleeper->location = memory::read<vec3>(player_model + player_model::position);

		uintptr_t display_name = memory::read<uintptr_t>(base_entity + base_player::display_name);
		memory::read_buffer(display_name + 0x14, &sleeper->name_buffer, sizeof(sleeper->name_buffer));

		return; //goodbye sleepers
	}


	int life_state = memory::read<int>(base_entity + base_player::life_state);
	if (life_state == life_states::dead)
		return; //fk dead niggas

	memory::read_buffer(name, &entity_name_buffer, sizeof(entity_name_buffer));

	if (macro::strstr(entity_name_buffer, s("LocalPlayer")))
	{
		entities::local_player.position = memory::read<vec3>(player_model + player_model::position);
		entities::local_player.player_input = memory::read<uintptr_t>(base_entity + base_player::player_input);
		entities::local_player.base_player = base_entity;

		uintptr_t inventory = memory::read<uintptr_t>(base_entity + base_player::player_inventory);
		uint32_t active_item_id = memory::read<uint32_t>(base_entity + base_player::active_item);
		update_active_item_info(inventory, active_item_id);
		return;
	}


	float health = memory::read<float>(base_entity + base_player::health);
	float max_health = memory::read<float>(base_entity + base_player::max_health);
	if (health < 1.0f)
		return;

	uintptr_t model = memory::read<uintptr_t>(base_entity + base_player::model);
	uintptr_t transforms = memory::read<uintptr_t>(model + model::bone_transforms);
	if (macro::strstr(entity_name_buffer, s("npc")))
	{
		c_npc* npc = tmp_npc_list.create_item();
		npc->player_model = player_model;

		npc->health = max(0.05f, health);
		npc->max_health = max(0.05f, max_health);
		macro::print_to_buffer(npc->health_buffer, s("NPC %d"), static_cast<int>(health));

		npc->head_address = memory::read<uintptr_t>(math::transform_bones(transforms, head) + 0x10);
		npc->position_address = (player_model + player_model::position);
	}
	else
	{
		c_entity* player = tmp_player_list.create_item();
		player->base_entity = base_entity;
		player->player_model = player_model;

		uintptr_t display_name = memory::read<uintptr_t>(base_entity + base_player::display_name);
		memory::read_buffer(display_name + 0x14, &player->name_buffer, sizeof(player->name_buffer));

		player->health = max(0.05f, health);
		player->max_health = max(0.05f, max_health);
		macro::print_to_buffer(player->health_buffer, s("HP %d"), static_cast<int>(health));

		player->head_address = memory::read<uintptr_t>(math::transform_bones(transforms, head) + 0x10);
		player->spine1_address = memory::read<uintptr_t>(math::transform_bones(transforms, spine1) + 0x10);
		player->spine2_address = memory::read<uintptr_t>(math::transform_bones(transforms, spine2) + 0x10);
		player->spine3_address = memory::read<uintptr_t>(math::transform_bones(transforms, spine3) + 0x10);
		player->spine4_address = memory::read<uintptr_t>(math::transform_bones(transforms, spine4) + 0x10);
		player->l_clavicle_address = memory::read<uintptr_t>(math::transform_bones(transforms, l_clavicle) + 0x10);
		player->l_upperarm_address = memory::read<uintptr_t>(math::transform_bones(transforms, l_upperarm) + 0x10);
		player->l_forearm_address = memory::read<uintptr_t>(math::transform_bones(transforms, l_forearm) + 0x10);
		player->l_hand_address = memory::read<uintptr_t>(math::transform_bones(transforms, l_hand) + 0x10);
		player->r_clavicle_address = memory::read<uintptr_t>(math::transform_bones(transforms, r_clavicle) + 0x10);
		player->r_upperarm_address = memory::read<uintptr_t>(math::transform_bones(transforms, r_upperarm) + 0x10);
		player->r_forearm_address = memory::read<uintptr_t>(math::transform_bones(transforms, r_forearm) + 0x10);
		player->r_hand_address = memory::read<uintptr_t>(math::transform_bones(transforms, r_hand) + 0x10);
		player->l_hip_address = memory::read<uintptr_t>(math::transform_bones(transforms, l_hip) + 0x10);
		player->l_knee_address = memory::read<uintptr_t>(math::transform_bones(transforms, l_knee) + 0x10);
		player->l_foot_address = memory::read<uintptr_t>(math::transform_bones(transforms, l_foot) + 0x10);
		player->r_hip_address = memory::read<uintptr_t>(math::transform_bones(transforms, r_hip) + 0x10);
		player->r_knee_address = memory::read<uintptr_t>(math::transform_bones(transforms, r_knee) + 0x10);
		player->r_foot_address = memory::read<uintptr_t>(math::transform_bones(transforms, r_foot) + 0x10);
	}
}

void run_object_loop()
{
	tmp_npc_list.clear();
	tmp_player_list.clear();
	tmp_sleeper_list.clear();

	uintptr_t object_manager_pointer = memory::read<uintptr_t>(memory::unity_player + o_object_manager);
	c_object_manager object_manager = memory::read<c_object_manager>(object_manager_pointer);

	uintptr_t last_tagged_object = memory::read<uintptr_t>(object_manager.last_tagged_object);
	uintptr_t tagged_objects = memory::read<uintptr_t>(object_manager.tagged_objects);
	if ((tagged_objects == 0x0) || (last_tagged_object == 0x0) || (tagged_objects == last_tagged_object))
		return;

	unsigned long loops = 0;
	while ((tagged_objects != last_tagged_object) && (loops++ < ULONG_MAX))
	{
		c_linked_object linked_object = memory::read<c_linked_object>(tagged_objects);
		if (linked_object.game_object != 0x0)
		{
			c_object object = memory::read<c_object>(linked_object.game_object);
			switch (object.tag)
			{
			case main_camera:
				entities::handle_camera(linked_object.game_object);
				break;

			case player:
				entities::handle_entity(linked_object.game_object, object.name);
				break;
			}
		}

		tagged_objects = linked_object.next_object;
		if (tagged_objects == 0x0)
			break;
	}

#pragma region remove duplicates (cancer)
	for (auto entry = tmp_player_list.list->next_entry; entry != nullptr; entry = entry->next_entry)
	{
		auto entry2 = tmp_player_list.list->next_entry;
		do
		{
			if ((entry != entry2) && (entry->_item.base_entity == entry2->_item.base_entity))
			{
				auto next_entry = entry2->next_entry;
				tmp_player_list.remove_entry(entry2);
				entry2 = next_entry;
			}
			else
				entry2 = entry2->next_entry;
		} while (entry2 != nullptr);
	}
	for (auto entry = tmp_npc_list.list->next_entry; entry != nullptr; entry = entry->next_entry)
	{
		auto entry2 = tmp_npc_list.list->next_entry;
		do
		{
			if ((entry != entry2) && (entry->_item.player_model == entry2->_item.player_model))
			{
				auto next_entry = entry2->next_entry;
				tmp_npc_list.remove_entry(entry2);
				entry2 = next_entry;
			}
			else
				entry2 = entry2->next_entry;
		} while (entry2 != nullptr);
	}
	for (auto entry = tmp_sleeper_list.list->next_entry; entry != nullptr; entry = entry->next_entry)
	{
		auto entry2 = tmp_sleeper_list.list->next_entry;
		do
		{
			if ((entry != entry2) && (entry->_item.base_entity == entry2->_item.base_entity))
			{
				auto next_entry = entry2->next_entry;
				tmp_sleeper_list.remove_entry(entry2);
				entry2 = next_entry;
			}
			else
				entry2 = entry2->next_entry;
		} while (entry2 != nullptr);
	}
#pragma endregion

	while (entities::npcs.is_locked() || entities::players.is_locked() || entities::sleepers.is_locked())
		Sleep(1);

	entities::is_updating = true;
	entities::npcs.copy(&tmp_npc_list);
	entities::players.copy(&tmp_player_list);
	entities::sleepers.copy(&tmp_sleeper_list);
	entities::is_updating = false;
}

unsigned long __stdcall entities::object_loop(void* param)
{
	while (true)
	{
		run_object_loop();
		Sleep(1000);
	}
}

void entities::init()
{
}

void entities::start()
{
	entities::npcs.allocate_list();
	tmp_npc_list.allocate_list();

	entities::players.allocate_list();
	tmp_player_list.allocate_list();

	entities::sleepers.allocate_list();
	tmp_sleeper_list.allocate_list();

	entities::is_updating = true;

	HANDLE thread = CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(entities::object_loop), nullptr, 0, 0);
	SetThreadPriority(thread, THREAD_PRIORITY_BELOW_NORMAL);
}