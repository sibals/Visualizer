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


void main( void ) 
{
  vec2 resolution = size;

  vec2 uPos = ( gl_FragCoord.xy / resolution.xy );

  uPos.x -= 10.5;
  uPos.y -= -0.0;

  vec3 color = vec3(0.0);

  float vertColor = 0.0;

  for( float i = 0.0; i < 10.0; ++i )
  {
    float t = time * (0.75);
  
    uPos.y -= ( (sin(cos( uPos.y*(i+2.0) + t+i/2.0)) * 0.2) - (tan(cos( uPos.y*(i+0.25) + t+i/0.25)) * 0.15));
    uPos.x += ( (tan(cos( uPos.x*(i+2.0) + t+i/2.0)) * 0.2) - (sin(cos( uPos.y*(i+0.25) + t+i/0.25)) * 0.15));

    uPos.y -= ( (sin(cos( uPos.y*(i+2.0) + t+i/2.0)) * 0.2) - (tan(cos( uPos.y*(i+0.25) + t+i/0.25)) * 0.15));
    uPos.x += ( (tan(cos( uPos.x*(i+2.0) + t+i/2.0)) * 0.2) - (sin(tan( uPos.y*(i+0.25) + t+i/0.25)) * 0.15));

    float fTemp = abs(1.0 / (uPos.y * uPos.x) / 800.0);

    vertColor *= fTemp;

    color += vec3( 0.005, sin(fTemp*(1.0-i)/10.0), fTemp*i/55.0 );

	vec2 mouse = vec2(1, 1);
    uPos = ((gl_FragCoord.xy - (resolution * 0.5)) / min(resolution.y,resolution.x) * 1.0) * mouse.xy * 10.0;
  }
  
  vec4 color_final = vec4(color, 1.0);

  gl_FragColor = color_final;

}