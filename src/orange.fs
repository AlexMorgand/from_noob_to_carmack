#version 330

out vec4 color;

// Variable from CPU (OpenGL implementation).
uniform vec4 openGLColor;
in vec4 ourColor;
in vec2 TexCoord;

void main()
{
    //FragColor = vec4(1.0, 0.5, 0.0, 1.0);
    //FragColor = openGLColor;
	//color = outputColor;
	color = texture(ourTexture, TexCoord);
}
