#pragma once
#include "raymath.h"

class light
{
public:
	light(vec3 position, vec3 color) :
		m_Position(position), m_Color(color)
	{}

	const vec3& position() const { return m_Position; }
	const vec3& color() const { return m_Color; }

private:
	vec3 m_Position;
	vec3 m_Color;
};