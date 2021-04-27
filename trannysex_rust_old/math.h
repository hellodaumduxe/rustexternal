#pragma once

class vec3;
namespace math
{
#pragma region cancer math.h stuff
	inline float atan2(float y, float x)
	{
		const float n1 = 0.97239411f;
		const float n2 = -0.19194795f;
		float result = 0.0f;
		if (x != 0.0f)
		{
			const union { float flVal; uint32_t nVal; } tYSign = { y };
			const union { float flVal; uint32_t nVal; } tXSign = { x };
			if (fabsf(x) >= fabsf(y))
			{
				union { float flVal; uint32_t nVal; } tOffset = { m_pi };
				// Add or subtract PI based on y's sign.
				tOffset.nVal |= tYSign.nVal & 0x80000000u;
				// No offset if x is positive, so multiply by 0 or based on x's sign.
				tOffset.nVal *= tXSign.nVal >> 31;
				result = tOffset.flVal;
				const float z = y / x;
				result += (n1 + n2 * z * z) * z;
			}
			else // Use atan(y/x) = pi/2 - atan(x/y) if |y/x| > 1.
			{
				union { float flVal; uint32_t nVal; } tOffset = { m_half_pi };
				// Add or subtract PI/2 based on y's sign.
				tOffset.nVal |= tYSign.nVal & 0x80000000u;
				result = tOffset.flVal;
				const float z = x / y;
				result -= (n1 + n2 * z * z) * z;
			}
		}
		else if (y > 0.0f)
		{
			result = m_half_pi;
		}
		else if (y < 0.0f)
		{
			result = -m_half_pi;
		}
		return result;
	}
	__forceinline float sqrt(float in)
	{
		float out = 0.0f;
		_mm_store_ss(&out, _mm_sqrt_ss(_mm_load_ss(&in)));
		return out;
	}
	inline float asin(float x)
	{
		float negate = static_cast<float>(x < 0);
		x = abs(x);
		float ret = -0.0187293f;
		ret *= x;
		ret += 0.0742610f;
		ret *= x;
		ret -= 0.2121144f;
		ret *= x;
		ret += 1.5707288f;
		ret = 3.14159265358979f * 0.5f - math::sqrt(1.0f - x) * ret;
		return ret - 2 * negate * ret;
	}
	inline float cos_32s(float x)
	{
		const float c1 = 0.99940307f;
		const float c2 = -0.49558072f;
		const float c3 = 0.03679168f;
		float x2;      // The input argument squared
		x2 = x * x;
		return (c1 + x2 * (c2 + c3 * x2));
	}
	inline float floor(float n)
	{
		__m128 ang_m128 = _mm_load_ps(&n);

		__m128 dst = __m128();
		_mm_round_ss(dst, ang_m128, _MM_FROUND_FLOOR);

		float res = 0.0f;
		_mm_store_ps(&res, dst);
		return res;
	}
	inline float cos(float angle)
	{
		angle = angle - math::floor(angle * invtwopi) * two_pi;
		angle = angle > 0.0f ? angle : -angle;

		if (angle < m_half_pi) return cos_32s(angle);
		if (angle < m_pi) return -cos_32s(m_pi - angle);
		if (angle < m_three_half_pi) return -cos_32s(angle - m_pi);
		return cos_32s(two_pi - angle);
	}
	inline float sin(float angle)
	{
		return math::cos(m_half_pi - angle);
	}
#pragma endregion

	vec3 get_bone_pos(uint64_t bone_transform);
	__forceinline uintptr_t transform_bones(uintptr_t transforms, e_bones bone)
	{
		return memory::read<uintptr_t>(transforms + 0x20 + bone * 0x8);
	}


	__forceinline void clamp(float& value, float min, float max)
	{
		if (value >= max)
			value = max;

		if (value <= min)
			value = min;
	}

	__forceinline void clamp(int& value, int min, int max)
	{
		if (value >= max)
			value = max;

		if (value <= min)
			value = min;
	}

	__forceinline bool is_nan(float n) { return (n != n); };

	vec3 calc_angle(vec3& src, vec3& dst);
	float calc_fov(vec3& view, vec3& aim);
};