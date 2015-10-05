#include "scene.hpp"

#include <limits>

using namespace ray;

Color Scene::render_pixel(const Ray &r) const {
  double smallest_k = std::numeric_limits<double>::infinity();
  Color pixel;

  for (auto &o : _objects) {
    double k;
    Color c;
    if (o->incident(*this, r, smallest_k, k, c) && k < smallest_k && k >= 0.0) {
      smallest_k = k;
      pixel = c;
    }
  }

  return pixel;
}
