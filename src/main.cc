// /!\ glew should be declared before freeglut/glfw/sdnl ...
#define GLEW_STATIC
// Handle the different opengl functions.
#include <GL/glew.h>
// Window and input handling library.
#include <GLFW/glfw3.h>

// Import texture libray.
#include <SOIL.h>

// C++ libraries.
#include <iostream>
#include <fstream>
#include <list>


#include "shader_manager.hh"

GLfloat h_offset_ud = 0.0f;
GLfloat h_offset_lr = 0.0f;


GLuint texture;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true,
	// closing the application
	if ((key == GLFW_KEY_ESCAPE  || key == GLFW_KEY_A) && action == GLFW_PRESS)
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
	else if (key == GLFW_KEY_UP)
	{
		h_offset_ud += 0.01f;
	}
	else if (key == GLFW_KEY_DOWN)
	{
		h_offset_ud -= 0.01f;
	}
	else if (key == GLFW_KEY_LEFT)
	{
		h_offset_lr -= 0.01f;
	}
	else if (key == GLFW_KEY_RIGHT)
	{
		h_offset_lr += 0.01f;
	}
}

void create_scene(std::list<GLuint>* VBO_l, std::list<GLuint>* VAO_l, std::list<GLuint>* EBO_l)
{
	// Set up vertex data (and buffer(s)) and attribute pointers.
    //GLfloat vertices[] = {-0.5f, 0.5f, 0.0f,
				//		  0.5f,  0.5f, 0.0f, 
				//		  0.5f,  -0.5f, 0.0f,
				//		  -0.5f,  -0.5f, 0.0f};
    //GLfloat vertices[] = {-0.5f, -0.5f, 0.0f,
				//		  0.5f,  -0.5f, 0.0f, 
				//		  0.0f,  0.5f, 0.0f};
	
	// Positions // Colors // Texture Coords
	GLfloat vertices[] = { 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Top Right
						   0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
						   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom Left
						   -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f }; // Top Left


	// FIXME: for test purposes.
	//GLuint indices[2][3] = {{0, 1, 2}, {2, 3, 0}};
	GLuint indices[1][6] = {{0, 1, 3, 
		                     1, 2, 3}};

	// FIXME: can we do better?
	auto elt_vbo = VBO_l->begin();
	auto elt_vao = VAO_l->begin();
	auto elt_ebo = EBO_l->begin();
	unsigned int i = 0;
	for (; elt_vbo != VBO_l->end(); ++elt_vbo, ++elt_vao, ++elt_ebo)
	{
		// Create one buffer object (our vertex buffer object).
		glGenBuffers(1, &(*elt_vbo));

		// Create one array object (our vertex array object).
		// Our vertex array object will keep track (and store) every changes in the context caused by the VBO.
		// To do all the drawing, we just have to bind the VAO and it's done.
		glGenVertexArrays(1, &((*elt_vao)));

		// To avoid duplication of the vertices, we will use indices to draw our triangles.
		// To do so, we use an element object buffer. Every changes made the the EOB are also recorded in the VAO.
		glGenBuffers(1, &((*elt_ebo)));

		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(*elt_vao);
		glBindBuffer(GL_ARRAY_BUFFER, *elt_vbo);

		// Fill the type array buffer with our vertices. Static data since we do not want it to move.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *elt_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[i]), indices[i], GL_STATIC_DRAW);
	

		// Unbind VAO by setting it to a null pointer 
		// (it's always a good thing to unbind any buffer/array to prevent strange bugs).
		glBindVertexArray(0);

		++i;
	}

	// FIXME: maybe use OpenCV ?
	int width, height;
	unsigned char* image = SOIL_load_image("C:/Users/am237982/Desktop/AlexFormation/openGL/src/container.jpg", &width, &height, 0,	SOIL_LOAD_RGB);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Free and unbinding.
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
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

int main(int argc, char* argv[])
{
	// This is our vertex buffer objects.
	GLuint VBO1, VBO2;
	// This is our vertex array object.
	GLuint VAO1, VAO2;
	// This is our element buffer objects.
	GLuint EBO1, EBO2;

	std::list<GLuint> VAO_l;
	VAO_l.push_back(VAO1);
	//VAO_l.push_back(VAO2);

	std::list<GLuint> EBO_l;
	EBO_l.push_back(EBO1);
	//EBO_l.push_back(EBO2);

	std::list<GLuint> VBO_l;
	VBO_l.push_back(VBO1);
	//VBO_l.push_back(VBO2);

	// Init GLFW.
	int error_code = 0;
	GLFWwindow* window = nullptr;
	error_code = init_glfw(&window);
	if (error_code)
		return error_code;

	error_code = init_glew();
	if (error_code)
		return error_code;
	
	ShaderManager red("C:/Users/am237982/Desktop/AlexFormation/openGL/src/shader.vs",
		       "C:/Users/am237982/Desktop/AlexFormation/openGL/src/red.fs");
	//ShaderManager orange("C:/Users/am237982/Desktop/AlexFormation/openGL/src/shader.vs",
	//	          "C:/Users/am237982/Desktop/AlexFormation/openGL/src/orange.fs");

	std::list<ShaderManager> shader_program_l;
	shader_program_l.push_back(red);
	//shader_program_l.push_back(orange);

	// Create the scene and init the context (VBO, ...).
	create_scene(&VBO_l, &VAO_l, &EBO_l);

	// Main loop.
	while(!glfwWindowShouldClose(window))
	{
		// Listen to events, inputs and window interactions.
		glfwPollEvents();

		// Rendering.
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		auto elt_shader = shader_program_l.begin();
		for (auto elt_vao = VAO_l.begin(); elt_vao != VAO_l.end(); ++elt_vao, ++ elt_shader)
		{

			glBindTexture(GL_TEXTURE_2D, texture);
			
			// We need to have an active shader before setting (not getting) the uniform (global) variable.
			elt_shader->use();

			GLint h_offset_ud_loc = glGetUniformLocation(elt_shader->get_program(), "h_offset_ud");
			GLint h_offset_lr_loc = glGetUniformLocation(elt_shader->get_program(), "h_offset_lr");
			glUniform1f(h_offset_ud_loc, h_offset_ud);
			glUniform1f(h_offset_lr_loc, h_offset_lr);
			glBindVertexArray(*elt_vao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		// Swap the buffer to avoid the flickering.
		glfwSwapBuffers(window);
	}

	// Clearing.
	auto elt_vao = VAO_l.begin();
	auto elt_vbo = VBO_l.begin();
	auto elt_ebo = EBO_l.begin();
	for (; elt_vao != VAO_l.end(); ++elt_vao, ++elt_vbo, ++elt_ebo)
	{
		// Properly de-allocate all resources once they've outlived their purpose
		glDeleteVertexArrays(1, &(*elt_vao));
		glDeleteBuffers(1, &(*elt_vbo));
		glDeleteBuffers(1, &(*elt_ebo));
	}

	glfwTerminate();
	return error_code;
}

