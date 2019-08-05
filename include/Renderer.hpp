//
// Created by primoz on 25. 07. 19.
//

#ifndef CPUPATHTRACER_RENDERER_HPP
#define CPUPATHTRACER_RENDERER_HPP

#include "ImageSampler.hpp"
#include "PathTracer.hpp"
#include "lsg/lsg.h"

class Renderer {
 public:
  Renderer(lsg::Ref<lsg::Scene> scene, size_t width, size_t height);

  size_t renderSample();

  const lsg::Ref<lsg::Image>& getImage() const;

 private:
  lsg::Ref<lsg::Scene> scene_;
  PathTracer tracer_;
  lsg::Ref<lsg::Transform> camTransform_;
  lsg::Ref<lsg::PerspectiveCamera> camPerspective_;

  size_t width_;
  size_t height_;
  size_t currentSample_;
  ImageSampler pixelSampler_;
  lsg::Ref<lsg::Image> accImage_;
  lsg::ImageView<glm::vec3> accImageView_;
  lsg::Ref<lsg::Image> outImage_;
  lsg::ImageView<glm::vec3> outImageView_;
};

#endif // CPUPATHTRACER_RENDERER_HPP
