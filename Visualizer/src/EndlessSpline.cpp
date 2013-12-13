#include "EndlessSpline.h"
#include <glm/gtx/spline.hpp>

static const int SPLINE_COMPLEXITY = 10;
static const int RIBBON_COLUMNS = 2;

EndlessSpline::EndlessSpline(void)
{
}

bool EndlessSpline::Initialize(int numSplines)
{
	//m_randomOffsetVector = normalize(vec3(rand() % 2 - 1, rand() % 2 - 1, rand() % 2 - 1));
	m_numSplines = numSplines;
	Spline* splineArray = new Spline[numSplines];

	splineArray[0].GeneratePoints(vec3(0.0f, 0.0f, 0.0f), vec3(rand() % 10 + 5, rand() % 4 - 2, rand() % 4 - 2));
	m_splineQueue.push(&splineArray[0]);

	printf("(%3.2f,%3.2f,%3.2f), (%3.2f,%3.2f,%3.2f), (%3.2f,%3.2f,%3.2f), (%3.2f,%3.2f,%3.2f)\n", 
		splineArray[0].m_startPoint.x, splineArray[0].m_startPoint.y, splineArray[0].m_startPoint.z, 
		splineArray[0].m_controlPoint1.x, splineArray[0].m_controlPoint1.y, splineArray[0].m_controlPoint1.z, 
		splineArray[0].m_controlPoint2.x, splineArray[0].m_controlPoint2.y, splineArray[0].m_controlPoint2.z, 
		splineArray[0].m_endPoint.x, splineArray[0].m_endPoint.y, splineArray[0].m_endPoint.z);


	for(int i = 1; i < numSplines; i++) {
		splineArray[i].GeneratePoints(splineArray[i-1].m_controlPoint2, splineArray[i-1].m_endPoint, splineArray[i-1].m_endPoint + vec3(rand() % 10 + 5, rand() % 4 - 2, rand() % 4 - 2));
		m_splineQueue.push(&splineArray[i]);

		printf("(%3.2f,%3.2f,%3.2f), (%3.2f,%3.2f,%3.2f), (%3.2f,%3.2f,%3.2f), (%3.2f,%3.2f,%3.2f)\n", 
			splineArray[i].m_startPoint.x, splineArray[i].m_startPoint.y, splineArray[i].m_startPoint.z, 
			splineArray[i].m_controlPoint1.x, splineArray[i].m_controlPoint1.y, splineArray[i].m_controlPoint1.z, 
			splineArray[i].m_controlPoint2.x, splineArray[i].m_controlPoint2.y, splineArray[i].m_controlPoint2.z, 
			splineArray[i].m_endPoint.x, splineArray[i].m_endPoint.y, splineArray[i].m_endPoint.z);
	}

	ConcatenateSplinePoints();
	CreateBezier();
	//m_bezierPoints = m_splinePoints;
	BuildMesh(m_bezierPoints.size(), RIBBON_COLUMNS, 1.0f);
	super::Initialize(10.0f);

	delete[] splineArray;

	return true;
}

void EndlessSpline::ConcatenateSplinePoints()
{
	m_splinePoints.clear();

	std::queue<Spline*> tempQueue = m_splineQueue;

	Spline splineToConcatenate = *tempQueue.front();
	tempQueue.pop();
	m_splinePoints.push_back(splineToConcatenate.m_startPoint);

	while(!tempQueue.empty()) {
		if(m_splinePoints.size() != 1) {
			splineToConcatenate = *tempQueue.front();
			tempQueue.pop();
		}
		m_splinePoints.push_back(splineToConcatenate.m_controlPoint1);
		m_splinePoints.push_back(splineToConcatenate.m_controlPoint2);
		m_splinePoints.push_back(splineToConcatenate.m_endPoint);
	} 
}

void EndlessSpline::CreateBezier()
{
	m_bezierPoints.clear();
	int itr = 0;
	int index = 0;
	while (itr < int(m_splinePoints.size()) - 4) {
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
			float t = static_cast<float>(i)/float(SPLINE_COMPLEXITY);

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

			vec3 color = vec3(1.0f, 0.4f, 0.1f);
		
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

	//restuff buffers with updated positions
    //glBindVertexArray(this->vertex_array_handle);
    //glBindBuffer(GL_ARRAY_BUFFER, this->vertex_coordinate_handle);
    //glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(VertexAttributesPCNT), &this->vertices[0], GL_STATIC_DRAW);

	this->CalculateNormals(size.x, size.y);
}

void EndlessSpline::Draw(const mat4 & projection, mat4 view, const ivec2 & size, Lights & lights, const float time)
{
	glEnable(GL_PROGRAM_POINT_SIZE); //enable to be able to adjust points in shaders
        if (this->GLReturnedError("Mesh::Draw - on entry"))
                return;
		
        glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
        mat4 model = glm::mat4(1.0f);
        //model = scale(model, vec3(10, 10, 10));
        mat4 mvp = projection * view * model;
        mat3 nm = inverse(transpose(mat3(view)));

        this->shaders[1]->Use(); //set shader to pure white for stars
        this->GLReturnedError("Mesh::Draw - after use");
        this->shaders[1]->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(view), value_ptr(mvp), value_ptr(nm));

        this->GLReturnedError("Mesh::Draw - after common setup");
        glBindVertexArray(this->vertex_array_handle);
        glPointSize(2); //change star point size
		glDrawElements(GL_TRIANGLES
			, this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);
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
