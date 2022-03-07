# version 400 core

uniform sampler2D mainTex;
uniform	sampler2D _UVIslands;


in Vectex
{
	vec2 texCoord;
	vec3 position;
}IN;

out vec4 fragColor;


void main(void)
{
	vec4 color = texture2D(mainTex,IN.texCoord);
	vec4 island = texture2D(_UVIslands,IN.texCoord);
	fragColor = color;
}