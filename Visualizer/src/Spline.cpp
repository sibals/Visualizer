#include "Spline.h"
#include <glm/gtx/spline.hpp>

using namespace glm;

static const int SPLINE_COMPLEXITY = 10;
static const int RIBBON_COLUMNS = 2;

Spline::Spline() 
{
}

Spline::~Spline() 
{
}

bool Spline::Initialize(int length, glm::vec3 startPos, glm::vec3 endPos) 
{
	CreatePoints(length, startPos, endPos);	
	CreateBezier(SPLINE_COMPLEXITY);

	m_splineModificationIndex = 0;
	ivec2 size = ivec2(m_numBezierPoints, RIBBON_COLUMNS);
	//Create Mesh
    this->BuildMesh(size.x, size.y);
        
	vec3 startColor = vec3(1.0f, 1.0f, 1.0f);
    //Modify Mesh Vertices
    int index = 0;
    for(int y = 0; y < size.y; y++) {
        for(int x = 0; x < size.x; x++) {

			vec3 pos = m_bezierPoints[x];

			//pos.y += y;
			vertices[index].position = pos;
			vertices[index].color = startColor;
			startColor.r -= (1 / size.x);
            index++;
        }
    }

	for(int i = 0; i < m_numBezierPoints; i++) {

		m_randomOffsets.push_back(normalize(vec3(rand(), rand(), rand())));
	}

    this->CalculateNormals(size.x, size.y);
    super::Initialize(10.0f);

	return true;
}

void Spline::TakeDown() 
{
	super::TakeDown();
}

// general algorithm: http://www.cubic.org/docs/bezier.htm && http://gamedev.stackexchange.com/questions/18615/how-do-i-linearly-interpolate-between-two-vectors
vec3 Spline::Lerp(vec3 dest, vec3 a, vec3 b, float t)
{
	dest = a * t + b * (1.0f - t);
	return dest;
}

vec3 Spline::Bezier(vec3 dest, vec3 a, vec3 b, vec3 c, vec3 d, float t)
{
	this->m_timer = 0.0f;
    vec3 ab, bc, cd, abbc, bccd;
    ab = Lerp(ab, a, b, t);           
    bc = Lerp(bc, b, c, t);           
    cd = Lerp(cd, c, d, t);          
    abbc = Lerp(abbc, ab, bc, t);     
    bccd = Lerp(bccd, bc, cd, t);     
    dest = Lerp(dest, abbc, bccd, t);   
	return dest;
}

void Spline::CreateBezier(int detail) 
{
	m_numBezierPoints = 0;
	m_bezierPoints.clear();
	int itr = 0;
	int index = m_headPointer;
	while (itr < m_numPoints - 6) {
		vec3 a = m_splinePoints[index];
		index = (index + 1) % m_numPoints;
		vec3 b = m_splinePoints[index];
		index = (index + 1) % m_numPoints;
		vec3 c = m_splinePoints[index];
		index = (index + 1) % m_numPoints;
		vec3 d = m_splinePoints[index];
		index = (index - 2) % (m_numPoints + 1);
		//if(index < 0) {
		//	index = m_numPoints - index;
		//}
		//if (index + itr >= m_numPoints - 6) {
		//	index = 0;
		//}
		itr++;

		for (int i = 0; i < detail; i++) {
			vec3 p;
			float t = static_cast<float>(i)/detail;
			//p = Bezier(p, a, b, c, d, t);

			p = catmullRom(a, b, c, d, t);

			m_bezierPoints.push_back(p);
		}
	}
	m_numBezierPoints = m_bezierPoints.size();
}


