//
// Created by primoz on 20.7.2019.
//

#include <glad/glad.h>
#define NOREORDER
#include <chrono>
#include <cppglfw/CppGLFW.h>
#include <future>
#include <lodepng.h>
#include <lsg/lsg.h>
#include <stdio.h>
#include <stdlib.h>
#include "ImageSampler.hpp"
#include "RayIntersector.hpp"
#include "Renderer.hpp"

static const GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
                                   1.0f,  -1.0f, 0.0f, 1.0f, 1.0f,  0.0f, -1.0f, 1.0f, 0.0f};

static const char* vertex_shader_text = "#version 450\n"
                                        "layout(location = 0) in vec3 vPos;\n"
                                        "layout(location = 0) out vec2 uv;\n"
                                        "void main()\n"
                                        "{\n"
                                        "    uv = (vPos.xy + 1.0f) / 2.0f;"
                                        "    gl_Position = vec4(vPos, 1.0);\n"
                                        "}\n";
static const char* fragment_shader_text = "#version 450\n"
                                          "layout(location = 0) in vec2 uv;\n"
                                          "uniform sampler2D texSampler;\n"
                                          "out vec3 outFragColor;\n"
                                          "void main()\n"
                                          "{\n"
                                          "    outFragColor = texture(texSampler, uv).rgb;\n"
                                          "}\n";

void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static const size_t width = 800;
static const size_t height = 600;
static const float scale = 1.0f;

int main() {
  cppglfw::GLFWManager& glfwManager = cppglfw::GLFWManager::instance();
  std::map<int32_t, std::variant<int32_t, std::string>> hints = {{GLFW_CONTEXT_VERSION_MAJOR, 4},
                                                                 {GLFW_CONTEXT_VERSION_MINOR, 5},
                                                                 {GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE},
                                                                 {GLFW_RESIZABLE, GLFW_FALSE}};

  glfwSetErrorCallback(error_callback);

  cppglfw::Window window = glfwManager.createWindow("Window", width * scale, height * scale, hints);
  window.makeContextCurrent();

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glfwManager.swapInterval(1);

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(vertex_shader);
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(fragment_shader);
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glUseProgram(program);

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);
  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  GLuint vpos_location = glGetAttribLocation(program, "vPos");
  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

  // Texture
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glUniform1i(0, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);

  std::unique_ptr<Renderer> renderer;

  std::future<size_t> sample = std::async(std::launch::async, [&] {
    lsg::GLTFLoader loader;
    std::vector<lsg::Ref<lsg::Scene>> scenes = loader.load("./resources/cornell_box.gltf");
    renderer = std::make_unique<Renderer>(scenes[0], width, height);
    return renderer->renderSample();
  });

  std::cout << "Drawing" << std::endl << std::flush;

  while (!window.shouldClose()) {
    using namespace std::chrono_literals;
    if (sample.wait_for(1s) == std::future_status::ready) {
      std::cout << "Sample: " << sample.get() << std::endl;
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, renderer->getImage()->rawPixelData());
      sample = std::async(std::launch::async, [&] { return renderer->renderSample(); });
    }

    auto fbSize = window.getFramebufferSize();
    glViewport(0, 0, fbSize.first, fbSize.second);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    window.swapBuffers();
    glfwManager.pollEvents();
  }
}

/*
int main() {



  Renderer renderer;

  auto img = renderer.render(scenes[0], 256, 256);

  unsigned error = lodepng::encode("out.png", reinterpret_cast<const unsigned char*>(img->rawPixelData()), 256, 256,
                                   LodePNGColorType::LCT_RGB);

  if (error)
    printf("encoder error %d: %s", error, lodepng_error_text(error));

  int a = 2;
}*/