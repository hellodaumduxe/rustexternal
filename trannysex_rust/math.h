#pragma once

class c_matrix4x4
{
public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};

	inline void transpose(c_matrix4x4* a)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				this->m[i][j] = a->m[j][i];
	}
	c_matrix4x4() {}
};

typedef double(__cdecl* atan_t)(double y);
typedef double(__cdecl* atan2_t)(double y, double x);
typedef double(__cdecl* cos_t)(double x);
typedef double(__cdecl* sin_t)(double x);
typedef double(__cdecl* asin_t)(double x);
namespace math
{
	extern atan_t atan;
	extern atan2_t atan2;
	extern cos_t cos;
	extern sin_t sin;
	extern asin_t asin;
	inline float atan2f(float y, float x) { return static_cast<float>(math::atan2(static_cast<double>(y), static_cast<double>(x))); }
	inline float atanf(float y) { return static_cast<float>(math::atan(static_cast<double>(y))); }
	inline float cosf(float n) { return static_cast<float>(math::cos(static_cast<double>(n))); }
	inline float sinf(float n) { return static_cast<float>(math::sin(static_cast<double>(n))); }
	inline float asinf(float n) { return static_cast<float>(math::asin(static_cast<double>(n))); }

	inline bool is_nan(const float n) { return (n != n); }
	inline float sqrt(const float n) { float o = 0.0f; _mm_store_ss(&o, _mm_sqrt_ss(_mm_load_ss(&n))); return o; }
	inline float square(const float n) { return (n * n); }
	inline float floor(const float n)
	{
		__m128 a = _mm_load_ps(&n);
		__m128 v = __m128();
		_mm_round_ss(v, a, _MM_FROUND_FLOOR);

		float o = 0.0f;
		_mm_store_ps(&o, v); 
		return o;
	}

	inline void clamp(float& value, const float min, const float max)
	{
		if (value >= max)
			value = max;
		if (value <= min)
			value = min;
	}
	inline void clamp(int& value, const int min, const int max)
	{
		if (value >= max)
			value = max;
		if (value <= min)
			value = min;
	}

	void init();
};