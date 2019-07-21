//
// Created by primoz on 20.7.2019.
//

#include <lsg/lsg.h>

int main() {
  lsg::GLTFLoader loader;
  std::vector<std::shared_ptr<lsg::Scene>> scenes = loader.load("./testdata/lantern/Lantern.gltf");
}