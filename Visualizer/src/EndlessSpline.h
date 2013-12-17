#pragma once

#include "Spline.h"
#include <queue>

using namespace glm;

class EndlessSpline : public Mesh
{
public:
	EndlessSpline(void);
	bool Initialize(int numSplines, ivec2 xRand, ivec2 yRand, ivec2 zRand);
	void ConcatenateSplinePoints();
	void BuildMesh(int columns, int rows, float width);
	void CreateBezier();
	void Update(float deltaTime, mat4 view);
	void Draw(const mat4 & projection, mat4 view, const ivec2 & size, Lights & lights, const float time);
	void TakeDown();
	~EndlessSpline(void);

	int m_numSplines;
	vec3 m_randomOffsetVector;
	std::queue<Spline*> m_splineQueue;
	std::vector<vec3> m_splinePoints;
	std::vector<vec3> m_bezierPoints;
protected:
	typedef Mesh super;
private:
	mat4 m_splineMV;
};

