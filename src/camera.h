#pragma once

#include "raymath.h"

class camera
{
public:
	camera(const vec3& origin, const vec3& lookat, float fFov, float fAspectRatio) :
		m_Origin(origin), m_fAspectRatio(fAspectRatio), m_fFov(fFov)
	{
		float fTheta = m_fFov * (3.141592f / 180.0f);
		float fWidth = std::tan(fTheta / 2.0f);

		float fViewportHeight = 2.0f * fWidth;
		float fViewportWidth = fViewportHeight * m_fAspectRatio;

		vec3 dir = (origin - lookat).norm();
		vec3 side = cross(vec3(0, 1, 0), dir);
		vec3 relative_up = cross(dir, side);

		m_vVertical = relative_up * fViewportHeight;
		m_vHorizontal = side * fViewportWidth;

		m_vBottomLeft = -m_vHorizontal / 2.0f - m_vVertical / 2.0f - dir + origin;
	}

	ray GetRay(float u, float v) const
	{
		return ray(m_Origin, m_vBottomLeft + m_vHorizontal * u + m_vVertical * v - m_Origin);
	}

private:
	vec3 m_Origin;
	vec3 m_vBottomLeft, m_vVertical, m_vHorizontal;
	float m_fAspectRatio, m_fFov;
};