void Spline::CreatePoints(int length, glm::vec3 startPos, glm::vec3 endPos)
{
	/* Create a vector of a set size that will hold all the control points for
	*  our bezier spline at the end. After placing the start and end vectors in, 
	*  run an algorithm that adds points in between - adding (n - 1) points every time
	*  where n is the current number of points in the vector.
	*/
	
	std::vector<glm::vec3> initialPoints;
	
	int trueLen = 2;

	for (int i = 0; i < length; i++) {
		trueLen = (trueLen - 1) + trueLen;
	}
	length = trueLen;

	// Fill an "empty" vector with the number of elements we want
	for(int i = 0; i < length; i++) {
		initialPoints.push_back(vec3(0, 0, 0));
	}

	// And assign the first and last elements
	initialPoints[0] = startPos;
	initialPoints[length - 1] = endPos;

	int numPts = 2;
	int currIteration;
	int currPt = 0;

	
	
	while (numPts < length) {
		currIteration = (length - 1) / (numPts - 1);
		while (currPt < length - 1) {
			int pointOne = currPt;
			currPt += currIteration;
			int pointTwo = currPt;
			if(pointTwo > length - 1) break;

			vec3 newPoint = (initialPoints[pointOne] + initialPoints[pointTwo]) / 2.0f;
			vec3 randVec = vec3(float(rand()) / float(RAND_MAX) * 10 - 5, float(rand()) / float(RAND_MAX) * 10 - 5, float(rand()) / float(RAND_MAX) * 10 - 5);
			//vec3 randVec = vec3(0.0f, 0.0f, 0.0f);
			//printf("(%f, %f, %f)\n", randVec.x, randVec.y, randVec.z);

			// Move the new point randomly within a small range
			newPoint += randVec;

			initialPoints[(pointOne + pointTwo) / 2] = newPoint;
			numPts++;
		}
		currPt = 0;
	}

	for(int i = 0; i < length; i++) {
		//printf("(%f, %f, %f)\n", initialPoints[i].x, initialPoints[i].y, initialPoints[i].z);  
	}

	for(int i = 0; i < length; i++) {
		m_splinePoints.push_back(initialPoints[i]);  
	}

	m_numPoints = m_splinePoints.size();
	m_headPointer = 0;
	m_tailPointer = m_splinePoints.size() - 1;

}

void Spline::Update(float deltaTime) 
{
		m_timer += deltaTime;
		if (m_timer < 0.02f) {
		//	return;
		}

		//reset the timer if the update is up
		m_timer = 0.0f;

		// Used for moving head to tail of spline vector
		/*int pointer = m_headPointer;
		for (int i = 0; i < 4; i++) {
			
			m_headPointer = (m_headPointer + 1) % m_numPoints;
			m_tailPointer = (m_tailPointer + 1) % m_numPoints;
			m_splinePoints[m_tailPointer] = m_splinePoints[m_tailPointer] - 1.0f;
		}*/

		//for (int i = 0; i < m_numPoints; i++) {
		//m_splinePoints[m_splineModificationIndex].z += float(rand()) / float(RAND_MAX) * 2 - 1;
		//m_splinePoints[m_splineModificationIndex].y = float(rand()) / float(RAND_MAX) * 2 - 1;
		//m_splinePoints[m_splineModificationIndex].x += float(rand()) / float(RAND_MAX) * 2 - 1;
		//}
		
		//reparametrize the curve
		CreateBezier(SPLINE_COMPLEXITY);

		ivec2 size = ivec2(m_numBezierPoints, RIBBON_COLUMNS);

		//update vertices with new positions from curve
		int index = 0;
		for(int y = 0; y < size.y; y++) {
			for(int x = 0; x < size.x; x++) {
				vec3 pos = m_bezierPoints[x];
				//pos.y += m_randomOffsets[x].y;
				pos.y += y;
				vertices[index].position = pos;
				index++;
			}
		}

		m_splineModificationIndex = (m_splineModificationIndex + 1) % m_numPoints;

		//restuff buffers with updated positions
        glBindVertexArray(this->vertex_array_handle);
        glBindBuffer(GL_ARRAY_BUFFER, this->vertex_coordinate_handle);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(VertexAttributesPCNT), &this->vertices[0], GL_STATIC_DRAW);
}

void Spline::Draw(const mat4 & projection, mat4 view, const ivec2 & size, Lights & lights, const float time)
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
		glDrawElements(GL_TRIANGLES, this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);
        glBindVertexArray(0);
        this->GLReturnedError("Mesh::Draw - after draw");

		glEnable(GL_CULL_FACE);

        if (this->GLReturnedError("Mesh::Draw - on exit"))
                return;
}

void Spline::BuildMesh(int columns, int rows)
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
}

