#include "Spline.h"


Spline::Spline(void)
{
}

// to generate first spline only
void Spline::GeneratePoints(vec3 startPoint, vec3 endPoint)
{
	m_startPoint = startPoint;
	m_endPoint = endPoint;
	// work with control pt 1
	m_controlPoint1 = (2.0f/3.0f)*m_startPoint + (1.0f/3.0f)*m_endPoint + vec3(0.0f, 0, 0);
	m_controlPoint2 = (1.0f/3.0f)*m_startPoint + (2.0f/3.0f)*m_endPoint + vec3(0.0f, 0, 0);
}

//to generate spline based on last spline
void Spline::GeneratePoints(vec3 controlPoint, vec3 previousEndPoint, vec3 endPoint)
{
	m_startPoint = previousEndPoint;
	m_endPoint = endPoint;
	m_controlPoint1 = m_startPoint + (previousEndPoint - controlPoint);
	m_controlPoint2 = (1.0f/3.0f)*m_startPoint + (2.0f/3.0f)*m_endPoint + vec3(0.0f, 0, 0);
}

Spline::~Spline(void)
{
}
