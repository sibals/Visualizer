#pragma once

class Framebuffer
{
public:
	unsigned int renderFBO; // The handle to the FBO
	unsigned int renderTex; // 
	unsigned int depthBuf; //
	unsigned int intermediateFBO;
	unsigned int intermediateTex;

	int width, height;


	Framebuffer(void);
	~Framebuffer(void);

	bool Initialize(int width, int height, int textureID);
	void Use();
	void Disable();
	
};

