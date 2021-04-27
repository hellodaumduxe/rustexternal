#include "stdafx.h"

c_camera entities::camera;
c_local_player entities::local_player;

c_linked_list<c_entity> entities::players;
c_linked_list<c_entity> tmp_players;

c_linked_list<c_sleeper> entities::sleepers;
c_linked_list<c_sleeper> tmp_sleepers;

c_linked_list<c_npc> entities::npcs;
c_linked_list<c_npc> tmp_npcs;

c_linked_list<c_tc> entities::tcs;
c_linked_list<c_tc> tmp_tcs;
bool entities::is_updating = false;

char* pid_buffer = nullptr;
char* unity_player_buffer = nullptr;
char* game_assembly_buffer = nullptr;
char* loop_count_buffer = nullptr;
char* last_update_buffer = nullptr;
char* update_time_buffer = nullptr;
char* loop_state_buffer = nullptr;

void entities::handle_entity(uintptr_t object, uintptr_t name)
{
	char name_buffer[64] = {};
	wchar_t display_name_buffer[64] = {};

	uintptr_t class_pointer = memory::read<uintptr_t>(object + 0x30);
	pointer_validity_check(class_pointer);
	uintptr_t info_pointer = memory::read<uintptr_t>(class_pointer + 0x18);
	pointer_validity_check(info_pointer);
	uintptr_t base_entity = memory::read<uintptr_t>(info_pointer + 0x28);
	pointer_validity_check(base_entity);
	uintptr_t player_model = memory::read<uintptr_t>(base_entity + base_player::player_model);
	pointer_validity_check(player_model);

	float health = memory::read<float>(base_entity + base_player::health);
	float max_health = memory::read<float>(base_entity + base_player::max_health);
	int life_state = memory::read<int>(base_entity + base_player::life_state);
	if ((life_state != life_states::alive) || (health < 1.0f))
		return; // no dead niggas allowed

	//read name shit
	memory::read_buffer(name, &name_buffer, sizeof(name_buffer));
	uintptr_t display_name = memory::read<uintptr_t>(base_entity + base_player::display_name);
	pointer_validity_check(display_name);
	memory::read_buffer(display_name + 0x14, &display_name_buffer, sizeof(display_name_buffer));

	uint64_t flags = memory::read<uint64_t>(base_entity + base_player::player_flags);
	if (flags & 16) //sleepers
	{
		strcpy(loop_state_buffer, s("creating sleeper"));
		validate_entry(tmp_sleepers, base_entity);
		c_sleeper* sleeper = tmp_sleepers.create_item();

		sleeper->base_entity = base_entity;
		sleeper->location = memory::read<vec3>(player_model + player_model::position);
		memcpy(&sleeper->name_buffer, display_name_buffer, sizeof(sleeper->name_buffer));
		return;
	}

	//local player
	if (macro::strstr(name_buffer, s("LocalPlayer")))
	{
		strcpy(loop_state_buffer, s("updating local pzlayer"));
		entities::local_player.player_input = memory::read<uintptr_t>(base_entity + base_player::player_input);
		entities::local_player.base_player = base_entity;
		entities::local_player.player_model = player_model;
		return;
	}
	else if (macro::strstr(name_buffer, s("npc")))
	{
		strcpy(loop_state_buffer, s("creating npc"));
		validate_entry(tmp_npcs, player_model);
		c_npc* npc = tmp_npcs.create_item();
		npc->player_model = player_model;

		npc->health_percentage = min(1.0f, (health / max_health));
		macro::print_to_buffer(npc->health_buffer, s("NPC %d"), static_cast<int>(health));
		return;
	}

	uintptr_t model = memory::read<uintptr_t>(base_entity + base_player::model);
	pointer_validity_check(model);
	uintptr_t transforms = memory::read<uintptr_t>(model + model::bone_transforms);
	pointer_validity_check(transforms);

	strcpy(loop_state_buffer, s("creating player"));
	validate_entry(tmp_players, base_entity);
	c_entity* player = tmp_players.create_item();

	player->base_entity = base_entity;
	player->player_model = player_model;
	memcpy(&player->name_buffer, display_name_buffer, sizeof(player->name_buffer));

	player->health_percentage = min(1.0f, (health / max_health));
	macro::print_to_buffer(player->health_buffer, s("HP %d"), static_cast<int>(health));

	player->head_address = util::transform_bones(transforms, head);
	player->spine1_address = util::transform_bones(transforms, spine1);
	player->spine2_address = util::transform_bones(transforms, spine2);
	player->spine4_address = util::transform_bones(transforms, spine4);
	player->l_clavicle_address = util::transform_bones(transforms, l_clavicle);
	player->l_upperarm_address = util::transform_bones(transforms, l_upperarm);
	player->l_forearm_address = util::transform_bones(transforms, l_forearm);
	player->l_hand_address = util::transform_bones(transforms, l_hand);
	player->r_clavicle_address = util::transform_bones(transforms, r_clavicle);
	player->r_upperarm_address = util::transform_bones(transforms, r_upperarm);
	player->r_forearm_address = util::transform_bones(transforms, r_forearm);
	player->r_hand_address = util::transform_bones(transforms, r_hand);
	player->l_hip_address = util::transform_bones(transforms, l_hip);
	player->l_knee_address = util::transform_bones(transforms, l_knee);
	player->l_foot_address = util::transform_bones(transforms, l_foot);
	player->r_hip_address = util::transform_bones(transforms, r_hip);
	player->r_knee_address = util::transform_bones(transforms, r_knee);
	player->r_foot_address = util::transform_bones(transforms, r_foot);
}


