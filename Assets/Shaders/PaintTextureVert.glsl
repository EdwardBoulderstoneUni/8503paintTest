#version 400 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;



layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;

out Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 worldPosition;
}OUT;


void main(void)
{

	OUT.worldPosition = (modelMatrix * vec4(position,1.0f)).xyz;
	OUT.texCoord = texCoord;
	vec4 temp = vec4(0,0,0,1);
	temp.xy = vec2(1,1) * (OUT.texCoord.xy * 2 -1);
	gl_Position	= temp;
}