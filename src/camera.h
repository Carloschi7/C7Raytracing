#pragma once

#include "raymath.h"

class camera
{
public:
	camera(const vec3& origin, const vec3& lookat, float fFov, float fAspectRatio, 
		float fAperture, float fFocusDist) :
		m_Origin(origin), m_fAspectRatio(fAspectRatio), m_fFov(fFov),
		m_fLensRadius(fAperture / 2.0f), m_fFocusDist(fFocusDist)
	{
		float fTheta = m_fFov * (3.141592f / 180.0f);
		float fWidth = std::tan(fTheta / 2.0f);

		float fViewportHeight = 2.0f * fWidth;
		float fViewportWidth = fViewportHeight * m_fAspectRatio;

		m_TargetToOrigin = (origin - lookat).norm();
		m_Side = cross(vec3(0, 1, 0), m_TargetToOrigin);
		m_RelativeUp = cross(m_TargetToOrigin, m_Side);

		m_vVertical = m_RelativeUp * fViewportHeight;
		m_vHorizontal = m_Side * fViewportWidth;

		m_vBottomLeft = -m_vHorizontal / 2.0f - m_vVertical / 2.0f - m_TargetToOrigin + origin;
	}

	ray GetRay(float u, float v) const
	{
		vec3 vec = random_in_flat_z() * m_fLensRadius;
		vec3 offset = m_Side * vec.x() + m_RelativeUp * vec.y();

		return ray(m_Origin + offset, (m_vBottomLeft + m_vHorizontal * u + m_vVertical * v - m_Origin - offset / m_fFocusDist).norm());
	}
protected:
	vec3 random_in_flat_z() const
	{
		while (true)
		{
			vec3 ret(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f), 0.0f);
			if (ret.length() > 1.0f)
				continue;

			return ret;
		}
	}
private:
	vec3 m_Origin;
	vec3 m_vBottomLeft, m_vVertical, m_vHorizontal;
	vec3 m_TargetToOrigin, m_Side, m_RelativeUp;
	float m_fAspectRatio, m_fFov, m_fLensRadius, m_fFocusDist;
};