bool entities::loaded = false;
void entities::load()
{
	if (entities::loaded)
		return;

	print("loading \n");
	entities::unload();
	memory::pid = memory::find_process(s("RustClient.exe"));
	if (memory::pid == 0)
		return;
	Sleep(500);
	memory::unity_player = memory::find_module(s("UnityPlayer.dll"));
	if (memory::unity_player == 0)
		return;
	Sleep(500);
	memory::game_assembly = memory::find_module(s("GameAssembly.dll"));
	if (memory::game_assembly == 0)
		return;
	Sleep(500);
	entities::loaded = true;
}

void entities::unload()
{
	print("unloading \n");
	entities::loaded = false;
	memory::pid = 0;
	memory::unity_player = 0;
	memory::game_assembly = 0;

	entities::camera.view_matrix = 0;
	entities::camera.visual_state = 0;

	entities::local_player.player_input = 0;
	entities::local_player.base_player = 0;
	entities::local_player.player_model = 0;
	entities::local_player.position = { 0.0f, 0.0f, 0.0f };
	memset(&entities::local_player.input, 0, sizeof(entities::local_player.input));

	entities::players.clear();
	tmp_players.clear();
	entities::sleepers.clear();
	tmp_sleepers.clear();
	entities::npcs.clear();
	tmp_npcs.clear();
	entities::tcs.clear();
	tmp_tcs.clear();
}

unsigned long __stdcall entities::loop(void* param)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
	do
	{
		memory::refresh_working_set();
		input::is_loaded ? entities::load() : entities::unload();
		if (entities::loaded) entities::run_loop();

		Sleep(1000);
	} while (true);
	return 0;
}

void entities::update_local_data()
{
	entities::local_player.position = memory::read<vec3>(entities::local_player.player_model + player_model::position);
	memory::read_buffer(entities::local_player.player_input, &entities::local_player.input, sizeof(entities::local_player.input));
	entities::local_player.input.get_view()->normalize();
}

int debug_index = 0;
void entities::on_render()
{
	if (!settings::o.misc.debug)
		return;

	debug_index = 0;
	auto add_debug_element = [=](const char* name, char* buffer)
	{
		float x = overlay::rust_width - 215.0f;
		holy_font::draw_string(holy_font::pixel_font, name, x, 10 + static_cast<float>(debug_index * 12), 0, flag_outline, c_green);
		holy_font::draw_string(holy_font::pixel_font, buffer, x + 100, 10 + static_cast<float>(debug_index * 12), 0, flag_outline, c_white);
		debug_index++;
	};

	add_debug_element(s("rust pid"), pid_buffer);
	add_debug_element(s("unity_player dll"), unity_player_buffer);
	add_debug_element(s("game_assembly dll"), game_assembly_buffer);
	add_debug_element(s("bn size"), loop_count_buffer);
	add_debug_element(s("last update tick"), last_update_buffer);
	add_debug_element(s("update time (ms)"), update_time_buffer);
	add_debug_element(s("state"), loop_state_buffer);
}

