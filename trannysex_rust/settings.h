#pragma once

class c_settings
{
public:
	struct //aim
	{
		bool enabled;
		int fov;
		bool silent;

		struct
		{
			bool enabled;
			int amount;
			int x;
			int y;
		} smoothing;
	} aim;

	struct //visuals
	{
		bool crosshair;
		bool radar;
		int radar_distance;
		bool npcs;
		bool sleepers;
		bool items;
		bool chest;
		bool tc;

		struct
		{
			bool enabled;
			bool name;
			bool skeleton;
			bool health;
		} players;
	} visuals;

	struct //misc
	{
		bool debug;
		bool no_recoil;
		bool no_spread;
	} misc;

	c_settings() {}
};

namespace settings
{
	extern c_settings o;
};