//
// Created by primoz on 25. 07. 19.
//
#include "Renderer.hpp"
#include <algorithm>
#include <utility>

Renderer::Renderer(lsg::Ref<lsg::Scene> scene, size_t width, size_t height)
        : scene_(std::move(scene)), tracer_(scene_), camTransform_(), camPerspective_(), width_(width), height_(height),
          currentSample_(0u), pixelSampler_(width, height),
          accImage_(lsg::makeRef<lsg::Image>("Acc", lsg::Format::eR32G32B32Sfloat, width_, height_)),
          accImageView_(accImage_),
          outImage_(lsg::makeRef<lsg::Image>("Out", lsg::Format::eR32G32B32Sfloat, width_, height_)),
          outImageView_(outImage_) {
    scene_->traverseDown([&](const lsg::Ref<lsg::Object> &object) {
        if (!camPerspective_) {
            camPerspective_ = object->getComponent<lsg::PerspectiveCamera>();
            camTransform_ = object->getComponent<lsg::Transform>();
            return true;
        } else {
            return false;
        }
    });

    if (!camPerspective_) {
        throw std::runtime_error("Could not find camera.");
    }
}

size_t Renderer::renderSample() {
    std::vector<PixelSample> samples = pixelSampler_.generateSamples();
    currentSample_++;

#pragma omp parallel for num_threads(8)
    for (size_t i = 0; i < samples.size(); i++) {
        float rX = samples[i].sample.x * 2.0 / width_ - 1; // [0, screenWidth] -> [-1, 1]
        float rY = samples[i].sample.y * 2.0 / height_ - 1;

        glm::vec3 rayDir = glm::normalize(
                glm::vec3(rX * glm::sin(camPerspective_->fov() / 2.0f), rY * glm::sin(camPerspective_->fov() / 2.0f),
                          -1));
        lsg::Ray<float> ray(glm::vec3(0.0f), rayDir);
        ray = ray.transform(camTransform_->worldMatrix());

        accImageView_.at(samples[i].pixel.x, samples[i].pixel.y) += tracer_.traceRay(ray);
        outImageView_.at(samples[i].pixel.x, samples[i].pixel.y) = glm::clamp(
                accImageView_.at(samples[i].pixel.x, samples[i].pixel.y) / static_cast<float>(currentSample_), 0.0f,
                1.0f);
    }

    return currentSample_;
}

const lsg::Ref<lsg::Image> &Renderer::getImage() const {
    return outImage_;
}
