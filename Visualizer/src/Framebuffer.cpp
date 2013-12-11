#include "Framebuffer.h"

#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>

using namespace std;

Framebuffer::Framebuffer(void)
{
}
Framebuffer::~Framebuffer(void)
{
}

void Framebuffer::Use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
}

void Framebuffer::Disable()
{
	//Unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// adapted from GLSL cookbook
bool Framebuffer::Initialize(int width, int height, int textureID)
{
	GLuint depthBuf, posTex;
	this->width = width;
	this->height = height;

    // Create and bind the FBO
	glGenFramebuffers(1, &renderFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

    // The depth buffer
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    // 
    glActiveTexture(GL_TEXTURE0 + textureID);   // bind to texture number passed in
    glGenTextures(1, &posTex);
    glBindTexture(GL_TEXTURE_2D, posTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex, 0);

	GLenum drawBuffers[] = {GL_NONE, GL_COLOR_ATTACHMENT0};
    glDrawBuffers(2, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}