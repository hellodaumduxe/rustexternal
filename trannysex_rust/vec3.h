#pragma once

class vec3
{
public:
	float x, y, z;

	vec3(float _x, float _y, float _z) {
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}

	vec3() {};

	__forceinline float length() const
	{
		return math::sqrt(this->length_sqr());
	}

	__forceinline float length_2d() const
	{
		return math::sqrt(this->length_sqr_2d());
	}

	__forceinline float length_sqr() const
	{
		return (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
	}

	__forceinline float length_sqr_2d() const
	{
		return (this->x * this->x) + (this->y * this->y);
	}

	__forceinline float dot(vec3& v1) const
	{
		return (v1.x * this->x) + (v1.y * this->y) + (v1.z * this->z);
	}

	__forceinline float distance(vec3 target) const
	{
		vec3 delta = *this;
		delta -= target;

		return delta.length();
	}

	__forceinline float distance_2d(vec3 target) const
	{
		vec3 delta = *this;
		delta -= target;

		return delta.length_2d();
	}

	vec3 normalize()
	{
		if (this->x > 180.0f)
			this->x -= 360.f;
		else if (this->x < -180.0f)
			this->x += 360.0f;

		if (this->y > 180.0f)
			this->y -= 360.0f;
		else if (this->y < -180.0f)
			this->y += 360.0f;

		this->z = 0.0f;
		return *this;
	}

	__forceinline void clamp()
	{
		math::clamp(this->x, -89.0f, 89.0f);
		math::clamp(this->y, -180.0f, 180.0f);
		this->z = 0.0f;
	}

	__forceinline void floor()
	{
		this->x = math::floor(this->x);
		this->y = math::floor(this->y);
		this->z = math::floor(this->z);
	}

	__forceinline bool is_nan() const
	{
		return (
			math::is_nan(this->x) ||
			math::is_nan(this->y) ||
			math::is_nan(this->z)
			);
	}

	__forceinline bool is_valid() const
	{
		return (
			(this->x != 0.0f) &&
			(this->y != 0.0f) &&
			(this->z != 0.0f)
			);
	}

	__forceinline bool is_valid_2d()
	{
		return (
			(this->x != 0.0f) &&
			(this->y != 0.0f)
			);
	}


#pragma region +

	__forceinline vec3& operator+=(const vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	__forceinline vec3& operator+=(float v)
	{
		x += v;
		y += v;
		z += v;
		return *this;
	}

	__forceinline vec3 operator+(const vec3& v)
	{
		return { x + v.x, y + v.y, z + v.z };
	}

	__forceinline vec3 operator+(float v)
	{
		return { x + v, y + v, z + v };
	}

#pragma endregion

#pragma region *

	__forceinline vec3& operator*=(const vec3& vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		return *this;
	}

	__forceinline vec3& operator*=(float v)
	{
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	__forceinline vec3 operator*(const vec3& v)
	{
		return { x * v.x, y * v.y, z * v.z };
	}

	__forceinline vec3 operator*(float v)
	{
		return { x * v, y * v, z * v };
	}

#pragma endregion

#pragma region -

	__forceinline vec3& operator-=(const vec3& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}

	__forceinline vec3& operator-=(float v)
	{
		x -= v;
		y -= v;
		z -= v;
		return *this;
	}

	__forceinline vec3 operator-(const vec3& v)
	{
		return { x - v.x, y - v.y, z - v.z };
	}

	__forceinline vec3 operator-(float v)
	{
		return { x - v, y - v, z - v };
	}

#pragma endregion

#pragma region /

	__forceinline vec3& operator/=(const vec3& vec)
	{
		x /= vec.x;
		y /= vec.y;
		z /= vec.z;
		return *this;
	}

	__forceinline vec3& operator/=(float v)
	{
		x /= v;
		y /= v;
		z /= v;
		return *this;
	}

	__forceinline vec3 operator/(const vec3& v)
	{
		return { x / v.x, y / v.y, z / v.z };
	}

	__forceinline vec3 operator/(float v)
	{
		return { x / v, y / v, z / v };
	}

#pragma endregion
};