void entities::run_loop()
{
	uintptr_t object_manager_ptr = memory::read<uintptr_t>(memory::unity_player + o_object_manager);
	pointer_validity_check(object_manager_ptr);
	c_object_manager object_manager = memory::read<c_object_manager>(object_manager_ptr);

	uintptr_t last_tagged_object = memory::read<uintptr_t>(object_manager.last_tagged_object);
	uintptr_t tagged_objects = memory::read<uintptr_t>(object_manager.tagged_objects);
	if (tagged_objects == last_tagged_object) return;

	tmp_players.clear();
	tmp_sleepers.clear();
	tmp_npcs.clear();
	tmp_tcs.clear();
	unsigned long long start_time = GetTickCount64();
	unsigned long long loops = 0;
	do
	{
		c_linked_object linked_object = memory::read<c_linked_object>(tagged_objects);
		c_object object = memory::read<c_object>(linked_object.game_object);
		if (macro::is_valid_address(linked_object.game_object) && (object.tag == main_camera))
		{
			uintptr_t class_pointer = memory::read<uintptr_t>(linked_object.game_object + 0x30);
			uintptr_t entity_pointer = memory::read<uintptr_t>(class_pointer + 0x18);
			uintptr_t transform_address = memory::read<uintptr_t>(class_pointer + 0x8);
			uintptr_t visual_state = memory::read<uintptr_t>(transform_address + 0x38);

			entities::camera.visual_state = visual_state;
			entities::camera.view_matrix = (entity_pointer + 0x2e4);
			break;
		}

		tagged_objects = linked_object.next_object;
		if (!macro::is_valid_address(tagged_objects)) break;
	} while ((tagged_objects != last_tagged_object) && (loops++ < 15000));

	uintptr_t base_networkables = memory::read<uintptr_t>(memory::game_assembly + o_base_networkables);
	uintptr_t bn1 = memory::read<uintptr_t>(base_networkables + 0xb8);
	uintptr_t bn2 = memory::read<uintptr_t>(bn1);
	uintptr_t bn3 = memory::read<uintptr_t>(bn2 + 0x10);
	uintptr_t bn4 = memory::read<uintptr_t>(bn3 + 0x28);
	uintptr_t bn5 = memory::read<uintptr_t>(bn4 + 0x18);
	size_t bn_count = memory::read<size_t>(bn4 + 0x10);
	for (size_t i = 0; i < bn_count; i++)
	{
		auto get_object_base = [=](uintptr_t bn5, size_t index)->uintptr_t
		{
			uintptr_t object_reference = memory::read<uintptr_t>(bn5 + 0x20 + (index * 8));
			uintptr_t object_pointer = memory::read<uintptr_t>(object_reference + 0x10);
			return memory::read<uintptr_t>(object_pointer + 0x30);
		};

		uintptr_t object_base = get_object_base(bn5, i);
		c_object object = memory::read<c_object>(object_base);

		if (object.tag == player)
			entities::handle_entity(object_base, object.name);
		else
		{
			char name_buffer[64] = {};
			memory::read_buffer(object.name, &name_buffer, sizeof(name_buffer));

			if (macro::strstr(name_buffer, s("cupboard")))
			{
				uintptr_t class_pointer = memory::read<uintptr_t>(object_base + 0x30);
				uintptr_t instance_pointer = memory::read<uintptr_t>(class_pointer + 0x8);
				uintptr_t model_pointer = memory::read<uintptr_t>(instance_pointer + 0x38);
				vec3 base_position = memory::read<vec3>(model_pointer + 0xb0);

				c_tc* tc = tmp_tcs.create_item();
				tc->b1 = (base_position + vec3{ 0.6f, 0.0f, 0.6f });
				tc->b2 = (base_position + vec3{ -0.6f, 0.0f, 0.6f });
				tc->b3 = (base_position + vec3{ 0.6f, 0.0f, -0.6f });
				tc->b4 = (base_position + vec3{ -0.6f, 0.0f, -0.6f });
				tc->t1 = (tc->b1 + vec3{ 0.0f, 2.0f, 0.0f });
				tc->t2 = (tc->b2 + vec3{ 0.0f, 2.0f, 0.0f });
				tc->t3 = (tc->b3 + vec3{ 0.0f, 2.0f, 0.0f });
				tc->t4 = (tc->b4 + vec3{ 0.0f, 2.0f, 0.0f });
			}
		}
	}
	unsigned long long end_time = GetTickCount64();

	entities::is_updating = true;
	entities::players.copy(&tmp_players);
	entities::sleepers.copy(&tmp_sleepers);
	entities::npcs.copy(&tmp_npcs);
	entities::tcs.copy(&tmp_tcs);
	entities::is_updating = false;

	macro::print_to_buffer(pid_buffer, s("%d"), memory::pid);
	macro::print_to_buffer(unity_player_buffer, s("%p"), memory::unity_player);
	macro::print_to_buffer(game_assembly_buffer, s("%p"), memory::game_assembly);
	macro::print_to_buffer(loop_count_buffer, s("%I64u"), bn_count);
	macro::print_to_buffer(last_update_buffer, s("%I64u"), end_time);
	macro::print_to_buffer(update_time_buffer, s("%I64u"), (end_time - start_time));
}

void entities::init()
{
	entities::players.allocate_list();
	tmp_players.allocate_list();
	entities::sleepers.allocate_list();
	tmp_sleepers.allocate_list();
	entities::npcs.allocate_list();
	tmp_npcs.allocate_list();
	entities::tcs.allocate_list();
	tmp_tcs.allocate_list();
	entities::unload();

	pid_buffer = reinterpret_cast<char*>(memory::heap_alloc(256));
	unity_player_buffer = reinterpret_cast<char*>(memory::heap_alloc(256));
	game_assembly_buffer = reinterpret_cast<char*>(memory::heap_alloc(256));
	loop_count_buffer = reinterpret_cast<char*>(memory::heap_alloc(256));
	last_update_buffer = reinterpret_cast<char*>(memory::heap_alloc(256));
	update_time_buffer = reinterpret_cast<char*>(memory::heap_alloc(256));
	loop_state_buffer = reinterpret_cast<char*>(memory::heap_alloc(256));

	macro::create_thread_fake_start(entities::loop);
}
