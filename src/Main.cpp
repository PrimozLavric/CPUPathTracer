//
// Created by primoz on 20.7.2019.
//

#include <lsg/lsg.h>
#include "RayIntersector.h"

int main() {
  lsg::GLTFLoader loader;
  std::vector<std::shared_ptr<lsg::Scene>> scenes = loader.load("./resources/cornell_box.gltf");

  RayIntersector intersector(scenes[0]);
  auto isect = intersector.intersect(lsg::Ray<float>(glm::vec3(5.0, 0.2, -0.5), glm::vec3(-1.0, 0.0, 0.0)));
  int a = 0;
}