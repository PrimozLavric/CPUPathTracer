//
// Created by primoz on 25. 07. 19.
//
#include "Renderer.hpp"
#include <algorithm>

lsg::Ref<lsg::Image> Renderer::render(const lsg::Ref<lsg::Scene>& scene, size_t width, size_t height) {
  PathTracer tracer(scene);

  lsg::Ref<lsg::Object> camera = scene->find("Camera_Orientation");
  lsg::Ref<lsg::Transform> camTransform = camera->getComponent<lsg::Transform>();
  lsg::Ref<lsg::PerspectiveCamera> camPerspective = camera->getComponent<lsg::PerspectiveCamera>();

  lsg::Ref<lsg::Image> image = lsg::makeRef<lsg::Image>("Image", lsg::Format::eR32G32B32Sfloat, width, height);
  lsg::ImageView<glm::vec3> imageView(image);

  ImageSampler pixelSampler(width, height);

  size_t spp = 500;

  for (size_t i = 0; i < spp; i++) {
    std::vector<PixelSample> samples = pixelSampler.generateSamples();

#pragma omp parallel for num_threads(4)
    for (size_t i = 0; i < samples.size(); i++) {
      float rX = samples[i].sample.x * 2.0 / width - 1; // [0, screenWidth] -> [-1, 1]
      float rY = samples[i].sample.y * 2.0 / height - 1;

      glm::vec3 rayDir = glm::normalize(
        glm::vec3(rX * glm::sin(camPerspective->fov() / 2.0f), rY * glm::sin(camPerspective->fov() / 2.0f), -1));
      lsg::Ray<float> ray(glm::vec3(0.0f), rayDir);
      ray = ray.transform(camTransform->worldMatrix());

      imageView.at(samples[i].pixel.x, height - samples[i].pixel.y - 1) += tracer.traceRay(ray) * (1.0f / spp);
    }
  }

  lsg::Ref<lsg::Image> imageOut = lsg::makeRef<lsg::Image>("Image", lsg::Format::eR8G8B8Srgb, width, height);
  lsg::ImageView<glm::u8vec3> imageOutView(imageOut);

  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      imageOutView.at(i, j) = glm::clamp(imageView.at(i, j), 0.0f, 1.0f) * 255.0f;
    }
  }

  return imageOut;
}
