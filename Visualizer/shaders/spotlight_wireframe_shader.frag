/*
Code taken from GLSL 4.0 Cookboook for Wire shader, Chapter 6
Also based on Spotlight shader
https://github.com/daw42/glslcookbook/blob/master/chapter06/shader/shadewire.fs
*/

#version 400

in vec3 Gposition;
in vec3 Gnormal;
flat in vec3 Gcolor;
in vec2 Gtexture;
noperspective in vec3 GEdgeDistance;

uniform sampler2D s_texture;

layout( location = 0 ) out vec4 FragColor;


void main() {

	// Find the smallest edge distance
    float d = min( GEdgeDistance.x, GEdgeDistance.y );
    d = min( d, GEdgeDistance.z );

	float mixVal;
	float lineWidth = 1.0;
    if( d < lineWidth - 1 ) {
        mixVal = 1.0;
    } else if( d > lineWidth + 1 ) {
        mixVal = 0.0;
    } else {
        float x = d - (lineWidth - 1);
        mixVal = exp2(-2.0 * (x*x));
    }
    

    vec4 t_color = texture2D(s_texture, Gtexture);
	vec4 line_color = t_color;
	t_color.b = t_color.b * 0.25;
	t_color.w = 0.4;

	vec4 lineC = line_color * 4;
	lineC.w = 0.5;
	FragColor = mix(t_color, lineC, mixVal); 
}