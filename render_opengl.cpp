#include "gameboy.h"
#include "display.h"

#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void render_loop(Gameboy &gb)
{
  //
  // Set up window
  //
  const unsigned int width = Display::width * 5;
  const unsigned int height = Display::height * 5;
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialise GLFW\n");
    abort();
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(width, height, "Gameboy Emulator", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to open window.\n");
    glfwTerminate();
    abort();
  }
  glfwMakeContextCurrent(window);
//  glfwSetWindowUserPointer(window, this);
//  glfwSetKeyCallback(window, key_callback);

  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialise GLEW.\n");
    abort();
  }

  //
  // OpenGL settings
  //
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, width, height, 0);

  //
  // Shaders
  //
  const GLchar *vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec2 position;"
    "layout (location = 1) in vec2 texCoord;"
    "out vec2 TexCoord;"
    "void main()"
    "{"
    "  gl_Position = vec4(position, 0.0, 1.0);"
    "  TexCoord = texCoord;"
    "}";

  const GLchar *fragment_shader_source =
    "#version 330 core\n"
    "in vec2 TexCoord;"
    "out vec4 colour;"
    "uniform sampler2D display;"
    "void main()"
    "{"
    "  colour = texture(display, TexCoord);"
    "}";

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  GLint status;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
  if (!status)
  {
    char error_buffer[512];
    glGetShaderInfoLog(vertex_shader, 512, NULL, error_buffer);
    fprintf(stderr, "Vertex shader error:\n%s\n", error_buffer);
    abort();
  }

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
  if (!status)
  {
    char error_buffer[512];
    glGetShaderInfoLog(fragment_shader, 512, NULL, error_buffer);
    fprintf(stderr, "Fragment shader error:\n%s\n", error_buffer);
    abort();
  }

  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
  if (!status)
  {
    char error_buffer[512];
    glGetProgramInfoLog(shader_program, 512, NULL, error_buffer);
    fprintf(stderr, "Shader link error:\n%s\n", error_buffer);
    abort();
  }
  glUseProgram(shader_program);

  //
  // Buffers
  //
  GLfloat display_vertices[] = {
    // Pos          Tex
   -1.0f,-1.0f,  0.0f, 1.0f,
   -1.0f, 1.0f,  0.0f, 0.0f,
    1.0f,-1.0f,  1.0f, 1.0f,
    1.0f, 1.0f,  1.0f, 0.0f,
    1.0f,-1.0f,  1.0f, 1.0f,
   -1.0f, 1.0f,  0.0f, 0.0f,
  };

  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(display_vertices), display_vertices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);

  // TexCoord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  //
  // Texture
  //
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // Uncomment to change display FPS to confirm games still run at correct speed
  glfwSwapInterval(0);
  while (!glfwWindowShouldClose(window))
  {
    for (int i=0; i<100; i++)
    {
      gb.step();
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Display::width, Display::height, 0, GL_RGB, GL_UNSIGNED_BYTE, gb.get_framebuffer());
    glUniform1i(glGetUniformLocation(shader_program, "display"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}
