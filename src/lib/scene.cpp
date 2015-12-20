#include "scene.hpp"

#include <limits>

using namespace ray;

Color Scene::render_pixel(const Ray &r, Context &ctx) const {
  double smallest_k = std::numeric_limits<double>::infinity();
  Color pixel;

  for (auto &o : _objects) {
    double k;
    Color c;
    if (o->incident(ctx, r, smallest_k, k, c) && k < smallest_k &&
        k >= 0.0) {
      smallest_k = k;
      pixel = c;
    }
  }

  return pixel;
}

void Scene::init_object_ids(Context &ctx) const {
  for (unsigned i = 0, e = _objects.size(); i != e; ++i) {
    _objects[i]->set_object_id(i);
    _objects[i]->initialize(ctx);
  }
}
