//
// Created by primoz on 21.7.2019.
//

#ifndef CPUPATHTRACER_RAYINTERSECTOR_HPP
#define CPUPATHTRACER_RAYINTERSECTOR_HPP

#include <lsg/lsg.h>

struct Intersection {
  Intersection(glm::vec3 position, glm::vec3 normal, uint32_t primitiveIndex, lsg::Ref<lsg::SubMesh> mesh);

  glm::vec3 position;
  glm::vec3 normal;
  uint32_t primitiveIndex;
  lsg::Ref<lsg::SubMesh> mesh;
};

class RayIntersector {
 public:
  explicit RayIntersector(const lsg::Ref<lsg::Scene>& scene);

  std::optional<Intersection> intersect(const lsg::Ray<float>& ray) const;

 protected:
  static glm::vec3 computeIntersectionNormal(const glm::vec3& intersectionPos, lsg::Triangle<glm::vec3> posTri,
                                             lsg::Triangle<glm::vec3> normalTri);

  static glm::vec3 computeBarycentric(const glm::vec3& position, lsg::Triangle<glm::vec3> triangle);
  ;

 private:
  struct MeshIntersectable {
    explicit MeshIntersectable(glm::mat4 inWorldMat, lsg::Ref<lsg::SubMesh> inMesh);

    glm::mat4 worldMat;
    glm::mat4 worldMatInverse;
    lsg::Ref<lsg::SubMesh> mesh;
    lsg::Ref<lsg::BVH<float>> bvh;
  };

  std::vector<MeshIntersectable> intersectables;
  lsg::Ref<lsg::BVH<float>> bvh;
};

#endif // CPUPATHTRACER_RAYINTERSECTOR_HPP
