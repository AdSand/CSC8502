#version 330 core

uniform sampler2D diffuseTex;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)	{
	vec4 colour = texture(diffuseTex, IN.texCoord);
	float purpleThreshold = 0.4;
	float purpleDist = distance(vec3(0.5, 0.0, 0.5), colour.rgb);

	if (purpleDist > purpleThreshold)
	{
		float average = (colour.r + colour.g + colour.b) /3;
		fragColour = vec4(average, average, average, colour.a);
	}
	else
	{
		fragColour = colour;
	}
}
