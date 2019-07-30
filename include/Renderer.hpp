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
  lsg::Ref<lsg::Image> render(const lsg::Ref<lsg::Scene>& scene, size_t width, size_t height);
};

#endif // CPUPATHTRACER_RENDERER_HPP
