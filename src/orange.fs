#version 330

out vec4 color;

// Variable from CPU (OpenGL implementation).
uniform vec4 openGLColor;
in vec3 ourColor;
in vec2 TexCoord;

in vec4 outputColor;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{
    //FragColor = vec4(1.0, 0.5, 0.0, 1.0);
    //FragColor = openGLColor;
	//color = outputColor;
	color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);
}
