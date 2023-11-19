#version 330 core

uniform sampler2D diffuseTex;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)	{
	vec4 texColour = texture(diffuseTex, IN.texCoord);
	float grey = (texColour.r + texColour.g + texColour.b) / 3;

	float noiseModifier = 0.3;

	float noise = (fract(sin(dot(gl_FragCoord.xyz, vec3(19, 725, 110))) * 2500) -0.5 ) * noiseModifier;
	grey += noise;

	float purpleThreshold = 0.4;
	float purpleDist = distance(vec3(0.5, 0.0, 0.5), texColour.rgb);

	if (purpleDist > purpleThreshold)
	{
		fragColour = vec4(grey, grey, grey, 1);
	}
	else
	{
		fragColour = texColour;
	}
}
