#ifndef CPUPATHTRACER_PATHTRACER_HPP
#define CPUPATHTRACER_PATHTRACER_HPP

#include <lsg/lsg.h>
#include "RayIntersector.hpp"

class PathTracer {
 public:
  struct Configuration {
    size_t maxDepth;
    bool russianRoulette;
    size_t rrBounces;
    float rrStopProbability;
  };

  PathTracer(const lsg::Ref<lsg::Scene>& scene, const Configuration& config = {})
    : intersector_(scene), config_(config) {}

  glm::vec3 traceRay(const lsg::Ray<float>& ray) {
    glm::vec3 accColor(0.0f);
    glm::vec3 beta(1.0f);

    for (size_t bounce = 0; bounce < config_.maxDepth; bounce++) {
      std::optional<Intersection> optIntersection = intersector_.intersect(ray);

      if (!optIntersection.has_value()) {
        return accColor;
      }

      const Intersection& intersection = optIntersection.value();
      const lsg::Ref<lsg::MetallicRoughnessMaterial> material =
        lsg::dynamicRefCast<lsg::MetallicRoughnessMaterial>(intersection.mesh->material());

      return intersection.normal;

      // Handle unknown material.
      if (!material) {
        return accColor;
      }

      accColor += beta * material->emissiveFactor();
    }
  }

 private:
  RayIntersector intersector_;
  Configuration config_;
};

#endif // CPUPATHTRACER_PATHTRACER_HPP
