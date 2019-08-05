
#define GLM_ENABLE_EXPERIMENTAL
#include "RayIntersector.hpp"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <utility>

Intersection::Intersection(glm::vec3 position, glm::vec3 normal, uint32_t primitiveIndex, lsg::Ref<lsg::SubMesh> mesh)
  : position(position), normal(normal), primitiveIndex(primitiveIndex), mesh(std::move(mesh)) {}

RayIntersector::MeshIntersectable::MeshIntersectable(glm::mat4 inWorldMat, lsg::Ref<lsg::SubMesh> inMesh)
  : worldMat(inWorldMat), worldMatInverse(glm::inverse(worldMat)), mesh(std::move(inMesh)) {
  assert(mesh->geometry());

  lsg::bvh::SplitBVHBuilder<float> builder;
  bvh = builder.process(mesh->geometry()->getTrianglePositionAccessor());
}

RayIntersector::RayIntersector(const lsg::Ref<lsg::Scene>& scene) {
  for (const auto& rootObj : scene->children()) {
    rootObj->traverseDown([this](const lsg::Ref<lsg::Object>& object) {
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
    lsg::Ref<lsg::TriangleAccessor<glm::vec3>> triPosAccessor =
      intersectable.mesh->geometry()->getTrianglePositionAccessor();

    for (uint32_t j : objectIsectIndices) {
      std::optional<glm::vec3> triIsectPos =
        objectSpaceRay.intersectTriangle((*triPosAccessor)[j].a(), (*triPosAccessor)[j].b(), (*triPosAccessor)[j].c());

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
        // Compute normal.
        lsg::Ref<lsg::TriangleAccessor<glm::vec3>> triNormalAccessor =
          intersectable.mesh->geometry()->getTriangleNormalAccessor();

        glm::vec3 normal = glm::normalize(
          glm::mat3(intersectable.worldMat) *
          computeIntersectionNormal(closestIsectPos, (*triPosAccessor)[index], (*triNormalAccessor)[index]));
        closestIntersection.emplace(worldClosestIntersectionPos, normal, index, intersectable.mesh);
      }
    }
  }

  return closestIntersection;
}

glm::vec3 RayIntersector::computeIntersectionNormal(const glm::vec3& intersectionPos, lsg::Triangle<glm::vec3> posTri,
                                                    lsg::Triangle<glm::vec3> normalTri) {
  glm::vec3 barycentric = computeBarycentric(intersectionPos, posTri);

  return barycentric.x * normalTri.a() + barycentric.y * normalTri.b() + barycentric.z * normalTri.c();
}

glm::vec3 RayIntersector::computeBarycentric(const glm::vec3& position, lsg::Triangle<glm::vec3> triangle) {
  const glm::vec3& triA = triangle.a();
  glm::vec3 v0 = triangle.b() - triA, v1 = triangle.c() - triA, v2 = position - triA;
  float d00 = glm::dot(v0, v0);
  float d01 = glm::dot(v0, v1);
  float d11 = glm::dot(v1, v1);
  float d20 = glm::dot(v2, v0);
  float d21 = glm::dot(v2, v1);
  float denom = d00 * d11 - d01 * d01;

  float v = (d11 * d20 - d01 * d21) / denom;
  float w = (d00 * d21 - d01 * d20) / denom;
  float u = 1.0f - v - w;

  return glm::vec3(u, v, w);
}
