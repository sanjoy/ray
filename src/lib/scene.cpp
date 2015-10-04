#include "scene.hpp"

#include <limits>

using namespace ray;

Color Scene::render_pixel(const Ray &r) const {
  double smallest_k = std::numeric_limits<double>::infinity();
  Color pixel;

  for (Object *o : _objects) {
    double k;
    Color c;
    if (o->incident(*this, r, k, c) && k < smallest_k)
      pixel = c;
  }

  return pixel;
}
