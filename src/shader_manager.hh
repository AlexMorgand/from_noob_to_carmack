#ifndef SHADER_MANAGER_HH
# define SHADER_MANAGER_HH

# include <string>
# include <fstream>
# include <sstream>
# include <iostream>

# include <GL/glew.h>

class ShaderManager
{
	public:
		ShaderManager(const std::string vert_path, const std::string frag_path);
		void use();
		GLuint get_program();
	private:
		void compile_shader(std::string source_code, GLenum type);
		std::string load_code(std::string path);
		GLuint program_;
		unsigned int error_code_;
		GLchar infoLog_[512];
};

#endif