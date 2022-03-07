# version 400 core

uniform sampler2D mainTex;
uniform vec4 mainTex_ST;

uniform vec4 paintColor;
uniform vec3 paintPosition;
uniform float radius;
uniform float hardness;
uniform float strength;

in Vectex
{
	vec4 colour;
	vec2 texCoord;
	vec3 worldPosition;
}IN;

out vec4 fragColor;


void main(void)
{
	vec4 col = texture2D(mainTex,IN.texCoord);
	float m = distance(paintPosition,IN.worldPosition);
	float value_temp = 1- smoothstep(radius * hardness, radius,m);
	float edge = value_temp * strength;
	fragColor = mix(col,paintColor,edge);	
	//fragColor = col + (paintColor - col) * edge;
}