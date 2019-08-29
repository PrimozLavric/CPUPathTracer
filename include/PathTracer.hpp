#ifndef CPUPATHTRACER_PATHTRACER_HPP
#define CPUPATHTRACER_PATHTRACER_HPP

#include <chrono>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/component_wise.hpp>
#include <random>
#include "RayIntersector.hpp"
#include "lsg/lsg.h"

class PathTracer {
public:
    struct Configuration {
        Configuration(size_t maxDepth = 20, bool russianRoulette = true, size_t rrBounces = 3,
                      float rrStopProbability = 0.05);

        size_t maxDepth;
        bool russianRoulette;
        size_t rrBounces;
        float rrStopProbability;
    };

    PathTracer(const lsg::Ref<lsg::Scene> &scene, const Configuration &config = {});

    glm::vec3 traceRay(lsg::Ray<float> ray);

    float random(float min, float max) {
        static thread_local std::mt19937 generator(
                std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(generator);
    }

protected:
    enum class InteractionType {
        kDiff, kSpec, kRefr
    };

    InteractionType determineInteractionType(float metallicFactor, float transmissionFactor);

    float
    calcFresnelReflectance(glm::vec3 n, glm::vec3 nl, glm::vec3 rayDirection, float nc, float nt, glm::vec3 &tdir);

private:
    RayIntersector intersector_;
    Configuration config_;
};

#endif // CPUPATHTRACER_PATHTRACER_HPP
