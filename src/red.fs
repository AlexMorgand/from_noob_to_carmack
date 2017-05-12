#version 330

out vec4 FragColor;
// Variable from CPU (OpenGL implementation).
uniform vec4 openGLColor;

void main()
{
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	FragColor = openGLColor;
}
