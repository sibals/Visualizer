#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "Camera.h"
#include "background.h"
#include "Mars.h"
#include "Light.h"
#include "Starfield.h"
#include "RenderTexture.h"
#include "Cylinder.h"
#include "Framebuffer.h"

class Window
{
public:
	Window();
	~Window();

	float time_last_pause_began;
	float total_time_paused;
	bool paused , wireframe, normals;
	int window_handle;
	int interval;
	int slices;
	glm::ivec2 size;
	float window_aspect;
	std::vector<std::string> instructions;
	int mode;
	int debugTextureID;
	float deltaTime;
	float lastFrameTime;
	float splineRotation;
	float beatDetectorRotation;
	float beatDetectorRotationSpeed;
	float speedModifier;

	Background background;
	Mesh mesh;
	Mars mars;
	Camera camera;
	Lights lights;
	Starfield starfield;
	RenderTexture rendertexture;
	Framebuffer frame_buffer;
	Framebuffer fboSecondary;
	Mars sphere;

	glm::mat4 lightPV;

	bool draw_planes;		//should we draw x,y,z axis or not?
	void drawPlanes();
};