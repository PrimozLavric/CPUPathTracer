
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <utility>

//
// Created by primoz on 21.7.2019.
//

#include "RayIntersector.h"

Intersection::Intersection(glm::vec3 position, uint32_t primitiveIndex, std::shared_ptr<lsg::SubMesh> mesh)
  : position(position), primitiveIndex(primitiveIndex), mesh(std::move(mesh)) {}

RayIntersector::MeshIntersectable::MeshIntersectable(glm::mat4 inWorldMat, std::shared_ptr<lsg::SubMesh> inMesh)
  : worldMat(inWorldMat), worldMatInverse(glm::inverse(worldMat)), mesh(std::move(inMesh)) {
  assert(mesh->geometry());

  lsg::bvh::SplitBVHBuilder<float> builder;
  bvh = builder.process(mesh->geometry()->getTriangleAccessor());
}

RayIntersector::RayIntersector(const std::shared_ptr<lsg::Scene>& scene) {
  for (const auto& rootObj : scene->rootObjects()) {
    rootObj->traverseDown([this](const std::shared_ptr<lsg::Object>& object) {
      if (auto mesh = object->getComponent<lsg::Mesh>()) {
        std::cout << "Building BVH for object " << object->name() << "..." << std::flush;
        glm::mat4 worldMatrix(1.0f);

        if (auto transform = object->getComponent<lsg::Transform>()) {
          worldMatrix = transform->worldMatrix();
        }

        for (const auto& submesh : mesh->subMeshes()) {
          intersectables.emplace_back(worldMatrix, submesh);
        }
        std::cout << " Finished." << std::endl;
      }

      return true;
    });
  }

  std::cout << "Building scene BVH..." << std::flush;
  std::vector<lsg::AABB<float>> intersectablesBounds;
  intersectablesBounds.reserve(intersectables.size());

  for (const auto& intersectable : intersectables) {
    intersectablesBounds.emplace_back(intersectable.bvh->getBounds().transform(intersectable.worldMat));
  }

  lsg::bvh::BVHBuilder<float> builder;
  bvh = builder.process(intersectablesBounds);
  std::cout << " Finished." << std::endl;
}

std::optional<Intersection> RayIntersector::intersect(const lsg::Ray<float>& ray) const {
  std::vector<uint32_t> sceneIsectIndices = bvh->rayIntersect(ray);

  std::optional<Intersection> closestIntersection;

  for (uint32_t i : sceneIsectIndices) {
    const MeshIntersectable& intersectable = intersectables[i];
    // Transform ray to object space.
    lsg::Ray<float> objectSpaceRay = ray.transform(intersectable.worldMatInverse);

    // Find potential triangle intersections.
    std::vector<uint32_t> objectIsectIndices = intersectable.bvh->rayIntersect(objectSpaceRay);

    glm::vec3 closestIsectPos(std::numeric_limits<float>::max());
    uint32_t index = std::numeric_limits<uint32_t>::max();

    // Test triangles.
    std::shared_ptr<lsg::TriangleAccessor<float>> triAccessor = intersectable.mesh->geometry()->getTriangleAccessor();
    for (uint32_t j : objectIsectIndices) {
      std::optional<glm::vec3> triIsectPos =
        objectSpaceRay.intersectTriangle((*triAccessor)[j].a(), (*triAccessor)[j].b(), (*triAccessor)[j].c());

      if (triIsectPos.has_value() && glm::distance(objectSpaceRay.origin(), *triIsectPos) <
                                       glm::distance(objectSpaceRay.origin(), closestIsectPos)) {
        closestIsectPos = *triIsectPos;
        index = j;
      }
    }

    // If there was any intersection.
    if (index != std::numeric_limits<uint32_t>::max()) {
      glm::vec3 worldClosestIntersectionPos = intersectable.worldMat * glm::vec4(closestIsectPos, 1.0);

      if (!closestIntersection.has_value() || glm::distance(ray.origin(), worldClosestIntersectionPos) <
                                                glm::distance(ray.origin(), closestIntersection->position)) {
        closestIntersection.emplace(worldClosestIntersectionPos, index, intersectable.mesh);
      }
    }
  }

  return closestIntersection;
}
