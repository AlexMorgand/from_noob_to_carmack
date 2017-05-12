#version 330

layout (location = 0) in vec3 Position;
out vec4 outputColor;

void main()
{
    gl_Position = vec4(Position.x, Position.y, Position.z, 1.0);
	outputColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}
