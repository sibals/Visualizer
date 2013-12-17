#include "EndlessSpline.h"
#include <glm/gtx/spline.hpp>
#include <iostream>
#include <math.h>

static const int SPLINE_COMPLEXITY = 2;
static const int RIBBON_COLUMNS = 2;
static const float MOVEMENT_MULTIPLIER = 0.0f;

EndlessSpline::EndlessSpline(void)
{
}

bool EndlessSpline::Initialize(int numSplines, ivec2 xRand, ivec2 yRand, ivec2 zRand)
{
	//m_randomOffsetVector = normalize(vec3(rand() % 2 - 1, rand() % 2 - 1, rand() % 2 - 1));
	m_numSplines = numSplines;
	Spline* splineArray = new Spline[numSplines];

	
	float theta = 2.0f * 3.1415926f * float(0) / float(numSplines);//get the current angle 
	float r = 400.0f;
	float x = r * cosf(theta);//calculate the x component 
	float y = r * sinf(theta);//calculate the y component 
	
	float theta2 = 2.0f * 3.1415926f * float(1) / float(numSplines);//get the current angle 
	float x2 = r * cosf(theta);//calculate the x component 
	float y2 = r * sinf(theta);//calculate the y component 

	splineArray[0].GeneratePoints(vec3(x, y, 0), vec3(x2, y2, 0));
	m_splineQueue.push(&splineArray[0]);


	for(int i = 1; i < numSplines; i++) {

		float theta = 2.0f * 3.1415926f * float(i) / float(numSplines); //get the current angle 
		float r = 400.0f;
		float x = r * cosf(theta); //calculate the x component 
		float y = r * sinf(theta); //calculate the y component 

		if (i == numSplines - 2) {
			theta = 2.0f * 3.1415926f * float(1) / float(numSplines);
			
			x = r * cosf(theta);
			y = r * sinf(theta);
			splineArray[i].GeneratePoints(splineArray[i-1].m_controlPoint2, splineArray[i-1].m_endPoint, splineArray[0].m_startPoint);
		} else {
			splineArray[i].GeneratePoints(splineArray[i-1].m_controlPoint2, splineArray[i-1].m_endPoint, vec3(x + rand() % xRand.x - xRand.y, y + rand() % yRand.x - yRand.y, rand() % zRand.x - zRand.y));
		}
		m_splineQueue.push(&splineArray[i]);
	}

	ConcatenateSplinePoints();
	CreateBezier();
	BuildMesh(m_bezierPoints.size(), RIBBON_COLUMNS, 8.0f);
	super::Initialize(10.0f);

	delete[] splineArray;

	return true;
}

// uses the qeueue to make a vector of points that make a mesh
void EndlessSpline::ConcatenateSplinePoints()
{
	m_splinePoints.clear();

	std::queue<Spline*> tempQueue = m_splineQueue;

	Spline* splineToConcatenate = tempQueue.front();
	tempQueue.pop();
	m_splinePoints.push_back(splineToConcatenate->m_startPoint);

	while(!tempQueue.empty()) {
		if(m_splinePoints.size() != 1) { // only if not the first one
			splineToConcatenate = tempQueue.front();
			tempQueue.pop();
		}
		m_splinePoints.push_back(splineToConcatenate->m_controlPoint1);
		m_splinePoints.push_back(splineToConcatenate->m_controlPoint2);
		m_splinePoints.push_back(splineToConcatenate->m_endPoint);
	} 
}

void EndlessSpline::CreateBezier()
{
	m_bezierPoints.clear();
	int itr = 0;
	int index = 0;
	while (itr < int(m_splinePoints.size()) - 4) {
		// four points at a time, iterating by one point 
		vec3 a = m_splinePoints[index];
		index = (index + 1) % m_splinePoints.size();
		vec3 b = m_splinePoints[index];
		index = (index + 1) % m_splinePoints.size();
		vec3 c = m_splinePoints[index];
		index = (index + 1) % m_splinePoints.size();
		vec3 d = m_splinePoints[index];
		index -= 2;
		itr++;

		for (int i = 0; i < SPLINE_COMPLEXITY; i++) {
			vec3 p;
			float t = static_cast<float>(i)/float(SPLINE_COMPLEXITY); // between 0 and 1

			// catmull rom takes in 4 control points, make s curve between middle 2
			p = catmullRom(a, b, c, d, t);

			m_bezierPoints.push_back(p);
		}
	}
}

