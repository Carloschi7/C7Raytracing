#pragma once
#include <iostream>
#include <random>

float random_float(float fMin, float fMax)
{
	static std::uniform_real_distribution<float> dist(fMin, fMax);
	static std::mt19937 gen;
	return dist(gen);
}

class vec3;
float dot(const vec3& v0, const vec3& v1);
vec3 cross(const vec3& v0, const vec3& v1);

class vec3
{
public:
	vec3() :
		v{ 0.0f,0.0f,0.0f }
	{}

	vec3(float f) :
		v{ f,f,f }
	{}

	vec3(float f0, float f1, float f2) :
		v{ f0,f1,f2 }
	{}

	float length_squared() const
	{
		return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	}

	float length() const
	{
		return std::sqrt(length_squared());
	}

	inline float& x() { return v[0]; }
	inline const float& x() const { return v[0]; }
	inline float& y() { return v[1]; }
	inline const float& y() const { return v[1]; }
	inline float& z() { return v[2]; }
	inline const float& z() const { return v[2]; }

	vec3& operator+=(const vec3& rhs)
	{
		for (int i = 0; i < 3; i++)
			v[i] += rhs.v[i];

		return *this;
	}

	vec3& operator-=(const vec3& rhs)
	{
		for (int i = 0; i < 3; i++)
			v[i] -= rhs.v[i];

		return *this;
	}

	vec3& operator*=(const vec3& rhs)
	{
		for (int i = 0; i < 3; i++)
			v[i] *= rhs.v[i];

		return *this;
	}

	vec3& operator*=(float rhs)
	{
		for (int i = 0; i < 3; i++)
			v[i] *= rhs;

		return *this;
	}

	vec3& operator/=(const vec3& rhs)
	{
		for (int i = 0; i < 3; i++)
			if (rhs.v[i] != 0.0f)
				v[i] /= rhs.v[i];
			else
				throw std::runtime_error("cannot divide by zero");

		return *this;
	}

	vec3& operator/=(float rhs)
	{
		if (rhs == 0.0f)
			throw std::runtime_error("cannot divide by zero");

		for (int i = 0; i < 3; i++)
			v[i] /= rhs;

		return *this;
	}

	vec3& operator=(const vec3& rhs)
	{
		std::memcpy(this, &rhs, 3 * sizeof(float));
		return *this;
	}

	vec3 operator-() const
	{
		return vec3(-v[0], -v[1], -v[2]);
	}

	vec3 norm() const
	{
		vec3 vec = *this;
		vec /= vec.length();
		return vec;
	}

	static vec3 ranged_random(float fMin, float fMax)
	{
		return vec3(random_float(fMin, fMax),
			random_float(fMin, fMax),
			random_float(fMin, fMax));
	}

	static vec3 unit_random()
	{
		return ranged_random(-1.0f, 1.0f);
	}

	static vec3 unit_random_towards(const vec3& norm)
	{
		vec3 ret = unit_random();
		return (dot(ret, norm) > 0) ? ret : -ret;
	}

private:
	float v[3];
};

float dot(const vec3& v0, const vec3& v1)
{
	return v0.x() * v1.x() + v0.y() * v1.y() + v0.z() * v1.z();
}

vec3 cross(const vec3& v0, const vec3& v1)
{
	return vec3(v0.y() * v1.z() - v0.z() * v1.y(),
		v0.z() * v1.x() - v0.x() * v1.z(),
		v0.x() * v1.y() - v0.y() * v1.x());
}

vec3 operator+(const vec3& v0, const vec3& v1)
{
	vec3 ret = v0;
	ret += v1;
	return ret;
}

vec3 operator-(const vec3& v0, const vec3& v1)
{
	vec3 ret = v0;
	ret -= v1;
	return ret;
}

vec3 operator*(const vec3& v0, float f)
{
	vec3 ret = v0;
	ret *= f;
	return ret;
}

vec3 operator*(const vec3& v0, const vec3& v1)
{
	vec3 ret = v0;
	ret *= v1;
	return ret;
}

vec3 operator/(const vec3& v0, float f)
{
	vec3 ret = v0;
	ret /= f;
	return ret;
}

vec3 reflect(const vec3& vec, const vec3& norm)
{
	return vec - norm * 2 * dot(vec, norm);
}

//sin(th2)/sin(th1) = v2/v1
//Ratio = eta1/eta2
vec3 refract(const vec3& vec, const vec3& n, float fRatio)
{
	float costh1 = dot(vec, -n);
	float sinth2 = fRatio * std::sqrt(1.0f - costh1 * costh1);

	if (sinth2 > 1.0f)
		return reflect(vec, n);

	float costh2 = std::sqrt(1.0f - sinth2 * sinth2);
	return fRatio * vec + (fRatio * costh1 - costh2) * n;
}

struct ray
{
	vec3 origin;
	vec3 direction;

	ray() {}

	ray(const vec3& _origin, const vec3& _direction) :
		origin(_origin), direction(_direction)
	{}


	vec3 at(float t) const { return origin + direction * t; }
};