#version 460 core

#define WIDTH 600

out vec4 fragColor;

in vec2 texUV;

uniform sampler2D frame1;
uniform sampler2D frame2;

float offset = 1.0f/WIDTH;

vec2 offsets[9] = {
	vec2(-offset, offset),
	vec2(0.0f, offset),
	vec2(offset, offset),
	vec2(-offset, 0.0f),
	vec2(0.0f, 0.0f),
	vec2(offset, 0.0f),
	vec2(-offset, -offset),
	vec2(0.0f, -offset),
	vec2(offset, -offset)
};

float kernel[9] = {
    1,  1,  1,
    1,  4,  1,
    1,  1,  1	
};

void main(){
	float d = 0.9f;

	//vec3 color = vec3(0.0f);
	//for(int i=0; i < 9; i++){
		//color += vec3(texture(frame1, texUV + offsets[i]) * d + texture(frame2, texUV + offsets[i]) *(1.0f - d)) * kernel[i] / 12.f;
	//}
	//fragColor = vec4(color, 1.0f);
	
	//fragColor = texture(frame1, texUV) * d + texture(frame2, texUV) * (1.f - d);

	fragColor = texture(frame1, texUV);
}
