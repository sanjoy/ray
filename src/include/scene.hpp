#ifndef RAY_SCENE_HPP
#define RAY_SCENE_HPP

#include "object.hpp"

#include <memory>
#include <vector>

namespace ray {

class Scene {
  std::vector<std::unique_ptr<Object>> _objects;

public:
  void add_object(std::unique_ptr<Object> o) {
    _objects.push_back(std::move(o));
  }
  Color render_pixel(const Ray &r) const;
};
}

#endif
