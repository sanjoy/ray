#ifndef RAY_SCENE_HPP
#define RAY_SCENE_HPP

#include "object.hpp"

#include <vector>

namespace ray {

class Scene {
  std::vector<Object *> _objects;

public:
  void add_object(Object *o) { _objects.push_back(o); }
  Color render_pixel(const Ray &r) const;
};

}

#endif
