// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <GLES2/gl2.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/lib/gl/gles2/gl2ext_ppapi.h"
#include "ppapi/utility/completion_callback_factory.h"

#include "ppapi/cpp/var_dictionary.h"
#include "ppapi/cpp/var_array_buffer.h"

#include "ppapi/cpp/input_event.h"

#ifdef WIN32
#undef PostMessage
// Allow 'this' in initializer list
#pragma warning(disable : 4355)
#endif

#include "core/gameboy.h"
#include "core/display.h"

namespace {

GLuint CompileShader(GLenum type, const char* data) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &data, NULL);
  glCompileShader(shader);

  GLint compile_status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
  if (compile_status != GL_TRUE) {
    // Shader failed to compile, let's see what the error is.
    char buffer[1024];
    GLsizei length;
    glGetShaderInfoLog(shader, sizeof(buffer), &length, &buffer[0]);
    fprintf(stderr, "Shader failed to compile: %s\n", buffer);
    fprintf(stderr, "Length: %d\n", length);
    fprintf(stderr, "Source: %s\n", data);
    return 0;
  }

  return shader;
}

GLuint LinkProgram(GLuint frag_shader, GLuint vert_shader) {
  GLuint program = glCreateProgram();
  glAttachShader(program, frag_shader);
  glAttachShader(program, vert_shader);
  glLinkProgram(program);

  GLint link_status;
  glGetProgramiv(program, GL_LINK_STATUS, &link_status);
  if (link_status != GL_TRUE) {
    // Program failed to link, let's see what the error is.
    char buffer[1024];
    GLsizei length;
    glGetProgramInfoLog(program, sizeof(buffer), &length, &buffer[0]);
    fprintf(stderr, "Program failed to link: %s\n", buffer);
    return 0;
  }

  return program;
}

const GLchar *kVertexShaderSource =
    "attribute vec2 a_texcoord;\n"
    "attribute vec2 a_position;\n"
    "varying vec2 v_texcoord;\n"
    "void main() {\n"
    "  gl_Position = vec4(a_position, 0.0, 1.0);\n"
    "  v_texcoord = a_texcoord;\n"
    "}\n";

const GLchar *kFragmentShaderSource =
    "precision mediump float;\n"
    "varying vec2 v_texcoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "  gl_FragColor = texture2D(u_texture, v_texcoord);\n"
    "}\n";

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

}  // namespace


class Graphics3DInstance : public pp::Instance {
 public:
  explicit Graphics3DInstance(PP_Instance instance)
      : pp::Instance(instance),
        callback_factory_(this),
        width_(0),
        height_(0),
        frag_shader_(0),
        vertex_shader_(0),
        program_(0),
        texture_loc_(0),
        position_loc_(0),
        color_loc_(0) {}

