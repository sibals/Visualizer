#version 400

layout (location = 0) out vec4 FragColor;

// http://glsl.heroku.com/e#6541.0
//
// @paulofalcao
//
// Blue Pattern
//
// A old shader i had lying around
// Although it's really simple, I like the effect :)
// modified by @hintz

#ifdef GL_ES
precision highp float;
#endif

uniform float time;
uniform ivec2 size;

void main(void)
{
	//	modification to adapt to current example
	vec2 resolution = vec2(size);
	vec2 u=(gl_FragCoord.xy/resolution.x)*2.0-vec2(1.0,resolution.y/resolution.x);
	float t=time*0.5;
	
	float tt=sin(t/8.0)*64.0;
	float x=u.x*tt+sin(t*2.1)*4.0;
	float y=u.y*tt+cos(t*2.3)*4.0;
	float c=sin(x)+sin(y);
	float zoom=sin(t);
	x=x*zoom*2.0+sin(t*1.1);
	y=y*zoom*2.0+cos(t*1.3);
	float xx=cos(t*0.7)*x-sin(t*0.7)*y;
	float yy=sin(t*0.7)*x+cos(t*0.7)*y;
	c=(sin(c+(sin(xx)+sin(yy)))+1.0)*0.4;
	float v=2.0-length(u)*2.0;
	//	Modification to adapt to current example
	FragColor=vec4(v*vec3(c+v*0.4,c*c-0.5+v*0.5,c*1.9),1.0);
}