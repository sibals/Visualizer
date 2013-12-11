#pragma once
#include <queue>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include "Mesh.h"

class Spline : public Mesh
{
public: 
	Spline();
	~Spline();
	std::vector<glm::vec3> m_splinePoints;
	std::vector<glm::vec3> m_bezierPoints;
	int m_headPointer;
	int m_tailPointer;
	int m_numPoints;
	int m_numBezierPoints;
	float m_timer;

	void TakeDown();
	bool Initialize(int length, glm::vec3 startPos, glm::vec3 endPos);
	void Draw(const mat4 & projection, mat4 view, const ivec2 & size, Lights & lights, const float time);
	vec3 Lerp(vec3 dest, vec3 a, vec3 b, float t);
	vec3 Bezier(vec3 dest, vec3 a, vec3 b, vec3 c, vec3 d, float t);
	void Update(float deltaTime);
	void BuildMesh(int columns, int rows);


private: 
	void CreatePoints(int length, glm::vec3 startPos, glm::vec3 endPos);
	void CreateBezier(int detail);
	int m_splineModificationIndex;
	std::vector<vec3> m_randomOffsets;
protected:
	typedef Mesh super;
};