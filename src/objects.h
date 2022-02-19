#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include "raymath.h"

class material;
class coarse;

struct hit_data
{
	//If bHasHit is false, the other vars may not be defined
	bool bHasHit = false;
	float t = 0.0f;
	vec3 hitpoint;
	vec3 norm;
	bool front_face;
	std::shared_ptr<material> mat = nullptr;
};

class object
{
public:
	virtual hit_data RayHasHit(const ray& r, float fMin, float fMax) = 0;
protected:
	std::shared_ptr<material> m_Material;
};

class object_list
{
public:
	object_list() {}
	void push_back(const std::shared_ptr<object>& obj) { m_Objects.push_back(obj); }
	void clear() { m_Objects.clear(); }

	hit_data RayHasHitAny(const ray& r, float fMin, float fMax) const
	{
		hit_data data;
		float fLocalMax = fMax;

		for (auto& obj : m_Objects)
		{
			hit_data local_data = obj->RayHasHit(r, fMin, fLocalMax);
			if (local_data.bHasHit)
			{
				data = local_data;
				fLocalMax = (r.origin - data.hitpoint).length();
			}
		}

		return data;
	}

private:
	std::vector<std::shared_ptr<object>> m_Objects;
};



class sphere : public object
{
public:
	sphere(vec3 _origin, float _radius, std::shared_ptr<material> mat)
		:origin(_origin), radius(_radius)
	{
		m_Material = mat;
	}

	hit_data RayHasHit(const ray& r, float fMin, float fMax) override
	{
		hit_data data;
		vec3 diff = r.origin - origin;
		float a = dot(r.direction, r.direction);
		float b = 2 * dot(diff, r.direction);
		float c = dot(diff, diff) - radius * radius;
		float delta = (b * b - 4 * a * c);
		data.bHasHit = delta > 0;

		if (!data.bHasHit)
			return data;

		float sqrtdelta = std::sqrt(delta);
		float root[2]{ (-b - sqrtdelta) / (2 * a), (-b + sqrtdelta) / (2 * a) };

		//Discard collisions behind the observer
		float ft = root[0];
		if (ft < fMin || ft > fMax)
		{
			ft = root[1];
			if (ft < fMin || ft > fMax)
			{
				data.bHasHit = false;
				return data;
			}
		}

		data.t = ft;
		data.hitpoint = r.at(data.t);
		data.norm = (data.hitpoint - origin).norm();
		//Normal orientation
		if (dot(data.norm, r.direction) < 0)
		{
			data.front_face = true;
		}
		else
		{
			data.norm = -data.norm;
			data.front_face = false;
		}

		data.mat = m_Material;

		return data;
	}
private:
	vec3 origin;
	float radius;
};

//MATERIALS FOR OBJECTS

class material
{
public:
	material(const vec3& color) :
		m_Color(color)
	{}

	virtual ray scatter(const ray& r, const hit_data& data, vec3& color) = 0;
protected:
	vec3 m_Color;
};

class coarse : public material
{
public:
	coarse(const vec3& color) :
		material(color)
	{}

	ray scatter(const ray& r, const hit_data& data, vec3& color) override
	{
		color = m_Color;
		return ray(data.hitpoint, data.norm + vec3::unit_random());
	}
};

class metal : public material
{
public:
	metal(const vec3& color) :
		material(color)
	{}

	ray scatter(const ray& r, const hit_data& data, vec3& color) override
	{
		color = m_Color;
		return ray(data.hitpoint, reflect(r.direction, data.norm));
	}
};

class glass : public material
{
public:
	glass(const vec3& color, float fRefractiveIndex) :
		material(color), m_fRefractiveIndex(fRefractiveIndex)
	{}

	ray scatter(const ray& r, const hit_data& data, vec3& color) override
	{
		float fIndex = data.front_face ? (1.0f / m_fRefractiveIndex) : m_fRefractiveIndex;

		color = m_Color;
		return ray(data.hitpoint, refract(r.direction.norm(), data.norm, fIndex));
	}
private:
	float m_fRefractiveIndex;
};