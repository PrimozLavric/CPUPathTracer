#include "PathTracer.hpp"

PathTracer::Configuration::Configuration(size_t maxDepth, bool russianRoulette, size_t rrBounces,
                                         float rrStopProbability)
  : maxDepth(maxDepth), russianRoulette(russianRoulette), rrBounces(rrBounces), rrStopProbability(rrStopProbability) {}

PathTracer::PathTracer(const lsg::Ref<lsg::Scene>& scene, const PathTracer::Configuration& config)
  : intersector_(scene), config_(config) {}

glm::vec3 PathTracer::traceRay(lsg::Ray<float> ray) {
  glm::vec3 accColor(0.0f);
  glm::vec3 mask(1.0f);

  for (size_t bounce = 0; bounce < config_.maxDepth; bounce++) {
    std::optional<Intersection> optIntersection = intersector_.intersect(ray);

    // Stop if there is no intersection.
    if (!optIntersection.has_value()) {
      break;
    }

    const Intersection& intersection = optIntersection.value();
    const lsg::Ref<lsg::MetallicRoughnessMaterial> material =
      lsg::dynamicRefCast<lsg::MetallicRoughnessMaterial>(intersection.mesh->material());

    // Handle unknown material.
    if (!material) {
      throw std::runtime_error("Unknown material");
    }

    InteractionType interaction =
      determineInteractionType(material->roughnessFactor(), material->metallicFactor(), material->emissiveFactor());

    if (interaction == InteractionType::kLight) {
      accColor = mask * material->emissiveFactor() * 10.0f;
    } else if (interaction == InteractionType::kDiff) {
      float r1 = 2.0f * glm::pi<float>() *
                 random(0.0f, 1.0f); // pick random number on unit circle (radius = 1, circumference = 2*Pi) for azimuth
      float r2 = random(0.0f, 1.0f); // pick random number for elevation
      float r2s = glm::sqrt(r2);

      glm::vec3 nl = glm::dot(intersection.normal, ray.dir()) < 0.0f
                       ? intersection.normal
                       : intersection.normal * -1.0f; // front facing normal

      glm::vec3 w = nl;
      glm::vec3 u = glm::normalize(
        glm::cross((glm::abs(w.x) > 0.1f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f)), w));
      glm::vec3 v = glm::cross(w, u);

      ray = lsg::Ray<float>(intersection.position + w * 0.0005f,
                            glm::normalize(u * glm::cos(r1) * r2s + v * glm::sin(r1) * r2s + w * glm::sqrt(1.0f - r2)));

      mask *= glm::vec3(material->baseColorFactor());
      mask *= glm::dot(ray.dir(), nl);
    } else if (interaction == InteractionType::kSpec) {
      glm::vec3 nl = glm::dot(intersection.normal, ray.dir()) < 0.0f
                       ? intersection.normal
                       : intersection.normal * -1.0f; // front facing normal

      mask *= glm::vec3(material->baseColorFactor());
      ray = lsg::Ray<float>(intersection.position + nl * 0.000005f, glm::reflect(ray.dir(), nl));
    }

    // Early termination RR.
    if (glm::compMax(mask) < 0.5 && bounce > config_.rrBounces) {
      float q = glm::max(0.05f, 1.0f - glm::compMax(mask));
      if (random(0.0f, 1.0f) < q) {
        break;
      }
      mask /= 1.0f - q;
    }
  }

  return accColor;
}

PathTracer::InteractionType PathTracer::determineInteractionType(float roughnessFactor, float metallicFactor,
                                                                 glm::vec3 emissiveFactor) {
  float avgEmissiveFactor = glm::compAdd(emissiveFactor) / 3.0f;
  float maxRand = roughnessFactor + metallicFactor + avgEmissiveFactor;
  if (maxRand == 0.0f) {
    return InteractionType::kDiff;
  }

  float r = random(0.0f, maxRand);

  if (r < roughnessFactor) {
    return InteractionType::kDiff;
  } else if (r < roughnessFactor + metallicFactor) {
    return InteractionType::kSpec;
  } else {
    return InteractionType::kLight;
  }
}
