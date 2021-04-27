#include "stdafx.h"

vec3 math::get_bone_pos(uint64_t bone_transform)
{
	if (bone_transform == 0x0)
		return { 0.0f, 0.0f, 0.0f };

	__m128 result;
	const __m128 mul_vec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mul_vec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mul_vec2 = { -2.000, -2.000, 2.000, 0.000 };

	uint8_t buffer[12];
	memory::read_buffer(bone_transform + 0x38, &buffer, sizeof(buffer));

	unsigned int index = *reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(buffer) + 0x8);
	c_transform_data transform_data = memory::read<c_transform_data>(*reinterpret_cast<uint64_t*>(buffer) + 0x18);
	if (transform_data.transform_array && transform_data.transform_indices)
	{
		memory::read_buffer(transform_data.transform_array, memory::transform_array, 0x2580);
		memory::read_buffer(transform_data.transform_indices, memory::transform_indices, 0x320);

		c_matrix34* transform_array = reinterpret_cast<c_matrix34*>(memory::transform_array);
		int* transform_indices = reinterpret_cast<int*>(memory::transform_indices);

		result = *reinterpret_cast<__m128*>(&transform_array[index]);
		int transform_index = transform_indices[index];

		size_t loops = 0;
		while ((transform_index >= 0) && (loops++ < 200))
		{
			c_matrix34 matrix34 = transform_array[transform_index];

			__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));	// xxxx
			__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));	// yyyy
			__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));	// zwxy
			__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));	// wzyw
			__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));	// zzzz
			__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));	// yxwy
			__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

			//cancer intrin math
			result = _mm_add_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mul_vec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mul_vec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mul_vec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mul_vec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mul_vec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mul_vec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), *(__m128*)(&matrix34.vec0));


			transform_index = transform_indices[transform_index];
		}
	}

	return { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
}

#define m_rad_to_degree(rad) (rad * 57.2957795131f)
vec3 math::calc_angle(vec3& src, vec3& dst)
{
	vec3 direction = (src - dst);
	float x = m_rad_to_degree(math::asin(direction.y / direction.length()));
	float y = m_rad_to_degree(-math::atan2(direction.x, -direction.z));
	return { x, y, 0.0f };
}

float math::calc_fov(vec3& view, vec3& aim)
{
	vec3 delta = (aim - view);
	return delta.length();
}
