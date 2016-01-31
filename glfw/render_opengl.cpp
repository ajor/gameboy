#include "core/gameboy.h"
#include "core/display.h"

#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

GLuint compile_shader(GLenum type, const char* data) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &data, NULL);
  glCompileShader(shader);

  GLint compile_status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
  if (compile_status != GL_TRUE) {
    char buffer[1024];
    GLsizei length;
    glGetShaderInfoLog(shader, sizeof(buffer), &length, &buffer[0]);
    fprintf(stderr, "Shader failed to compile: %s\n", buffer);
    return 0;
  }

  return shader;
}

GLuint link_program(GLuint frag_shader, GLuint vert_shader) {
  GLuint program = glCreateProgram();
  glAttachShader(program, frag_shader);
  glAttachShader(program, vert_shader);
  glLinkProgram(program);

  GLint link_status;
  glGetProgramiv(program, GL_LINK_STATUS, &link_status);
  if (link_status != GL_TRUE) {
    char buffer[1024];
    GLsizei length;
    glGetProgramInfoLog(program, sizeof(buffer), &length, &buffer[0]);
    fprintf(stderr, "Program failed to link: %s\n", buffer);
    return 0;
  }

  return program;
}

const GLchar *kVertexShaderSource =
    "#version 100\n"
    "attribute vec2 a_position;"
    "attribute vec2 a_texcoord;"
    "varying vec2 v_texcoord;"
    "void main() {"
    "  gl_Position = vec4(a_position, 0.0, 1.0);"
    "  v_texcoord = a_texcoord;"
    "}";

const GLchar *kFragmentShaderSource =
    "#version 100\n"
    "precision mediump float;"
    "varying vec2 v_texcoord;"
    "uniform sampler2D u_texture;"
    "void main() {"
    "  gl_FragColor = texture2D(u_texture, v_texcoord);"
    "}";

struct Vertex {
  GLfloat pos[2];
  GLfloat tex[2];
};

const Vertex kDisplayVertices[] = {
 {{-1.0f,-1.0f},  {0.0f, 1.0f}},
 {{-1.0f, 1.0f},  {0.0f, 0.0f}},
 {{ 1.0f,-1.0f},  {1.0f, 1.0f}},
 {{ 1.0f, 1.0f},  {1.0f, 0.0f}},
 {{ 1.0f,-1.0f},  {1.0f, 1.0f}},
 {{-1.0f, 1.0f},  {0.0f, 0.0f}},
};

GLFWwindow *initgl(int32_t width, int32_t height) {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialise GLFW\n");
    abort();
  }


  GLFWwindow *window = glfwCreateWindow(width, height, "Gameboy Emulator", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to open window.\n");
    abort();
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialise GLEW.\n");
    abort();
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  return window;
}

GLuint init_shaders() {
  printf("opengl version: (%s)\n", glGetString(GL_VERSION));
  GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, kVertexShaderSource);
  if (!vertex_shader)
    abort();

  GLuint frag_shader = compile_shader(GL_FRAGMENT_SHADER, kFragmentShaderSource);
  if (!frag_shader)
    abort();

  GLuint program = link_program(frag_shader, vertex_shader);
  if (!program)
    abort();

  glUseProgram(program);

  return program;
}

void render_loop(Gameboy &gb)
{
  const unsigned int width = Display::width * 5;
  const unsigned int height = Display::height * 5;

  GLFWwindow *window = initgl(width, height);
  glfwSetWindowUserPointer(window, &gb);

  //
  // OpenGL settings
  //
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, width, height, 0);

  GLuint program = init_shaders();

  GLuint texture_loc = glGetUniformLocation(program, "u_texture");
  GLuint position_loc = glGetAttribLocation(program, "a_position");
  GLuint texcoord_loc = glGetAttribLocation(program, "a_texcoord");

  //
  // Buffers
  //
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kDisplayVertices), kDisplayVertices, GL_STATIC_DRAW);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // position attribute
  glVertexAttribPointer(position_loc, 2,
      GL_FLOAT, GL_FALSE, sizeof(Vertex),
      reinterpret_cast<GLvoid*>(offsetof(Vertex, pos)));
  glEnableVertexAttribArray(position_loc);

  // texcoord attribute
  glVertexAttribPointer(texcoord_loc, 2,
      GL_FLOAT, GL_FALSE, sizeof(Vertex),
      reinterpret_cast<GLvoid*>(offsetof(Vertex, tex)));
  glEnableVertexAttribArray(texcoord_loc);

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

  glActiveTexture(GL_TEXTURE0);

  // Uncomment to change display FPS
  //  glfwSwapInterval(0);
  while (!glfwWindowShouldClose(window))
  {
    for (int i=0; i<16666; i++)
    {
      gb.step();
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Display::width, Display::height,
        0, GL_RGB, GL_UNSIGNED_BYTE, gb.get_framebuffer());
    glUniform1i(texture_loc, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
  Gameboy *gb = (Gameboy *)glfwGetWindowUserPointer(window);
  bool pressed = (action != GLFW_RELEASE);
  switch (key)
  {
    case GLFW_KEY_UP:
      pressed ? gb->button_pressed(Joypad::Button::UP) :
                gb->button_released(Joypad::Button::UP);
      break;
    case GLFW_KEY_DOWN:
      pressed ? gb->button_pressed(Joypad::Button::DOWN) :
                gb->button_released(Joypad::Button::DOWN);
      break;
    case GLFW_KEY_LEFT:
      pressed ? gb->button_pressed(Joypad::Button::LEFT) :
                gb->button_released(Joypad::Button::LEFT);
      break;
    case GLFW_KEY_RIGHT:
      pressed ? gb->button_pressed(Joypad::Button::RIGHT) :
                gb->button_released(Joypad::Button::RIGHT);
      break;
    case GLFW_KEY_Z:
      pressed ? gb->button_pressed(Joypad::Button::A) :
                gb->button_released(Joypad::Button::A);
      break;
    case GLFW_KEY_X:
      pressed ? gb->button_pressed(Joypad::Button::B) :
                gb->button_released(Joypad::Button::B);
      break;
    case GLFW_KEY_ENTER:
      pressed ? gb->button_pressed(Joypad::Button::START) :
                gb->button_released(Joypad::Button::START);
      break;
    case GLFW_KEY_BACKSPACE:
      pressed ? gb->button_pressed(Joypad::Button::SELECT) :
                gb->button_released(Joypad::Button::SELECT);
      break;
  }
}
