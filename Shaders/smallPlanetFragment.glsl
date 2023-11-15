#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex2;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)	{
	float randomNum = fract(sin(2238964 * 126391263));
	if (randomNum < 0.5)
	{
		fragColour = texture(diffuseTex, IN.texCoord);
	}
	else
	{
		fragColour = texture(diffuseTex2, IN.texCoord);
	}
}
