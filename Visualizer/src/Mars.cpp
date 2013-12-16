#include "Mars.h"

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

bool Mars::Initialize(float size, char * mars_filename, char * mars_texture_filename)
{
	load_file(mars_filename);

	super::Initialize(size);

	//LoadTexture(mars_texture_filename);
	LoadTexture("assets/purple.jpg");

	return true;
}


void Mars::load_file(char * filename)
{
	const float scalar = 0.025f;	//scalar for mars depth data
	//Load file, error if cannot open
	ifstream file(filename);
	//float depth;

	int width; 
	int height; 
	if(!file.is_open()) {
		//cerr << "could not open mars.txt file" << endl;
		width = 100;
		height = 100;
	} else if (filename == "large") {
		width = 500;
		height = 500;
	} else {
		//Read first line with longitude and latitude information
		file >> width;
		file >> height;
		cout << "width: " << width << ", height: " << height << endl;
	}
	
	//Create Mesh
	this->BuildMesh(width, height);
	//Wrap Into Sphere
	this->BuildPrimitive(0.5f, width, height);


	/*//Modify Mesh Vertices
	int index = 0;
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			file >> depth;
			depth = 1.0f + depth * scalar;
			//assert(depth >= 0.0f && depth <= 2.0f);
			vertices[index].position *= depth;
			index++;
		}
		vertices[index-1].position = vertices[y * width].position;
	}*/
	
	this->CalculateNormals(width, height);
}

Mars::Mars(void)
{
}

Mars::~Mars(void)
{

}
