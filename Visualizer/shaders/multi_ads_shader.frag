//Taken from Perry Kivolowitz's Modern GL Example

#version 400

layout (location = 0) out vec4 FragColor;

flat in vec3 color;
in vec3 position;
in vec3 normal;

uniform float shininess = 100.0f;
const vec3 light_position_one = vec3(10.0, 15.0, 0.0);
const vec3 light_position_two = vec3(-20.0, 0.0, 0.0);

vec3 ads( )
{
  vec3 n = normal;

  if (!gl_FrontFacing)
	n = -n;

  // Variables named as such because compiler would not have it when overwriting same variables

  vec3 s_1 = normalize(light_position_one - position);
  vec3 v_1 = normalize(-position);
  vec3 r_1 = reflect(-s_1, n);
  float s_dot_n_1 = max(dot(s_1, n), 0.0);

  vec3 color_one = color * s_dot_n_1 + (s_dot_n_1 > 0 ? color * pow(max(dot(r_1, v_1), 0.0), shininess) : vec3(0.0));

  vec3 s_2 = normalize(light_position_two - position);
  vec3 v_2 = normalize(-position);
  vec3 r_2 = reflect(-s_2, n);
  float s_dot_n_2 = max(dot(s_2, n), 0.0);

  vec3 color_two = color * s_dot_n_2 + (s_dot_n_2 > 0 ? color * pow(max(dot(r_2, v_2), 0.0), shininess) : vec3(0.0));

  return (color_one + color_two) / 2.0;
}

void main()
{
	FragColor = vec4(ads(), 1.0);
}
