#pragma once

namespace util
{
	vec3 calc_angle(vec3* src, vec3* dst);
	inline float calc_fov(vec3* view, vec3* aim)
	{
		vec3 delta = (*aim - *view);
		return delta.length();
	}

	vec3 get_bone_position(uint64_t transform);
	inline uintptr_t transform_bones(uintptr_t transforms, bones bone) 
	{ 
		if (!macro::is_valid_address(transforms)) return 0;
		uintptr_t bone_transform = memory::read<uintptr_t>(transforms + 0x20 + bone * 0x8);
		if (!macro::is_valid_address(bone_transform)) return 0;
		return memory::read<uintptr_t>(bone_transform + 0x10);
	}
};