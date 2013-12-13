#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>

using namespace glm;

class Spline : public Mesh
{
public:
	Spline(void);
	void GeneratePoints(vec3 startPoint, vec3 endPoint);
	void GeneratePoints(vec3 controlPoint, vec3 previousEndPoint, vec3 endPoint);
	~Spline(void);

	vec3 m_startPoint;
	vec3 m_controlPoint1;
	vec3 m_controlPoint2;
	vec3 m_endPoint;
};

