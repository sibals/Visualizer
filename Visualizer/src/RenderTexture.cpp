#include "RenderTexture.h"

RenderTexture::RenderTexture() {
}

RenderTexture::~RenderTexture() {
}

void RenderTexture::StepPostEffect() {
	/*this->curr_shader_index = ++this->curr_shader_index % this->shaders.size();
	while(this->curr_shader_index < 6) {
		this->curr_shader_index = ++this->curr_shader_index % this->shaders.size();
	}*/
	if(this->curr_shader_index == 5) this->curr_shader_index = 6;
	else this->curr_shader_index = 5;
}

bool RenderTexture::Initialize() {
	this->BuildMesh(100, 100);
	super::Initialize(10.0f);
	curr_shader_index = 5;
	return true;
}

void RenderTexture::Draw(int textureID, int texture_id, const mat4 & projection, mat4 view, const ivec2 & size, Lights & lights,  const float time)
{
	if (this->GLReturnedError("Mesh::Draw - on entry"))
		return;

	glEnable(GL_DEPTH_TEST);
	mat4 model = glm::mat4(1.0f);
	mat4 mvp = projection * view * model;
	mat3 nm = inverse(transpose(mat3(view)));

	this->shaders[curr_shader_index]->Use();
	this->GLReturnedError("Mesh::Draw - after use");
	this->shaders[curr_shader_index]->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(view), value_ptr(mvp), value_ptr(nm));

	if(curr_shader_index == 5)
		this->post_normal.CustomSetup(textureID, texture_id);

	this->GLReturnedError("Mesh::Draw - after common setup");
	glBindVertexArray(this->vertex_array_handle);
	glDrawElements(GL_TRIANGLES , this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);
	glBindVertexArray(0);
	this->GLReturnedError("Mesh::Draw - after draw");
	glUseProgram(0);

	if (this->draw_normals)
	{
		this->solid_color.Use();
		this->solid_color.CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(view), value_ptr(mvp), value_ptr(nm));
		glBindVertexArray(this->normal_array_handle);
		glDrawElements(GL_LINES , this->normal_indices.size(), GL_UNSIGNED_INT , &this->normal_indices[0]);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	if (this->GLReturnedError("Mesh::Draw - on exit"))
		return;
}