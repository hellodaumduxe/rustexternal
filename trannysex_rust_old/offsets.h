#pragma once

#define o_object_manager 0x163d080

enum model : uintptr_t
{
	bone_transforms = 0x40, //Model::boneTransforms
};

enum player_model : uintptr_t
{
	velocity = 0x170, //PlayerModel::newVelocity
	position = 0x17c, //PlayerModel::position
};

enum base_player : uintptr_t
{
	model = 0xb0, //BaseEntity::model
	is_visible = 0x120, //: BaseEntity::isVisible

	life_state = 0x1ec, //BaseCombatEntity::lifestate
	health = 0x1f4, //BaseCombatEntity::_health
	max_health = 0x1f8, //BaseCombatEntity::_maxHealth

	player_flags = 0x480, //BasePlayer::playerFlags
	player_inventory = 0x490, //BasePlayer::inventory
	player_input = 0x4A8, //BasePlayer::input
	display_name = 0x4E8, //BasePlayer::_displayName
	player_model = 0x520, //BasePlayer::playerModel
	active_item = 0x5B8, //BasePlayer::clActiveItem
};

enum base_projectile : uintptr_t
{
	velocity_scale = 0x24c, //BaseProjectile::projectileVelocityScale
	primary_magazine = 0x280, //BaseProjectile::primaryMagazine
	recoil = 0x2a0, //BaseProjectile::recoil
};

enum recoil_properties : uintptr_t
{
	recoil_yaw_min = 0x18, //RecoilProperties::recoilYawMin
	recoil_yaw_max = 0x1c, //RecoilProperties::recoilYawMax
	recoil_pitch_min = 0x20, //RecoilProperties::recoilPitchMin
	recoil_pitch_max = 0x24, //RecoilProperties::recoilPitchMax
};

enum magazine : uintptr_t
{
	ammo_definition = 0x20, //BaseProjectile.Magazine::ammoType
};

enum item : uintptr_t
{
	definition = 0x18, //Item::info
	id = 0x20, //Item::uid
	projectile = 0x90, //Item::heldEntity
};

enum item_definition : uintptr_t
{
	name = 0x20, //ItemDefinition::shortname
};

enum player_input : uintptr_t
{
	view_angles = 0x3c, //PlayerInput::bodyAngles
	head_angles = 0x58, //PlayerInput::headAngles
	recoil_angles = 0x64, //PlayerInput::recoilAngles
};

enum visual_state : uintptr_t
{
	camera_position = 0xb0
};