void EndlessSpline::BuildMesh(int columns, int rows, float width)
{
	// Create planar mesh
	int r, s;

	int index = 0;
	for(r = 0; r < rows; r++) {
		for(s = 0; s < columns; s++) {
			int x = s;
			int y = r;
			int z = 0;

			vec3 vertex = vec3(x, y , z);

			vec3 normal = vec3(0, 0, -1);

			vec3 color = vec3(1.0f, 0.5f, 0.7f);
		
			vec2 tex = vec2((columns - 1) - float(s) / (columns-1), float(r) / (rows-1)); //aligning with GL's idea of what (0, 0) is

			this->vertices.push_back(VertexAttributesPCNT(vertex, color, normal, tex));
		}
	}


	// Wind triangles
	vertex_indices.resize(rows * columns * 6);
	std::vector<unsigned int>::iterator i = vertex_indices.begin();
	//int triangle_number = 0;
	for(r = 0; r < rows - 1; r++) {
        for(s = 0; s < columns - 1; s++) {
            *i++ = r * columns + (s+1);
            *i++ = (r) * columns + (s);
            *i++ = (r+1) * columns + s;
            *i++ = (r+1) * columns + s;
            *i++ = (r+1) * columns + (s+1);
            *i++ = (r) * columns + (s+1);
		}
	}

	ivec2 size = ivec2(m_bezierPoints.size(), RIBBON_COLUMNS);

	//update vertices with new positions from curve
	index = 0;
	for(int y = 0; y < size.y; y++) {
		for(int x = 0; x < size.x; x++) {
			vec3 pos = m_bezierPoints[x];
			pos += 1 * width * float(y);
			//pos.x += y;
			vertices[index].position = pos;
			index++;
		}
	}

	this->CalculateNormals(size.x, size.y);
}

void EndlessSpline::Update(float deltaTime, mat4 view)
{
	float x_translation = deltaTime * MOVEMENT_MULTIPLIER;

	m_splineMV = translate(m_splineMV, vec3(-x_translation, 0.0f, 0.0f));
	
	// ideas toward infinite spline
	/*if(fmod(floor(x_translation), 10.0) == 0.0f) {
		Spline splineToAdd;
		Spline* lastSpline = m_splineQueue.back();

		m_splineQueue.pop();
K
		splineToAdd.GeneratePoints(lastSpline->m_controlPoint2, lastSpline->m_endPoint, lastSpline->m_endPoint + vec3(rand() % 10 + 5, rand() % 4 - 2, rand() % 4 - 2));

		printf("lastSpline ends @ (%f, %f, %f)\nsplineToAdd starts @ (%f, %f, %f)\n\n", lastSpline->m_endPoint.x, lastSpline->m_endPoint.y, lastSpline->m_endPoint.z,
			splineToAdd.m_startPoint.x, splineToAdd.m_startPoint.y, splineToAdd.m_startPoint.z);

		m_splineQueue.push(&splineToAdd);

		ConcatenateSplinePoints();
		CreateBezier();
		BuildMesh(m_bezierPoints.size(), RIBBON_COLUMNS, 1.0f);
		super::Initialize(10.0f);
	}*/
}

void EndlessSpline::Draw(const mat4 & projection, mat4 view, const ivec2 & size, Lights & lights, const float time)
{
	m_splineMV = view;
	Update(time, view);

	glEnable(GL_PROGRAM_POINT_SIZE); //enable to be able to adjust points in shaders
        if (this->GLReturnedError("Mesh::Draw - on entry"))
                return;
		
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
    mat4 model = glm::mat4(1.0f);
    //model = scale(model, vec3(10, 10, 10));
    mat4 mvp = projection * m_splineMV * model;
    mat3 nm = inverse(transpose(mat3(m_splineMV)));

    this->shaders[4]->Use(); //set shader to ads for ribbons
    this->GLReturnedError("Mesh::Draw - after use");
    this->shaders[4]->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(m_splineMV), value_ptr(mvp), value_ptr(nm));

	this->spotlight_wireframe_shader.CustomSetup(1, time, size, projection, view, mvp, nm, lights, 2, cutoff_angle);

    this->GLReturnedError("Mesh::Draw - after common setup");
    glBindVertexArray(this->vertex_array_handle);

	glDrawElements(GL_TRIANGLES, this->vertex_indices.size(), GL_UNSIGNED_INT, &this->vertex_indices[0]);
    glBindVertexArray(0);
    this->GLReturnedError("Mesh::Draw - after draw");

	glEnable(GL_CULL_FACE);

    if (this->GLReturnedError("Mesh::Draw - on exit"))
            return;
}

void EndlessSpline::TakeDown()
{
	m_splinePoints.clear();
	m_bezierPoints.clear();
	super::TakeDown();
}

EndlessSpline::~EndlessSpline(void)
{
}
