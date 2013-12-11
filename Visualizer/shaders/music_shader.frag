//Taken from Perry Kivolowitz's Modern GL Example

#version 400

layout (location = 0) out vec4 FragColor;

flat in vec3 color;
in vec3 position;
in vec3 normal;
in float m_time;

uniform float shininess = 100.0f;
const vec3 light_position = vec3(10.0, 0.0, 10.0);

vec3 ads( )
{
  vec3 n = normal;

  if (!gl_FrontFacing)
	n = -n;

  vec3 s = normalize(light_position - position);
  vec3 v = normalize(-position);
  vec3 r = reflect(-s, n);
  float s_dot_n = max(dot(s, n), 0.0);
  
  float red = color.x + (m_time * 2);
  float g = color.y;
  float b = color.z;

  vec3 m_color = vec3(red, g, b);

  /*if (m_time != 0.0) {
	m_color = vec3(1.0, 0.0, 0.0);
  }*/

  return m_color * s_dot_n + (s_dot_n > 0 ? color * pow(max(dot(r, v), 0.0), shininess) : vec3(0.0));
}

void main()
{
	FragColor = vec4(ads(), 1.0);
}
