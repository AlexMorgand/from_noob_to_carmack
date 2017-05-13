#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

uniform float h_offset_ud;
uniform float h_offset_lr;

out vec3 ourColor;
out vec2 TexCoord;
out vec4 outputColor;

void main()
{
	vec4 pos = vec4(Position.x + h_offset_lr, Position.y + h_offset_ud, Position.z, 1.0);
    gl_Position = pos;
	outputColor = pos;
	
	ourColor = color;
	TexCoord = texCoord;
	
	
}