  virtual bool Init(uint32_t /*argc*/,
                    const char* /*argn*/[],
                    const char* /*argv*/[]) {
    RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);
    return true;
  }

  virtual void DidChangeView(const pp::View& view) {
    // Pepper specifies dimensions in DIPs (device-independent pixels). To
    // generate a context that is at device-pixel resolution on HiDPI devices,
    // scale the dimensions by view.GetDeviceScale().
    int32_t new_width = view.GetRect().width() * view.GetDeviceScale();
    int32_t new_height = view.GetRect().height() * view.GetDeviceScale();

    if (context_.is_null()) {
      if (!InitGL(new_width, new_height)) {
        // failed.
        return;
      }

      if (context_.is_null())
        fprintf(stderr, "null context\n");

      InitShaders();
      InitBuffers();
      InitTexture();
      MainLoop(0);
    } else {
      // Resize the buffers to the new size of the module.
      int32_t result = context_.ResizeBuffers(new_width, new_height);
      if (result < 0) {
        fprintf(stderr,
                "Unable to resize buffers to %d x %d!\n",
                new_width,
                new_height);
        return;
      }
    }

    width_ = new_width;
    height_ = new_height;
    glViewport(0, 0, width_, height_);
  }

  virtual void HandleMessage(const pp::Var& var_message) {
    if (!var_message.is_dictionary())
    {
      PostMessage(pp::Var("Error: message should be a dictionary"));
      return;
    }
    pp::VarDictionary dict_message(var_message);

    pp::Var var_name = dict_message.Get("name");
    if (!var_name.is_string()) {
      PostMessage(pp::Var("Error: name should be a string"));
      return;
    }
    std::string name = var_name.AsString();

    pp::Var var_rom = dict_message.Get("rom");
    if (!var_rom.is_array_buffer()) {
      PostMessage(pp::Var("Error: rom should be an array buffer"));
      return;
    }
    pp::VarArrayBuffer rom_buffer(var_rom);

    uint32_t rom_length = rom_buffer.ByteLength();
    char *rom_data = static_cast<char *>(rom_buffer.Map());
    std::string rom_str(rom_data, rom_length);
    std::istringstream rom_stream(rom_str);

    // TODO Temporary fix to get this building
    std::istringstream ram_stream(rom_str);

    gb_.load_rom(rom_stream, ram_stream);
    rom_loaded_ = true;
  }

  virtual bool HandleInputEvent(const pp::InputEvent& event) {
    switch (event.GetType())
    {
      case PP_INPUTEVENT_TYPE_KEYDOWN:
      {
        pp::KeyboardInputEvent key_event(event);
        switch (key_event.GetKeyCode())
        {
          case 38:
            gb_.button_pressed(Joypad::Button::UP);
            break;
          case 40:
            gb_.button_pressed(Joypad::Button::DOWN);
            break;
          case 37:
            gb_.button_pressed(Joypad::Button::LEFT);
            break;
          case 39:
            gb_.button_pressed(Joypad::Button::RIGHT);
            break;
          case 90:
            gb_.button_pressed(Joypad::Button::A);
            break;
          case 88:
            gb_.button_pressed(Joypad::Button::B);
            break;
          case 13:
            gb_.button_pressed(Joypad::Button::START);
            break;
          case 8:
            gb_.button_pressed(Joypad::Button::SELECT);
            break;
        }
        break;
      }
      case PP_INPUTEVENT_TYPE_KEYUP:
      {
        pp::KeyboardInputEvent key_event(event);
        switch (key_event.GetKeyCode())
        {
          case 38:
            gb_.button_released(Joypad::Button::UP);
            break;
          case 40:
            gb_.button_released(Joypad::Button::DOWN);
            break;
          case 37:
            gb_.button_released(Joypad::Button::LEFT);
            break;
          case 39:
            gb_.button_released(Joypad::Button::RIGHT);
            break;
          case 90:
            gb_.button_released(Joypad::Button::A);
            break;
          case 88:
            gb_.button_released(Joypad::Button::B);
            break;
          case 13:
            gb_.button_released(Joypad::Button::START);
            break;
          case 8:
            gb_.button_released(Joypad::Button::SELECT);
            break;
        }
        break;
      }
    }
    return true;
  }

 private:
  bool InitGL(int32_t new_width, int32_t new_height) {
    if (!glInitializePPAPI(pp::Module::Get()->get_browser_interface())) {
      fprintf(stderr, "Unable to initialize GL PPAPI!\n");
      return false;
    }

    const int32_t attrib_list[] = {
      PP_GRAPHICS3DATTRIB_WIDTH, new_width,
      PP_GRAPHICS3DATTRIB_HEIGHT, new_height,
      PP_GRAPHICS3DATTRIB_NONE
    };

    context_ = pp::Graphics3D(this, attrib_list);
    if (!BindGraphics(context_) || context_.is_null()) {
      fprintf(stderr, "Unable to bind 3d context!\n");
      context_ = pp::Graphics3D();
      glSetCurrentContextPPAPI(0);
      return false;
    }

    glSetCurrentContextPPAPI(context_.pp_resource());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    return true;
  }

  void InitShaders() {
    printf("opengl version: (%s)\n", glGetString(GL_VERSION));
    vertex_shader_ = CompileShader(GL_VERTEX_SHADER, kVertexShaderSource);
    if (!vertex_shader_)
      return;

    frag_shader_ = CompileShader(GL_FRAGMENT_SHADER, kFragmentShaderSource);
    if (!frag_shader_)
      return;

    program_ = LinkProgram(frag_shader_, vertex_shader_);
    if (!program_)
      return;

    texture_loc_ = glGetUniformLocation(program_, "u_texture");
    position_loc_ = glGetAttribLocation(program_, "a_position");
    texcoord_loc_ = glGetAttribLocation(program_, "a_texcoord");
    color_loc_ = glGetAttribLocation(program_, "a_color");
  }

  void InitBuffers() {
    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kDisplayVertices), &kDisplayVertices[0],
                 GL_STATIC_DRAW);
  }

  void InitTexture() {
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    //set what program to use
    glUseProgram(program_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glUniform1i(texture_loc_, 0);

    //define the attributes of the vertex
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glVertexAttribPointer(position_loc_,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          reinterpret_cast<GLvoid*>(offsetof(Vertex, pos)));
    glEnableVertexAttribArray(position_loc_);
    glVertexAttribPointer(texcoord_loc_,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          reinterpret_cast<GLvoid*>(offsetof(Vertex, tex)));
    glEnableVertexAttribArray(texcoord_loc_);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 Display::width,
                 Display::height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 gb_.get_framebuffer());

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  void MainLoop(int32_t) {
    if (rom_loaded_)
    {
      for (int i=0; i<16666; i++)
      {
        gb_.step();
      }
    }
    Render();
    context_.SwapBuffers(
        callback_factory_.NewCallback(&Graphics3DInstance::MainLoop));
  }

  pp::CompletionCallbackFactory<Graphics3DInstance> callback_factory_;
  pp::Graphics3D context_;
  int32_t width_;
  int32_t height_;
  GLuint frag_shader_;
  GLuint vertex_shader_;
  GLuint program_;
  GLuint vertex_buffer_;
  GLuint texture_;

  GLuint texture_loc_;
  GLuint position_loc_;
  GLuint texcoord_loc_;
  GLuint color_loc_;

  Gameboy gb_;
  bool rom_loaded_ = false;
};

class Graphics3DModule : public pp::Module {
 public:
  Graphics3DModule() : pp::Module() {}
  virtual ~Graphics3DModule() {}

  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new Graphics3DInstance(instance);
  }
};

namespace pp {
Module* CreateModule() { return new Graphics3DModule(); }
}  // namespace pp
