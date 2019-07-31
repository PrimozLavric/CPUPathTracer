//
// Created by primoz on 20.7.2019.
//

#include <lodepng.h>
#include <lsg/lsg.h>
#include "ImageSampler.hpp"
#include "RayIntersector.hpp"
#include "Renderer.hpp"

int main() {
  lsg::GLTFLoader loader;
  std::vector<lsg::Ref<lsg::Scene>> scenes = loader.load("./resources/cornell_box.gltf");

  Renderer renderer;

  /*
  auto image = lsg::makeRef<lsg::Image>("OutImage", lsg::Format::eR8G8B8Srgb, 1024, 1024, 1);
  lsg::ImageView<glm::u8vec3> image_view(image);

  for (size_t i = 0; i < image_view.width(); i++) {
    for (size_t j = 0; j < image_view.height(); j++) {
      image_view.at(i, j) = glm::u8vec3(i / static_cast<float>(image_view.width()) * 255.0f,
                                        j / static_cast<float>(image_view.height()) * 255.0, 0.0);
    }
  }*/

  auto img = renderer.render(scenes[0], 256, 256);

  unsigned error = lodepng::encode("out.png", reinterpret_cast<const unsigned char*>(img->rawPixelData()), 256, 256,
                                   LodePNGColorType::LCT_RGB);

  if (error)
    printf("encoder error %d: %s", error, lodepng_error_text(error));

  int a = 2;
}