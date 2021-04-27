#pragma once

class c_aim_ctx
{
public:
	vec3 camera;
	char item_name[32];
	char item_ammo_type[32];
	float item_velocity_scale;
	bool updated;

	bool found;
	vec3 position;
	vec3 velocity;

	c_aim_ctx() {}
};

class c_bullet
{
public:
	float velocity;
	float drag;
	float gravity_mod;

	void update(const float m) { this->velocity *= m; }
	c_bullet(const float velocity, const float drag, const float gravity_mod) { this->velocity = velocity; this->drag = drag; this->gravity_mod = gravity_mod; }
	c_bullet() {}
};

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
namespace aim
{
	extern c_aim_ctx aim_ctx;

	//bool calculate_predicted_pos(const vec3& vecTargetPosition, const vec3& vecTargetVelocity, const vec3& vecSourcePosition, const float flBulletSpeed, vec3* pPositionOut, float* pTimeOut);
	//void calculate_bullet_drop(const vec3& vecShootPosition, const float flTravelTime, vec3* pPositionOut);

	void find_target();
	c_bullet get_bullet();

	float calc_bullet_time(const float d, const c_bullet& b);
	void comp_for_drop(vec3 c, vec3& p, const float t, const c_bullet& b);

	void update_item_information();

	void on_input_loop();
	void on_render();
	void init();
};
