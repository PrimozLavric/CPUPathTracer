//
// Created by primoz on 25. 07. 19.
//
#include "Renderer.hpp"

lsg::Ref<lsg::Image> Renderer::render(const lsg::Ref<lsg::Scene>& scene, size_t width, size_t height) {
  PathTracer tracer(scene);

  lsg::Ref<lsg::Object> camera = scene->find("Camera_Orientation");
  lsg::Ref<lsg::Transform> camTransform = camera->getComponent<lsg::Transform>();
  lsg::Ref<lsg::PerspectiveCamera> camPerspective = camera->getComponent<lsg::PerspectiveCamera>();

  lsg::Ref<lsg::Image> image = lsg::makeRef<lsg::Image>("Image", lsg::Format::eR32G32B32Sfloat, width, height);
  lsg::ImageView<glm::vec3> imageView(image);

  ImageSampler pixelSampler(width, height);
  std::vector<PixelSample> samples = pixelSampler.generateSamples();

  for (const auto& sample : samples) {
    float rX = sample.sample.x * 2 / width - 1; // [0, screenWidth] -> [-1, 1]
    float rY = sample.sample.y * 2 / height - 1;

    glm::vec3 rayDir = glm::normalize(
      glm::vec3(rX * glm::sin(camPerspective->fov() / 2.0f), rY * glm::sin(camPerspective->fov() / 2.0f), 1));
    lsg::Ray<float> ray(glm::vec3(0.0f), rayDir);
    ray = ray.transform(camTransform->worldMatrix());

    imageView.at(sample.pixel.x, sample.pixel.y) = tracer.traceRay(ray);
  }

  lsg::Ref<lsg::Image> imageOut = lsg::makeRef<lsg::Image>("Image", lsg::Format::eR8G8B8Srgb, width, height);
  lsg::ImageView<glm::u8vec3> imageOutView(imageOut);

  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      imageOutView.at(i, j) = glm::abs(imageView.at(i, j) * 255.0f);
    }
  }

  return imageOut;
}
