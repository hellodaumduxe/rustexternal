#pragma once

class vec3;
enum objects_tags
{
	main_camera = 5,
	player = 6,
	terrain = 20001,
	corpse = 20009, //animals & players
	misc = 20006, //trash cans, sleeping bags, storage box, etc
	animal = 20008,
	sky_dome = 20012,
	river_mesh = 20014,
	monument = 20015 //airport, powerplant, etc
};

class c_object
{
public:
	char pad_0000[80]; //0x0000
	int32_t layer; //0x0050
	int16_t tag; //0x0054
	char pad_0056[10]; //0x0056
	uintptr_t name; //0x0060
};

class c_linked_object
{
public:
	char pad[0x8]; //0x0
	uintptr_t next_object; //0x8 
	uintptr_t game_object; //0x10 
};

class c_object_manager
{
public:
	uintptr_t last_tagged_object;
	uintptr_t tagged_objects;
};

enum bones : int 
{
	l_hip = 1,
	l_knee,
	l_foot,
	l_toe,
	l_ankle_scale,
	pelvis,

	penis,
	GenitalCensor,
	GenitalCensor_LOD0,
	Inner_LOD0,
	GenitalCensor_LOD1,
	GenitalCensor_LOD2,
	r_hip,
	r_knee,
	r_foot,
	r_toe,
	r_ankle_scale,
	spine1,
	spine1_scale,
	spine2,
	spine3,
	spine4,
	l_clavicle,
	l_upperarm,
	l_forearm,
	l_hand,
	l_index1,
	l_index2,
	l_index3,
	l_little1,
	l_little2,
	l_little3,
	l_middle1,
	l_middle2,
	l_middle3,
	l_prop,
	l_ring1,
	l_ring2,
	l_ring3,
	l_thumb1,
	l_thumb2,
	l_thumb3,
	IKtarget_righthand_min,
	IKtarget_righthand_max,
	l_ulna,
	neck,
	head,
	jaw,
	eyeTranform,
	l_eye,
	l_Eyelid,
	r_eye,
	r_Eyelid,
	r_clavicle,
	r_upperarm,
	r_forearm,
	r_hand,
	r_index1,
	r_index2,
	r_index3,
	r_little1,
	r_little2,
	r_little3,
	r_middle1,
	r_middle2,
	r_middle3,
	r_prop,
	r_ring1,
	r_ring2,
	r_ring3,
	r_thumb1,
	r_thumb2,
	r_thumb3,
	IKtarget_lefthand_min,
	IKtarget_lefthand_max,
	r_ulna,
	l_breast,
	r_breast,
	BoobCensor,
	BreastCensor_LOD0,
	BreastCensor_LOD1,
	BreastCensor_LOD2,
	collision,
	displacement,
};

struct vec4 
{
	float x, y, z, w;
};

struct c_matrix34
{
	vec4 vec0;
	vec4 vec1;
	vec4 vec2;
};

struct c_transform_data
{
	uint64_t transform_array;
	uint64_t transform_indices;
};


class c_player_input
{
	uint8_t buffer[0xb0];
public:

	make_get(view, vec3, player_input::view_angles);
	make_get(head_angles, vec3, player_input::head_angles);
	make_get(recoil, vec3, player_input::recoil_angles);
};

enum life_states : int
{
	alive = 0,
	dead = 1,
};

struct c_menu_rect
{
	int left;
	int top;
	int right;
	int bottom;

	bool is_click_in_bounds(POINT click)
	{
		if (
			(click.x >= this->left) &&
			(click.x <= this->right) &&
			(click.y >= this->top) &&
			(click.y <= this->bottom)
			)
			return true;

		return false;
	}
};
