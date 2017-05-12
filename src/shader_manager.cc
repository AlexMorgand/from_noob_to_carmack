#include "shader_manager.hh"

ShaderManager::ShaderManager(const std::string vert_path, const std::string frag_path)
{
	unsigned int error_code = 0;
	program_ = glCreateProgram();
	std::string vert_code = load_code(vert_path), frag_code = load_code(frag_path);

	compile_shader(vert_code, GL_VERTEX_SHADER);
	compile_shader(frag_code, GL_FRAGMENT_SHADER);

}

GLuint ShaderManager::get_program()
{
	return program_;
}

std::string ShaderManager::load_code(std::string path)
{
	std::ifstream file;
	file.open(path, std::ifstream::in);

	std::string str((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());
	return str;
}

void ShaderManager::compile_shader(std::string source_code, GLenum type)
{
	GLint success;
	GLchar infoLog[512];

    // Build and compile our shader program
	const GLchar* tmp = source_code.c_str();

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &tmp, nullptr);
	glCompileShader(shader);
	// Check for compile time errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		error_code_ = -1;
	}
		
	// Link shader.
	glAttachShader(program_, shader);
	glDeleteShader(shader);

    // Link shaders to the main program.
    glLinkProgram(program_);
    // Check for linking errors.
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success)
	{
        glGetProgramInfoLog(program_, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		error_code_ = -1;
    }
}
		
void ShaderManager::use()
{
	glUseProgram(program_);
}