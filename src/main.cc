// /!\ glew should be declared before freeglut.
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "math_3d.hh"
#include <iostream>
#include <string.h>
#include "ogldev_util.h"

GLint winWidth = 600, winHeight = 600;

// The vertex buffer is a global variable.
GLuint VBO;

const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

void add_shader(GLuint shader_program, const char* shader_text, GLenum shader_type)
{
  // Our shader.
  GLuint shader_obj = glCreateShader(shader_type);

  if (shader_obj == 0)
  {
    fprintf(stderr, "Error creating shader type %d\n", shader_type);
    exit(0);
  }

  const GLchar* p[1];
  p[0] = shader_text;
  GLint len[1];
  len[0] = strlen(shader_text);
  glShaderSource(shader_obj, 1, p, len);
  glCompileShader(shader_obj);
  GLint success;
  glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    GLchar InfoLog[1024];
    glGetShaderInfoLog(shader_obj, sizeof(InfoLog), NULL, InfoLog);
    fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader_type, InfoLog);
    exit(1);
  }
  glAttachShader(shader_program, shader_obj);
}

void compile_shaders()
{
  // All the shaders will be linked to this variable.
  GLuint shader_program = glCreateProgram();
  std::string vs, fs;

  if (!ReadFile(pVSFileName, vs))
    exit(1);

  if (!ReadFile(pFSFileName, fs))
    exit(1);

  add_shader(shader_program, vs.c_str(), GL_VERTEX_SHADER);
  add_shader(shader_program, fs.c_str(), GL_FRAGMENT_SHADER);

  GLint Success = 0;
  GLchar ErrorLog[1024] = { 0 };

  glLinkProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &Success);
  if (Success == 0)
  {
    glGetProgramInfoLog(shader_program, sizeof(ErrorLog), NULL, ErrorLog);
    fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
  }

  glValidateProgram(shader_program);
  glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &Success);
  if (!Success)
  {
    glGetProgramInfoLog(shader_program, sizeof(ErrorLog), NULL, ErrorLog);
    fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
    exit(1);
  }

  glUseProgram(shader_program);
}

void render_scene(void)
{

  /*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glBegin(GL_POINTS);
  glVertex3f(0.0, 0.0, 0.0);
  glEnd();

  glBegin(GL_TRIANGLES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(1.0, 0.0, 0.0);
  glVertex3f(0.5, 1.0, 0.0);
  glEnd();

  glFlush();*/


  // We clear the buffer with the color specified above.
  glClear(GL_COLOR_BUFFER_BIT);

  // This part is linking a shader and c++. Here, we deal with a vertex.
  glEnableVertexAttribArray(0);

  // New binding ?
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Not very clear but we specify the vertex attribute (type/length/flags).
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // We are not using a index buffer so we use a orderned draw. Start from index 0 and draw 1 vertex.
  //glDrawArrays(GL_POINTS, 0, 1);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // Good practice after drawing. We make the ressource accessible for any shader or others.
  glDisableVertexAttribArray(0);

  // Switch between the front and back buffer. I supposed this
  // is not working for a single buffer mode.
  glutSwapBuffers();

}


static void init_vertex_buffer()
{

  // Init a null vector.
  Vector3f Vertices[3];
  Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
  Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
  Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

  // Generate the vertex buffer.
  glGenBuffers(1, &VBO);

  // Bind a vector to the buffer. We bind it to a target name and
  // the action are done to the target and not the handle (VBO).
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Fill the VBO with the vertices.
  // Static draw because the scene is not moving.
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

void reshape_win(GLint w, GLint h)
{
  glViewport(0, 0, w, h);

  winWidth = w;
  winHeight = h;
}

int main(int argc, char* argv[])
{
  // Initialization of glut library.
  glutInit(&argc, argv);

  // Display mode with double buffer and colors in RGBA.
  // Double buffer will increase the quality of the display and avoid
  // flickering.
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

  // Init the window size/position/name.
  glutInitWindowSize(1024, 768);
  glutInitWindowPosition(winWidth, winHeight);
  glutCreateWindow("Carmack project");

  // Init glew (to handle different extensions) and check for errors.
  GLenum res = glewInit();
  if (res != GLEW_OK)
  {
    std::cerr << "Error: " << glewGetErrorString(res) << std::endl;
    return 1;
  }

  // Clean the screen.
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  init_vertex_buffer();
  compile_shaders();

  // Bind the display function.
  glutDisplayFunc(render_scene);
  glutReshapeFunc(reshape_win);

  // The main loop is always called at the end.
  glutMainLoop();

  return 0;
}

