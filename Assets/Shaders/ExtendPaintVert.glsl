#version 400 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform sampler2D MainTex;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

out Vertex{
	vec2 texCoord;
	vec4 position;
}OUT;


void main(void)
{
	mat4 mv = viewMatrix * modelMatrix;
	OUT.position = mv * vec4(position ,1);
	OUT.texCoord = texCoord;
	gl_Position = OUT.position;
}