#version 400

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D RenderTex;
uniform sampler2D tex02;

layout( location = 0 ) out vec4 FragColor;

const float blurSizeH = 1.0 / 500.0;
const float blurSizeV = 1.0 / 500.0;

const float blurSizeH2 = 1.0f/ 824.0;
const float blurSizeV2 = 1.0f/ 624.0;

void main() {
    //vec4 texColor = texture( RenderTex, TexCoord);
    //FragColor = texColor;
	//vec4 sum2 = texture(tex02, TexCoord);

    vec4 sum = vec4(0.0);
    for (int x = -4; x <= 4; x++)
        for (int y = -4; y <= 4; y++)
            sum += texture(
                RenderTex,
                vec2(TexCoord.x + x * blurSizeH, TexCoord.y + y * blurSizeV)
            ) / 300.0;

	vec4 sum2 = vec4(0.0);
	for (int x = -4; x <= 4; x++)
		for (int y = -4; y <= 4; y++)
			sum2 += texture(
				tex02,
				vec2(TexCoord.x + x * blurSizeH2, TexCoord.y + y * blurSizeV2)
				) / 81.0;
	
	sum = sum * 5.0;
	//sum = sum * sum2;
	sum2 = sum2 * 20.0;
    FragColor = mix(sum, sum2, .05);
}