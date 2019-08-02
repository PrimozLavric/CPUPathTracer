//
// Created by primoz on 25. 07. 19.
//
#include "Renderer.hpp"
#include <algorithm>
#include <utility>

Renderer::Renderer(lsg::Ref<lsg::Scene> scene, size_t width, size_t height)
  : scene_(std::move(scene)), tracer_(scene_), camTransform_(), camPerspective_(), width_(width), height_(height),
    currentSample_(0u), pixelSampler_(width, height),
    image_(lsg::makeRef<lsg::Image>("Out", lsg::Format::eR32G32B32Sfloat, width_, height_)), imageView_(image_) {
  auto camera = scene_->find("Camera_Orientation");
  camTransform_ = camera->getComponent<lsg::Transform>();
  camPerspective_ = camera->getComponent<lsg::PerspectiveCamera>();
}

size_t Renderer::renderSample() {
  std::vector<PixelSample> samples = pixelSampler_.generateSamples();

#pragma omp parallel for num_threads(8)
  for (size_t i = 0; i < samples.size(); i++) {
    float rX = samples[i].sample.x * 2.0 / width_ - 1; // [0, screenWidth] -> [-1, 1]
    float rY = samples[i].sample.y * 2.0 / height_ - 1;

    glm::vec3 rayDir = glm::normalize(
      glm::vec3(rX * glm::sin(camPerspective_->fov() / 2.0f), rY * glm::sin(camPerspective_->fov() / 2.0f), -1));
    lsg::Ray<float> ray(glm::vec3(0.0f), rayDir);
    ray = ray.transform(camTransform_->worldMatrix());

    imageView_.at(samples[i].pixel.x, samples[i].pixel.y) =
      glm::clamp((imageView_.at(samples[i].pixel.x, samples[i].pixel.y) * static_cast<float>(currentSample_) +
                  tracer_.traceRay(ray)) *
                   (1.0f / static_cast<float>(currentSample_ + 1)),
                 0.0f, 1.0f);
  }

  return ++currentSample_;
}
const lsg::Ref<lsg::Image>& Renderer::getImage() const {
  return image_;
}
