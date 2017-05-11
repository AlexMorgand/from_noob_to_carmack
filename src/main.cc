// /!\ glew should be declared before freeglut/glfw/sdnl ...
#define GLEW_STATIC
// Handle the different opengl functions.
#include <GL/glew.h>
// Window and input handling library.
#include <GLFW/glfw3.h>

// C++ libraries.
#include <iostream>
#include <fstream>
#include <list>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true,
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		std::cout << "Application is closing now." << std::endl;
	}
	else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		GLint polygonMode;
		glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
		if (polygonMode == GL_LINE)
		{
			std::cout << "Switching to wireframe" << std::endl;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
		{
			std::cout << "Switching to faces" << std::endl;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
	}
}

void create_scene(GLuint* VBO, GLuint* VAO, GLuint* EBO)
{
	// Set up vertex data (and buffer(s)) and attribute pointers.
    GLfloat vertices[] = {-0.5f, 0.5f, 0.0f,
						  0.5f,  0.5f, 0.0f, 
						  0.5f,  -0.5f, 0.0f,
						  -0.5f,  -0.5f, 0.0f};

	GLuint indices[] = {0, 1, 2,
		                2, 3, 0};

	

	// Create one buffer object (our vertex buffer object).
    glGenBuffers(1, VBO);
	
	// Create one array object (our vertex array object).
	// Our vertex array object will keep track (and store) every changes in the context caused by the VBO.
	// To do all the drawing, we just have to bind the VAO and it's done.
    glGenVertexArrays(1, VAO);

	// To avoid duplication of the vertices, we will use indices to draw our triangles.
	// To do so, we use an element object buffer. Every changes made the the EOB are also recorded in the VAO.
	glGenBuffers(1, EBO);


	// FIXME: move it above no ?
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);

	// Fill the type array buffer with our vertices. Static data since we do not want it to move.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	

	

	// Unbind VAO by setting it to a null pointer 
	// (it's always a good thing to unbind any buffer/array to prevent strange bugs).
    glBindVertexArray(0);


}

//FIXME: return an error code here too.
void compile_shader(GLuint* shaderProgram, std::list<std::pair<std::string, GLenum >> shader_code_l)
{
	GLint success;
	GLchar infoLog[512];
    *shaderProgram = glCreateProgram();
    // Build and compile our shader program

	for (auto elt = shader_code_l.begin(); elt != shader_code_l.end(); ++elt)
	{
		const GLchar* tmp = elt->first.c_str();
		GLenum e = elt->second;

		GLuint shader = glCreateShader(e);
		glShaderSource(shader, 1, &tmp, nullptr);
		glCompileShader(shader);
		// Check for compile time errors
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		
		// Link shader.
		glAttachShader(*shaderProgram, shader);
		glDeleteShader(shader);
	}

    // Link shaders to the main program.
    glLinkProgram(*shaderProgram);
    // Check for linking errors.
    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
	// Since it's linked, free the shaders.
    /*glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);*/
}

// FIXME: not a very C++ thing to do here.
int init_glfw(GLFWwindow** w)
{
	// See documentation. Call this before any OpenGL function.
	glewExperimental = GL_TRUE;

	glfwInit();

	// We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// We choose to enable resizable window.
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	*w = glfwCreateWindow(800, 600, "Carmack Rendering", nullptr, nullptr);
	if (w == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// The current display is this window.
	glfwMakeContextCurrent(*w);
	// Link the function for the key listening.
	glfwSetKeyCallback(*w, key_callback);
	return 0;
}

int init_glew()
{	
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	glViewport(0, 0, 800, 600);
	return 0;
}

std::list<std::pair<std::string, GLenum>> load_shader_file(std::list<std::pair<std::string, GLenum>> shader_l)
{
	std::list<std::pair<std::string, GLenum>> shader_code_l;
	for (auto elt = shader_l.begin(); elt != shader_l.end(); ++elt)
	{
		std::ifstream file;
		file.open(elt->first, std::ifstream::in);

		std::string str((std::istreambuf_iterator<char>(file)),
						 std::istreambuf_iterator<char>());
		shader_code_l.push_back(make_pair(str, elt->second));
	}
	return shader_code_l;
}


int main(int argc, char* argv[])
{
	// This is our vertex buffer object.
	GLuint VBO;
	// This is our vertex array object.
	GLuint VAO;
	// This is our element buffer object.
	GLuint EBO;

	// Init GLFW.
	int error_code = 0;
	GLFWwindow* window = nullptr;
	error_code = init_glfw(&window);
	if (error_code)
		return error_code;

	error_code = init_glew();
	if (error_code)
		return error_code;
	
	std::list<std::pair<std::string, GLenum >> shader_l;
	std::string fs("C:/Users/am237982/Desktop/AlexFormation/openGL/src/shader.fs");
	std::string vs("C:/Users/am237982/Desktop/AlexFormation/openGL/src/shader.vs");
	shader_l.push_back(make_pair(fs, GL_FRAGMENT_SHADER));
	shader_l.push_back(make_pair(vs, GL_VERTEX_SHADER));

	std::list<std::pair<std::string, GLenum >> shader_code_l = load_shader_file(shader_l);

	GLuint shaderProgram;
	// Handle the shaders, load them and bind them.
	compile_shader(&shaderProgram, shader_code_l);

	// Create the scene and init the context (VBO, ...).
	create_scene(&VBO, &VAO, &EBO);

	// Main loop.
	while(!glfwWindowShouldClose(window))
	{
		// Listen to events, inputs and window interactions.
		glfwPollEvents();

		// Rendering.
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		// FIXME: the usage of the VAO is not clear to me.
		// Do we use a VAO to avoid some cumbersome procedure ? (declare the vertex in the VBO and release it ?).
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swap the buffer to avoid the flickering.
		glfwSwapBuffers(window);
	}
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return error_code;
}

