//
// Created by primoz on 21.7.2019.
//

#ifndef CPUPATHTRACER_RAYINTERSECTOR_H
#define CPUPATHTRACER_RAYINTERSECTOR_H

#include <lsg/lsg.h>

struct Intersection {
  Intersection(glm::vec3 position, uint32_t primitiveIndex, std::shared_ptr<lsg::SubMesh> mesh);

  glm::vec3 position;
  uint32_t primitiveIndex;
  std::shared_ptr<lsg::SubMesh> mesh;
};

class RayIntersector {
 public:
  explicit RayIntersector(const std::shared_ptr<lsg::Scene>& scene);

  std::optional<Intersection> intersect(const lsg::Ray<float>& ray) const;

 private:
  struct MeshIntersectable {
    explicit MeshIntersectable(glm::mat4 inWorldMat, std::shared_ptr<lsg::SubMesh> inMesh);

    glm::mat4 worldMat;
    glm::mat4 worldMatInverse;
    std::shared_ptr<lsg::SubMesh> mesh;
    std::shared_ptr<lsg::BVH<float>> bvh;
  };

  std::vector<MeshIntersectable> intersectables;
  std::shared_ptr<lsg::BVH<float>> bvh;
};

#endif // CPUPATHTRACER_RAYINTERSECTOR_H
