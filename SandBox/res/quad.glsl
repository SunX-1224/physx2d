#version 460 core

out vec4 frag_color;

in Vert_Out{
	vec2 position;
	vec2 size;
	vec4 color;
}vs_out;


uniform float u_time;
uniform vec2 u_resolution;



void main(){
	frag_color = vs_out.color;
}