#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;

out Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj; // a new value!
} OUT;

void main(void) {
	vec3 scale = vec3(lightRadius);
	vec3 worldPos = (position*scale) + lightPos;
	gl_Position = (projMatrix * viewMatrix) * vec4(worldPos, 1.0);
}