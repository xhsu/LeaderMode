/*

Created Date: 10 Mar 2020

*/

#pragma once

class CInterpolation
{
public:
	CInterpolation(void);
	virtual ~CInterpolation(void);

public:
	void SetWaypoints(Vector* prev, Vector start, Vector end, Vector* next);
	void SetViewAngles(Vector start, Vector end);
	void SetFOVs(float start, float end);
	void SetSmoothing(bool start, bool end);
	void Interpolate(float t, Vector& point, Vector& angle, float* fov);

protected:
	void BezierInterpolatePoint(float t, Vector& point);
	void InterpolateAngle(float t, Vector& angle);

	Vector m_StartPoint;
	Vector m_EndPoint;
	Vector m_StartAngle;
	Vector m_EndAngle;
	Vector m_Center;
	float m_StartFov;
	float m_EndFov;

	bool m_SmoothStart;
	bool m_SmoothEnd;
};
